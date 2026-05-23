# frozen_string_literal: true

# Released under the MIT License.
# Copyright, 2026, by Samuel Williams.

require "openssl"
require "tempfile"

module Protocol
	module QUIC
		module Fixtures
			def self.certificate_files
				key = OpenSSL::PKey::RSA.new(2048)
				
				certificate = OpenSSL::X509::Certificate.new
				certificate.version = 2
				certificate.serial = 1
				certificate.subject = OpenSSL::X509::Name.parse("/CN=localhost")
				certificate.issuer = certificate.subject
				certificate.public_key = key.public_key
				certificate.not_before = Time.now
				certificate.not_after = Time.now + 3600
				
				extension_factory = OpenSSL::X509::ExtensionFactory.new
				extension_factory.subject_certificate = certificate
				extension_factory.issuer_certificate = certificate
				certificate.add_extension(extension_factory.create_extension("basicConstraints", "CA:FALSE", true))
				certificate.add_extension(extension_factory.create_extension("keyUsage", "digitalSignature,keyEncipherment", true))
				certificate.add_extension(extension_factory.create_extension("subjectAltName", "DNS:localhost,IP:127.0.0.1", false))
				certificate.sign(key, OpenSSL::Digest::SHA256.new)
				
				certificate_file = Tempfile.new(["protocol-quic", ".crt"])
				certificate_file.write(certificate.to_pem)
				certificate_file.flush
				
				key_file = Tempfile.new(["protocol-quic", ".key"])
				key_file.write(key.to_pem)
				key_file.flush
				
				return certificate_file, key_file
			end
		end
	end
end
