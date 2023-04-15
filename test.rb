
tls_client_context = Protocol::QUIC::TLS::ClientContext.new
tls_server_context = Protocol::QUIC::TLS::ServerContext.new
address = Protocol::QUIC::Address.new(host, port)

Thread.new do
	server = Protocol::QUIC::Server.new(tls_server_context)
	server.bind(address)
	server.listen(128)
	
	server.accept do |socket|
		socket.write("Hello World!")
		socket.close
	end
end

Thread.new do
	client = Protocol::QUIC::Client.new(tls_client_context)
	client.connect(address) do |socket|
		puts message: socket.read
		socket.close
	end
end
