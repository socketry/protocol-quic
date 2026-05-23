# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "socket"

require "protocol/quic"

module Protocol
	module QUIC
		module Fixtures
			def self.address
				Address.resolve("127.0.0.1", "0", ::Socket::AF_INET, ::Socket::SOCK_DGRAM, ::Socket::AI_PASSIVE).first
			end
		end
	end
end
