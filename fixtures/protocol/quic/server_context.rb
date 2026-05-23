# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "protocol/quic"
require "protocol/quic/certificate"

module Protocol
	module QUIC
		module Fixtures
			def self.server_context(protocol = "txt")
				context = TLS::ServerContext.new
				context.add_protocol(protocol)
				
				certificate_file, key_file = self.certificate_files
				context.load_certificate_file(certificate_file.path)
				context.load_private_key_file(key_file.path)
				
				context
			end
		end
	end
end
