# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "thread"

require "protocol/quic"
require "protocol/quic/test_stream"

module Protocol::QUIC
	class TestClient < Client
		def initialize(...)
			super
			@handshake_completed = Thread::Queue.new
		end
		
		attr :handshake_completed
		
		def handshake_completed
			@handshake_completed << true
		end
		
		def create_stream(stream_id)
			TestStream.new(self, stream_id)
		end
	end
end
