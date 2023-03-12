# frozen_string_literal: true

describe Protocol::QUIC do
	it "should have a version" do
		expect(Protocol::QUIC::VERSION).to be =~ /\d+\.\d+\.\d+/
	end
end
