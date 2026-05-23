# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic/address"

describe Protocol::QUIC::Address do
	it "resolves datagram addresses" do
		addresses = subject.resolve("127.0.0.1", "0", ::Socket::AF_INET, ::Socket::SOCK_DGRAM, ::Socket::AI_PASSIVE)
		
		expect(addresses).not.to be(:empty?)
		expect(addresses.first.family).to be == ::Socket::AF_INET
	end
end
