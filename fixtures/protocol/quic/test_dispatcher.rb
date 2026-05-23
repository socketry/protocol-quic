# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic"
require "protocol/quic/test_server"

module Protocol::QUIC
	class TestDispatcher < Dispatcher
		def create_server(socket, address, packet_header)
			TestServer.new(self, configuration, tls_context, socket, address, packet_header, nil)
		end
	end
end
