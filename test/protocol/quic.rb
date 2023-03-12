# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2023, by Samuel Williams.

describe Protocol::QUIC do
	it "should have a version" do
		expect(Protocol::QUIC::VERSION).to be =~ /\d+\.\d+\.\d+/
	end
end
