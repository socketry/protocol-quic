//
//  Client.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 5/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <cstdint>

#include <ngtcp2/ngtcp2_crypto_picotls.h>

namespace Protocol
{
	namespace QUIC
	{
		class Client
		{
		public:
			Client(std::uint32_t chosen_version, std::uint32_t original_version);
			virtual ~Client();
			
		private:
			std::uint32_t _chosen_version;
			std::uint32_t _original_version;
		
			// ngtcp2_crypto_conn_ref _connection_ref;
			// TLSClientSession _tls_session;
			
			// ngtcp2_conn *_connection;
			// ngtcp2_connection_close_error _last_error;
			// bool _ticket_received;
		};
	}
}
