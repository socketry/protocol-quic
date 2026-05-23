# frozen_string_literal: true

require_relative "lib/protocol/quic/version"

Gem::Specification.new do |spec|
	spec.name = "protocol-quic"
	spec.version = Protocol::QUIC::VERSION
	
	spec.summary = "QUIC protocol implementation using ngtcp2."
	spec.authors = ["Samuel Williams"]
	spec.license = "MIT"
	
	spec.cert_chain  = ["release.cert"]
	spec.signing_key = File.expand_path("~/.gem/release.pem")
	
	spec.homepage = "https://github.com/socketry/protocol-quic"
	
	spec.metadata = {
		"source_code_uri" => "https://github.com/socketry/protocol-quic.git",
	}
	
	spec.files = Dir["{ext,lib}/**/*", "*.md", base: __dir__]
	spec.require_paths = ["lib"]
	
	spec.extensions = ["ext/rakefile.rb"]
	
	spec.required_ruby_version = ">= 3.3"
	
	spec.add_dependency "teapot", "~> 3.5"
end
