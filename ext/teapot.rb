#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "3.0"

define_project "ruby-protocol-quic" do |project|
	project.title = "Protocol::QUIC"
	project.license = "MIT License"
end

define_target "ruby-protocol-quic" do |target|
	target.depends "Language/C++17"
	
	target.depends "Library/Protocol/QUIC"
	target.depends "Library/ruby"
	
	target.depends "Build/Files"
	target.depends "Build/Compile/Commands"
	
	target.provides "Ruby/Protocol/QUIC" do
		source_root = target.package.path + 'source'
		
		library_path = build dynamic_library: "Protocol_QUIC", source_files: source_root.glob('**/*.{c,cpp}')
		
		copy source: [library_path], prefix: environment[:ruby_install_path]
		
		compile_commands destination_path: (source_root + "compile_commands.json")
	end
end

define_configuration "ruby-protocol-quic" do |configuration|
	configuration[:source] = "https://github.com/kurocha/"
	
	configuration.require "platforms"
	
	configuration.require "build-make"
	configuration.require "build-cmake"
	
	configuration.require "scheduler"
	configuration.require "protocol-quic"
	configuration.require "ruby"
	
	configuration.require "generate-template"
	configuration.require "generate-cpp-class"
	
	configuration.require "build-compile-commands"
end
