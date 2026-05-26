# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2023-2026, by Samuel Williams.

require "rbconfig"
require "rubygems"

BUILD_GEMFILE = File.expand_path("gems.rb", __dir__)
BUNDLE = [RbConfig.ruby, Gem.bin_path("bundler", "bundle")]

task :default do
	ruby_library_directory = ENV.fetch("RUBYLIBDIR"){ENV.fetch("RUBYARCHDIR")}
	bundle_environment = {"BUNDLE_GEMFILE" => BUILD_GEMFILE}
	build_environment = bundle_environment.merge("RUBYLIBDIR" => ruby_library_directory)
	
	sh bundle_environment, *BUNDLE, "install"
	sh build_environment, *BUNDLE, "exec", "teapot", "fetch"
	sh build_environment, *BUNDLE, "exec", "teapot", "Ruby/Protocol/QUIC"
end
