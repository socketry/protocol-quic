//
//  Client.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 5/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <cstdint>
#include <memory>

#include "TLS/ClientSession.hpp"

namespace Protocol
{
	namespace QUIC
	{
		class Client
		{
		public:
			Client(std::unique_ptr<TLS::ClientSession> client_session, std::uint32_t chosen_version, std::uint32_t original_version);
			virtual ~Client();
			
			void ticket_received() {_ticket_received = true;}
			
		protected:
			std::uint32_t _chosen_version;
			std::uint32_t _original_version;
		
			TLS::ClientSession _tls_client_session;
			
			bool _ticket_received = false;
			
			std::unique_ptr<ngtcp2_conn> _connection;
			ngtcp2_connection_close_error _last_error;
		};
	}
}
