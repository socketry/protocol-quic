# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic/address"
require "protocol/quic/client_context"
require "protocol/quic/server_context"
require "protocol/quic/test_client"
require "protocol/quic/test_dispatcher"
require "timeout"

describe Protocol::QUIC::Server do
	let(:configuration) {Protocol::QUIC::Configuration.new}
	let(:server_context) {Protocol::QUIC::Fixtures.server_context}
	let(:client_context) {Protocol::QUIC::Fixtures.client_context}
	let(:address) {Protocol::QUIC::Fixtures.address}
	
	def bound_socket(address)
		socket = Protocol::QUIC::Socket.new(address.family, ::Socket::SOCK_DGRAM, ::Socket::IPPROTO_UDP)
		socket.bind(address)
		socket
	end
	
	it "can complete a client/server handshake" do
		server_socket = bound_socket(address)
		local_address = server_socket.local_address
		dispatcher = Protocol::QUIC::TestDispatcher.new(configuration, server_context)
		
		listener = Thread.new do
			loop do
				dispatcher.listen(server_socket)
			end
		end
		
		client_socket = Protocol::QUIC::Socket.new(local_address.family, ::Socket::SOCK_DGRAM, ::Socket::IPPROTO_UDP)
		client_socket.connect(local_address)
		
		client = Protocol::QUIC::TestClient.new(configuration, client_context, client_socket, local_address, 1)
		
		client_thread = Thread.new do
			client.connect
		end
		
		Timeout.timeout(10) do
			expect(client.handshake_completed.pop).to be == true
		end
		
		GC.start
		GC.compact
		
		expect(client_thread.join(1)).to be_nil
	ensure
		listener&.kill
		listener&.join
		client_thread&.kill
		client_thread&.join
	end
end
