# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2023-2026, by Samuel Williams.

require "rbconfig"
require "rubygems"

task :default do
	ruby_library_directory = ENV.fetch("RUBYLIBDIR"){ENV.fetch("RUBYARCHDIR")}
	build_environment = {"RUBYLIBDIR" => ruby_library_directory}
	teapot = [RbConfig.ruby, Gem.bin_path("teapot", "teapot")]
	
	sh build_environment, *teapot, "fetch"
	sh build_environment, *teapot, "Ruby/Protocol/QUIC"
end
