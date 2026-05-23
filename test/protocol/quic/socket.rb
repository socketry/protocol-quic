# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic/address"

describe Protocol::QUIC::Socket do
	let(:address) {Protocol::QUIC::Fixtures.address}
	let(:socket) {subject.new(address.family, ::Socket::SOCK_DGRAM, ::Socket::IPPROTO_UDP)}
	
	it "binds to a local address" do
		expect(socket.bind(address)).to be == 0
		expect(socket.local_address).to be_a(Protocol::QUIC::Address)
	end
	
	it "connects to a remote address" do
		server = subject.new(address.family, ::Socket::SOCK_DGRAM, ::Socket::IPPROTO_UDP)
		server.bind(address)
		
		expect(socket.connect(server.local_address)).to be == 0
		expect(socket.remote_address).to be_a(Protocol::QUIC::Address)
	end
end
