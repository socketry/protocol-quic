# frozen_string_literal: true

require_relative "lib/protocol/quic/version"

Gem::Specification.new do |spec|
	spec.name = "protocol-quic"
	spec.version = Protocol::QUIC::VERSION
	spec.authors = ["Samuel Williams"]
	spec.license = "MIT"
	
	spec.summary = "QUIC protocol implementation using ngtcp2."
	
	spec.files = Dir["{lib,ext}/**/*", "LICENSE", "README.md"]
	
	spec.required_ruby_version = ">= 2.7"
	
	spec.extensions = ["ext/ngtcp2/CMakeLists.txt"]
end
