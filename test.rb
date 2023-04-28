#!/usr/bin/env ruby

$LOAD_PATH.unshift(File.expand_path("lib", __dir__))

require 'socket'
require 'protocol/quic'

class Stream < Protocol::QUIC::Stream
end

class Server < Protocol::QUIC::Server
	def create_stream(stream_id)
		Stream.new(self, stream_id)
	end
end

class Client < Protocol::QUIC::Client
	def handshake_completed
		$stderr.puts "Handshake completed"
		# stream = open_bidirectional_stream
		# stream.output_buffer.append("Hello World")
		# stream.output_buffer.close
	end
	
	def create_stream(stream_id)
		Stream.new(self, stream_id)
	end
end

class Dispatcher < Protocol::QUIC::Dispatcher
	def create_server(socket, address, packet_header)
		$stderr.puts "Dispatcher create server"
		server = Server.new(self, configuration, tls_context, socket, address, packet_header, nil)
		server.send_packets
		return server
	end
end

threads = []

tls_client_context = Protocol::QUIC::TLS::ClientContext.new
tls_client_context.add_protocol("txt")

tls_server_context = Protocol::QUIC::TLS::ServerContext.new
tls_server_context.add_protocol("txt")

configuration = Protocol::QUIC::Configuration.new

addresses = Protocol::QUIC::Address.resolve("localhost", "4433", Socket::AF_UNSPEC, Socket::SOCK_DGRAM, Socket::AI_PASSIVE|Socket::AI_ADDRCONFIG)

dispatcher = Dispatcher.new(configuration, tls_server_context)

addresses.each do |address|
	socket = Protocol::QUIC::Socket.new(address.family, Socket::SOCK_DGRAM, Socket::IPPROTO_UDP)
	socket.bind(address)
	
	threads << Thread.new do
		dispatcher.listen(socket)
	end
end

addresses.each do |address|
	threads << Thread.new do
		socket = Protocol::QUIC::Socket.new(address.family, Socket::SOCK_DGRAM, Socket::IPPROTO_UDP)
		socket.connect(address)
		
		client = Client.new(configuration, tls_client_context, socket, address, 1)
		client.connect
	end
end

binding.irb

threads.each(&:join)
