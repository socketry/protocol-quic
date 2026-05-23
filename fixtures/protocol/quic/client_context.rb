# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic"

module Protocol
	module QUIC
		module Fixtures
			def self.client_context(protocol = "txt")
				context = TLS::ClientContext.new
				context.add_protocol(protocol)
				
				context
			end
		end
	end
end
