#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "3.0"

define_project "protocol-quic" do |project|
	project.title = "Protocol::QUIC"
	project.license = "MIT License"
end

define_target "ruby" do |target|
	target.provides "Platform/ruby" do
		append header_search_paths [
			Build::Files::Path.new(RbConfig::CONFIG['rubyarchhdrdir']),
			Build::Files::Path.new(RbConfig::CONFIG['rubyhdrdir']),
		]
		
		ruby_library_directory = ENV.fetch('RUBYLIBDIR') do
			File.expand_path("../lib", __dir__)
		end
		
		ruby_install_path Build::Files::Path.new(ruby_library_directory)
	end
end

define_target "protocol-quic" do |target|
	target.depends :platform
	
	target.depends "Library/nghttp3"
	target.depends "Platform/ruby"
	
	target.depends "Language/C99"
	target.depends "Build/Files"
	
	target.provides "Library/protocol/quic" do
		source_root = target.package.path + 'source'
		
		library_path = build dynamic_library: "Protocol_QUIC", source_files: source_root.glob('**/*.c')
		
		copy source: [library_path], prefix: environment[:ruby_install_path]
	end
end

define_configuration "protocol-quic" do |configuration|
	configuration[:source] = "https://github.com/kurocha/"
	
	configuration.require "platforms"
	
	configuration.require "build-make"
	configuration.require "build-cmake"
	
	configuration.require "nghttp3"
	
	configuration.require "generate-template"
	configuration.require "generate-cpp-class"
end
