# 第7章 moduo编程示例
## 7.6 再muduo中实现Protobuf编解码与消息分发器
### 7.6.1 什么是编解码器（codec）
* 没有消息(message)，只有字节流数据，这时候是用不到codec的，如：echo服务。
* non-trival的网络服务程序会以消息为单位通信，每条消息由明确的长度与边界，如：asio chat服务。
* `Codec`类的责任：
    - 把`TcpConnection`的数据喂给它
    - 然后向它注册`onXXXMessage()`回调
* `Dispathcer`类的责任：
    - 拦截`ProtobufCodec`的callback，按消息类型分派给多个callbacks

## 7.11 简单的消息广播
* 对比一个Go语言实现的类似例子：[publish-subscribe](https://github.com/yuxiang660/Advanced-Go-Programming/tree/master/code/grpc/publish-subscribe)。可以明显感觉到`muduo`在效率上的优势。
* 对比[moduo-hub](https://github.com/chenshuo/muduo/blob/master/examples/hub/hub.cc)和[go-pubsub](https://github.com/yuxiang660/Advanced-Go-Programming/blob/master/code/grpc/publish-subscribe/pubsub-server.go)
    - 两者都是主服务器，维护者所有信息。
    - `go-pubsub`提供RPC服务，客户端通过RPC与其通信；`muduo-hub`提供TCP服务，客户端通过`socket`的读写与其通讯。<br>
    RPC通信需要将函数签名和数据打包发给服务器，由服务器解析；而`socket`只需要将数据发包写给`socket`即可。这也是后者效率更高的一个原因。<br>
    - 对于TCP连接，服务器如何知道应该调用`publish`还是`subscribe`呢？<br>
    `moduo::PubSubClient`通过给传输数据加入不同的头部信息，以表示此数据是来自`publisher`还是`subscriber`。
    - 对于RPC服务，服务器如何一直保持与`subscriber`的连接？<br>
    `go-pubsub`利用了`gRPC`的流特性，在服务器和客户端之间建立起了一个流连接。服务器某个子线程在阻塞在`subscriber channel`上，当`chan`收到`publish`来的信息后，此子线程会将数据`send`到这个stream流上。而客户端就可以读取这个流上的数据了。
* 对比[moduo-pub](https://github.com/chenshuo/muduo/blob/master/examples/hub/pub.cc)和[go-publisher-client](https://github.com/yuxiang660/Advanced-Go-Programming/blob/master/code/grpc/publish-subscribe/publisher-client.go)
    - 两者都是`publisher`客户端，用于发布信息
    - `go-publisher-client`直接远程调用服务器的`publish`方法完成发布。`moduo-pub`由于是TCP连接，只能通过特定的函数，在特定的时机和服务器通信，所以并没有前者直观。`moduo-pub`在连接时的回调函数中调用了`socket`的`send`函数，将信息发布出去。
* 对比[moduo-sub](https://github.com/chenshuo/muduo/blob/master/examples/hub/sub.cc)和[go-subscriber-client](https://github.com/yuxiang660/Advanced-Go-Programming/blob/master/code/grpc/publish-subscribe/subscriber-client.go)
    - 两者都是`subscriber`客户端，会订阅主题和接收发布的信息
    - `go-subscriber-client`调用远程`subscribe`方法订阅相关主题，而`moduo-sub`是在连接的回调函数中间接的调用`send`方法，将命令发送给服务，完成订阅。
    - `go-subscriber-client`订阅完后会得到一个`gRPC`流连接，客户端就通过循环读取此流连接和服务保持联系。而`moduo-sub`注册订阅完后会一直保持和服务器的TCP连接，当有数据从服务器到来是，会执行到`onMessage`的回调函数，从而得到数据。
* 整体对比TCP通信和RPC通信
    - TCP连接只提供有限的通信函数，客户端需要在回调函数中完成各种业务逻辑。而RPC可以提供各种自定义的通信函数，对客户端编程更加友好。但是带来的代价就是，通信负载比TCP连接大，需要传递更多的信息给服务器。
    - 正常情况，RPC通信应该是一次性的短连接，调用完一个远程函数后应该就断开连接，下次如果还需要调用，需要再次连接。而TCP通信可以建立一个长连接，在一个统一的模型中实现事件驱动的服务器/客户端通信机制。发布者订阅者模式中，订阅者需要和服务器保持长时间的连接，以便一直接收信息。此时，RPC通信需要利用`gRPC`的“流特性”，建立起一个长连接。这种不统一的沟通机制，不仅不便于管理，也会对效率产生影响。TCP连接只需要在一个循环中监控`EPoll`事件，而RPC此时需要在多个循环中，监控不同的流。

## 8.13 epoll
* `epoll`是Linux独有的高效的IO multiplexing机制，它与`poll`的不同之处主要在于`poll`每次返回整个文件描述符数组，用户代码需要遍历数组以找到哪些文件描述符上有IO事件（`revents > 0`）。而`epoll_wait`返回的是活动fd的列表，需要遍历的数组通常会小得多。


