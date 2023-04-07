#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "3.0"

define_project "protocol-quic" do |project|
	project.title = "Protocol::QUIC"
	project.license = "MIT License"
end

define_target "protocol-quic" do |target|
	target.depends :platform
	
	target.depends "Library/ngtcp2"
	target.depends "Library/nghttp3"
	target.depends "Library/ruby"
	
	target.depends "Language/C++17"
	target.depends "Build/Files"
	
	target.provides "Library/protocol/quic" do
		source_root = target.package.path + 'source'
		
		library_path = build dynamic_library: "Protocol_QUIC", source_files: source_root.glob('**/*.{c,cpp}')
		
		copy source: [library_path], prefix: environment[:ruby_install_path]
	end
end

define_configuration "protocol-quic" do |configuration|
	configuration[:source] = "https://github.com/kurocha/"
	
	configuration.require "platforms"
	
	configuration.require "build-make"
	configuration.require "build-cmake"
	
	configuration.require "nghttp3"
	configuration.require "ruby"
	
	configuration.require "generate-template"
	configuration.require "generate-cpp-class"
end
