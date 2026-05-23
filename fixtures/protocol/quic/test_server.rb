# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic"
require "protocol/quic/test_stream"

module Protocol::QUIC
	class TestServer < Server
		def create_stream(stream_id)
			TestStream.new(self, stream_id)
		end
	end
end
