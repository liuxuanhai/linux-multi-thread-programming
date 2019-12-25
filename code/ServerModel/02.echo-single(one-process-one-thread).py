from SocketServer import BaseRequestHandler
from SocketServer import TCPServer

class EchoHandler(BaseRequestHandler):
	def handle(self):
		print "got connenction from", self.client_address
		while True:
			data = self.request.recv(4096)
			if data:
				sent = self.request.send(data)
			else:
				print "disconnect", self.client_address
				self.request.close()
				break

if __name__ == "__main__":
	listen_address = ("0.0.0.0", 2007)
	server = TCPServer(listen_address, EchoHandler)
	server.serve_forever()