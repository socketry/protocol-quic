# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2023-2026, by Samuel Williams.

require "protocol/quic/version"

describe Protocol::QUIC do
	it "has a version" do
		expect(Protocol::QUIC::VERSION).to be =~ /\d+\.\d+\.\d+/
	end
end
