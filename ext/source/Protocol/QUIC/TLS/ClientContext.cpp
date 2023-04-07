//
//  ClientContext.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 7/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "ClientContext.hpp"

#include <memory>
#include <stdexcept>

#include <openssl/pem.h>

namespace Protocol
{
	namespace QUIC
	{
		namespace TLS
		{
			ptls_key_exchange_algorithm_t *DEFAULT_KEY_EXCHANGES[] = {
				&ptls_openssl_x25519,
				&ptls_openssl_secp256r1,
				&ptls_openssl_secp384r1,
				&ptls_openssl_secp521r1,
				nullptr,
			};
			
			ptls_cipher_suite_t *DEFAULT_CIPHER_SUITS[] = {
				&ptls_openssl_aes128gcmsha256,
				&ptls_openssl_aes256gcmsha384,
				&ptls_openssl_chacha20poly1305sha256,
				nullptr,
			};
			
			ClientContext::ClientContext() :
				_context{
					.random_bytes = ptls_openssl_random_bytes,
					.get_time = &ptls_get_time,
					.key_exchanges = DEFAULT_KEY_EXCHANGES,
					.cipher_suites = DEFAULT_CIPHER_SUITS,
					.require_dhe_on_psk = 1,
				}
			{
				if (ngtcp2_crypto_picotls_configure_client_context(&_context) != 0) {
					throw std::runtime_error("ngtcp2_crypto_picotls_configure_client_context failed");
				}
			}
			
			ClientContext::~ClientContext()
			{
				if (_sign_certificate.key) {
					ptls_openssl_dispose_sign_certificate(&_sign_certificate);
				}
				
				for (size_t i = 0; i < _context.certificates.count; ++i) {
					free(_context.certificates.list[i].base);
				}
				free(_context.certificates.list);
			}
			
			void ClientContext::set_keylog(bool enabled)
			{
			}
			
			void ClientContext::load_certificate_file(const char * path)
			{
				if (ptls_load_certificates(&_context, path) != 0) {
					throw std::runtime_error(std::string("Could not load certificate file ") + path);
				}
			}
			
			template <typename Callback>
			struct Defer {
				Callback _callback;
				
				Defer(Callback callback) : _callback(callback) {}
				~Defer() {_callback();}
			};
			
			template <typename Callback>
			Defer<Callback> defer(Callback callback) {
				return Defer<Callback>(callback);
			}
			
			void ClientContext::load_private_key_file(const char * path)
			{
				auto file = fopen(path, "rb");
				if (file == nullptr) {
					throw std::runtime_error(std::string("Could not open private key file ") + path);
				}
				
				auto close_file = defer([&]{fclose(file);});
				
				auto private_key = PEM_read_PrivateKey(file, nullptr, nullptr, nullptr);
				if (private_key == nullptr) {
					throw std::runtime_error(std::string("Could not read private key file ") + path);
				}
				
				auto free_private_key = defer([&]{EVP_PKEY_free(private_key);});
				
				if (ptls_openssl_init_sign_certificate(&_sign_certificate, private_key) != 0) {
					throw std::runtime_error(std::string("Could not initialize sign certificate ") + path);
				}
				
				_context.sign_certificate = &_sign_certificate.super;
			}
		}
	}
}
