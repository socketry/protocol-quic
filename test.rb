#!/usr/bin/env ruby

$LOAD_PATH.unshift(File.expand_path("lib", __dir__))

require 'socket'
require 'openssl'
require 'tempfile'
require 'timeout'
require 'protocol/quic'

HANDSHAKES = Queue.new

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
		HANDSHAKES << true
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
		server = Server.new(self, configuration, tls_context, socket, address, packet_header, nil)
		return server
	end
end

threads = []

tls_client_context = Protocol::QUIC::TLS::ClientContext.new
tls_client_context.add_protocol("txt")

tls_server_context = Protocol::QUIC::TLS::ServerContext.new
tls_server_context.add_protocol("txt")

key = OpenSSL::PKey::RSA.new(2048)
certificate = OpenSSL::X509::Certificate.new
certificate.version = 2
certificate.serial = 1
certificate.subject = OpenSSL::X509::Name.parse("/CN=localhost")
certificate.issuer = certificate.subject
certificate.public_key = key.public_key
certificate.not_before = Time.now
certificate.not_after = Time.now + 3600

extension_factory = OpenSSL::X509::ExtensionFactory.new
extension_factory.subject_certificate = certificate
extension_factory.issuer_certificate = certificate
certificate.add_extension(extension_factory.create_extension("basicConstraints", "CA:FALSE", true))
certificate.add_extension(extension_factory.create_extension("keyUsage", "digitalSignature,keyEncipherment", true))
certificate.add_extension(extension_factory.create_extension("subjectAltName", "DNS:localhost,IP:127.0.0.1,IP:::1", false))
certificate.sign(key, OpenSSL::Digest::SHA256.new)

certificate_file = Tempfile.new(["protocol-quic", ".crt"])
certificate_file.write(certificate.to_pem)
certificate_file.flush

key_file = Tempfile.new(["protocol-quic", ".key"])
key_file.write(key.to_pem)
key_file.flush

tls_server_context.load_certificate_file(certificate_file.path)
tls_server_context.load_private_key_file(key_file.path)

configuration = Protocol::QUIC::Configuration.new

addresses = Protocol::QUIC::Address.resolve("localhost", "4433", Socket::AF_UNSPEC, Socket::SOCK_DGRAM, Socket::AI_PASSIVE|Socket::AI_ADDRCONFIG)

addresses.each do |address|
	socket = Protocol::QUIC::Socket.new(address.family, Socket::SOCK_DGRAM, Socket::IPPROTO_UDP)
	socket.bind(address)
	dispatcher = Dispatcher.new(configuration, tls_server_context)
	
	threads << Thread.new do
		loop do
			dispatcher.listen(socket)
		end
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

Timeout.timeout(10) do
	addresses.length.times do
		HANDSHAKES.pop
	end
end

$stderr.puts "Completed #{addresses.length} QUIC handshakes"
exit(true)
