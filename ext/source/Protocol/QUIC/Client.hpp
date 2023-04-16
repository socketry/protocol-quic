//
//  Client.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 5/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Random.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

#include <ngtcp2/ngtcp2.h>
#include "TLS/ClientSession.hpp"

namespace Protocol
{
	namespace QUIC
	{
		class Client
		{
		public:
			Client(std::shared_ptr<TLS::ClientContext> tls_context, const ngtcp2_cid *dcid, const ngtcp2_cid *scid, const ngtcp2_path *path, std::uint32_t chosen_version, ngtcp2_settings *settings, ngtcp2_transport_params *transport_parameters);
			virtual ~Client();
			
			ngtcp2_conn* native_handle() {return _connection;}
			
			std::uint64_t maximum_local_unidirectional_streams() const {
				return ngtcp2_conn_get_max_local_streams_uni(_connection);
			}
			
			virtual void decode_early_transport_parameters(std::string_view data);
			
			virtual void generate_connection_id(ngtcp2_cid *cid, std::size_t cidlen, uint8_t *token);
			
		protected:
			std::shared_ptr<TLS::ClientContext> _tls_context;
			
			Random _random;
			std::array<uint8_t, 32> _static_secret;
			
			ngtcp2_conn *_connection;
			ngtcp2_connection_close_error _last_error;
			
			std::uint32_t _chosen_version;
		};
	}
}
