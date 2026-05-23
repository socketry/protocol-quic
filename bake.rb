# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2021, by Samuel Williams.

def build
	ext_path = File.expand_path("ext", __dir__)
	
	Dir.chdir(ext_path) do
		system("teapot", "fetch")
		system("teapot", "Ruby/Protocol/QUIC")
	end
end

def before_test
	build
end

def clean
	ext_path = File.expand_path("ext", __dir__)
	
	Dir.chdir(ext_path) do
		system("make clean")
	end
end

def console
	require 'socket'
	require_relative 'lib/protocol/quic'
	binding.irb
end
