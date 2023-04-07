//
//  ClientSession.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 7/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "ClientSession.hpp"

namespace Protocol
{
	namespace QUIC
	{
		namespace TLS
		{
			static ngtcp2_conn* get_connection(ngtcp2_crypto_conn_ref *reference) {
  			auto client_session = static_cast<ClientSession *>(reference->user_data);
				
				return client_session->connection();
			}
			
			auto NEGOTIATED_PROTOCOLS = std::array<ptls_iovec_t, 1>{{
				{
				.base = "h3",
				.len = 2,
				},
			}};
			
			ClientSession::ClientSession(ClientContext &client_context, ngtcp2_conn *connection) : Session(), _connection(connection) _crypto_connection_reference{get_connection, this}
			{
				_context.ptls = ptls_client_new(client_context.native_handle());
				
				if (_context.ptls == nullptr) {
					throw std::runtime_error("Could not create client session");
				}
				
				*ptls_get_data_ptr(_context.ptls) = &_crypto_connection_reference;
				
				auto handshake_properties = _context.handshake_properties;
				handshake_properties.additional_extensions = new ptls_raw_extension_t[2]{
					{
						.type = UINT16_MAX,
					},
					{
						.type = UINT16_MAX,
					},
				};
				
				if (ngtcp2_crypto_picotls_configure_client_session(&_context, connection) != 0) {
					throw std::runtime_error("Could not configure client session");
				}
				
				handshake_properties.client.negotiated_protocols.list = NEGOTIATED_PROTOCOLS.data();
  			handshake_properties.client.negotiated_protocols.count = NEGOTIATED_PROTOCOLS.size();
				
				ptls_set_server_name(_context.ptls, server_name.c_str(), server_name.size());
			}
			
			ClientSession::~ClientSession()
			{
			}
			
			bool ClientSession::early_data_accepted() const {
				return _context.handshake_properties.client.early_data_acceptance == PTLS_EARLY_DATA_ACCEPTED;
			}
		}
	}
}
