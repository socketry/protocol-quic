# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2023-2026, by Samuel Williams.

require "rbconfig"
require "rubygems"

TEAPOT = [RbConfig.ruby, "-rrubygems", "-e", "load Gem.activate_bin_path('teapot', 'teapot')", "--"]

task :default do
	ruby_library_directory = ENV.fetch("RUBYLIBDIR"){ENV.fetch("RUBYARCHDIR")}
	build_environment = {"RUBYLIBDIR" => ruby_library_directory}
	
	sh build_environment, *TEAPOT, "fetch"
	sh build_environment, *TEAPOT, "Ruby/Protocol/QUIC"
end
