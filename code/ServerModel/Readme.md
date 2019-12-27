# Echo服务器常见模式

## [echo-iterative](./01.echo-iterative(one-process-one-thread).py)
- 单进程单线程IO阻塞模式

### Server
```
Server> python 01.echo-iterative(one-process-one-thread).py
```
此模型时最基本的服务器模式，只利用了`Socket`的基本网络函数，IO阻塞，通过循环监听客户端的动作。因此如果一个客户端连上了服务器，会因为循环完全占用服务器当前进程的资源，导致另一个客户端被阻塞。

### Client
```
Client1> telnet localhost 2007
```
客户端1可以正常连接服务器，并完成回显（echo）功能。在客户端1不断开的情况下，连接客户端2。
```
Client2> telnet localhost 2007
```
客户端2连接被阻塞，无法收到服务器的回显数据。此时断开客户端1与服务器的连接，客户端2马上连上了服务器，并收到服务器的回显信息（前面阻塞时，客户端2向服务器发送过数据）。因此可以证明，**客户端2被客户端1阻塞了**。

## [echo-single](./02.echo-single(one-process-one-thread).py)
- 单进程单线程IO阻塞模式

此服务器模式和 [echo-iterative](./01.echo-iterative(one-process-one-thread).py) 是一样的，单进程单线程，一个时刻只允许一个客户端连接。不同的是此模式用了python的`SocketServer`库，服务器的主循环已经被`TCPServer`取代，不用像[echo-iterative](./01.echo-iterative(one-process-one-thread).py) 一样，需要一个循环不断监控`accept`。但是由于所有`EchoHandler`属于一个线程，所以一个客户端会因为`while`循环阻塞另一个客户端。

## [echo-fork](./03.echo-fork(multi-process-one-thread).py)
- 多进程单线程IO阻塞模式

### Server
```
Server> python 03.echo-fork(multi-process-one-thread).py
```
此模式通过`ForkingTCPServer`为每个客户端连接创建一个新的进程，一个进程处理一个连接。这样客户端之间就不会互相阻塞干扰了。此模式需要消耗系统资源，这限制了客户端的个数。

### Client
```
Client1> telnet localhost 2007
```
```
Client2> telnet localhost 2007
```
可以同时启动多个客户端连接服务器，上面的客户端1和客户端2都可以完成回显功能。

## [echo-thread](./04.echo-thread(one-process-multi-thread).py)
- 单进程多线程IO阻塞模式

此模式在功能上和[echo-fork](./03.echo-fork(multi-process-one-thread).py)是一样的，都允许多个客户端连接，但是它是单进程多线程，而不是多进程单线程的。它通过`ThreadingTCPServer`为么个客户端创建一个新的线程，从而阻止客户端之间相互阻塞。但是由于线程之间是共享资源的，所以此模式要注意线程之间的相互竞争。

## [echo-poll](./06.echo-poll(one-process-one-thread-event-driven-IO-multiplexing).py)
- 单进程单线程IO-Multiplexing模式

此模式引入`Poll`机制，实现`event-driven`网络编程。达到在单进程单线程下也能实现多个客户端的连接。

## [TcpRelay](./05.tcpRelay(proxy).py)
- 多线程阻塞IO模式，代理服务器
- Echo服务器地址：localhost:2007
- 代理服务器地址：localhost:3007
- 客户端通过连接代理服务器和服务器通信。

代理服务器端通过`while`循环监听客户端连接，每当有一个客户端连接到来，代理服务器新建一个连接Echo服务器的`sock`和两个线程。
1. `sock`连接Echo服务器，`clientsocket`连接客户端。
2. 线程1由`clientsocket`接收客户端发过来的字符，并将这些字符通过`sock`发送给Echo服务器。
3. 线程2由`sock`接收Echo服务器发过来的字符，并将这些字符通过`clientsocket`发送给客户端。

* 为什么要用两个线程？<br>
一个线程处理客户端往服务器发数据，另一个线程处理服务器往客户端发数据。由于所有读数据都处于`while`循环中，所以只要由存在数据，一定会被读空，然后程序被阻塞在`recv`语句。<br>
因此我们需要两个线程。一个线程阻塞在`clientsocket`的`recv`语句上，等待客户端写入数据。另一个线程阻塞在`sock`的`recv`语句上，等待服务器写入数据。

### Echo服务器
```
Server>python 04.echo-thread(one-process-multi-thread).py
```
### 代理服务器
```
Proxy>python 05.tcpRelay(proxy).py
```
### 客户端1
```
Client1>telnet localhost 3007
```
### 客户端2
```
Client2>telnet localhost 3007
```
由于代理服务器和Echo服务器是单进程多线程服务器模式，所以支持多个客户端连接。
