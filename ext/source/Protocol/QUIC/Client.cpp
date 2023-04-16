//
//  Client.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 5/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Client.hpp"

#include <ngtcp2/ngtcp2.h>
#include <stdexcept>

#include <ngtcp2/ngtcp2_crypto.h>

namespace Protocol
{
	namespace QUIC
	{
		void random_callback(std::uint8_t *dest, std::size_t size, const ngtcp2_rand_ctx *context)
		{
			auto & random = *reinterpret_cast<Random*>(context->native_handle);
			random.generate(dest, size);
		}
		
		int get_new_connection_id_callback(ngtcp2_conn *conn, ngtcp2_cid *cid, uint8_t *token, size_t cidlen, void *user_data)
		{
			auto & client = *reinterpret_cast<Client*>(user_data);
			
			try {
				client.generate_connection_id(cid, cidlen, token);
			} catch (...) {
				return NGTCP2_ERR_CALLBACK_FAILURE;
			}
			
			return 0;
		}
		
		Client::Client(std::shared_ptr<TLS::ClientContext> tls_context, const ngtcp2_cid *dcid, const ngtcp2_cid *scid, const ngtcp2_path *path, std::uint32_t chosen_version, ngtcp2_settings *settings, ngtcp2_transport_params *transport_parameters) : _tls_context(tls_context), _chosen_version(chosen_version)
		{
			Random::generate_secret(_static_secret);
			settings->rand_ctx.native_handle = reinterpret_cast<void*>(&_random);
			
			auto callbacks = ngtcp2_callbacks{
				.client_initial = ngtcp2_crypto_client_initial_cb,
				.recv_crypto_data = ngtcp2_crypto_recv_crypto_data_cb,
				.encrypt = ngtcp2_crypto_encrypt_cb,
				.decrypt = ngtcp2_crypto_decrypt_cb,
				.hp_mask = ngtcp2_crypto_hp_mask_cb,
				.recv_retry = ngtcp2_crypto_recv_retry_cb,
				.rand = random_callback,
				.get_new_connection_id = get_new_connection_id_callback,
				.update_key = ngtcp2_crypto_update_key_cb,
				.delete_crypto_aead_ctx = ngtcp2_crypto_delete_crypto_aead_ctx_cb,
				.delete_crypto_cipher_ctx = ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
				.get_path_challenge_data = ngtcp2_crypto_get_path_challenge_data_cb,
				.version_negotiation = ngtcp2_crypto_version_negotiation_cb,
			};
			
			if (ngtcp2_conn_client_new(&_connection, dcid, scid, path, chosen_version, &callbacks, settings, transport_parameters, nullptr, this)) {
				throw std::runtime_error("Failed to create QUIC client connection!");
			}
			
			ngtcp2_conn_set_tls_native_handle(_connection, _tls_context->native_handle());
		}
		
		Client::~Client()
		{
			ngtcp2_conn_del(_connection);
		}
		
		// void Client::decode_early_transport_parameters(std::string_view buffer)
		// {
		// 	ngtcp2_conn_decode_early_transport_params(_connection, buffer.data(), buffer.size());
		// }
		
		void Client::generate_connection_id(ngtcp2_cid *cid, std::size_t cidlen, uint8_t *token)
		{
			Random::generate_secure(cid->data, cidlen);
			cid->datalen = cidlen;
			
			if (ngtcp2_crypto_generate_stateless_reset_token(token, _static_secret.data(), _static_secret.size(), cid) != 0) {
				throw std::runtime_error("Failed to generate stateless reset token!");
			}
		}
	}
}
