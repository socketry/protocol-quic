//
//  ClientSession.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 7/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Session.hpp"
#include "ClientContext.hpp"

namespace Protocol
{
	namespace QUIC
	{
		namespace TLS
		{
			class ClientSession < Session
			{
			public:
				ClientSession(ClientContext &client_context, ngtcp2_conn *connection, std::string_view server_name);
				virtual ~ClientSession();
				
				const ngtcp2_conn * connection() const {return _connection;}
				ngtcp2_conn * connection() {return _connection;}
				
				bool early_data_accepted() const;
				
			private:
				ngtcp2_conn *_connection;
				ngtcp2_crypto_conn_ref _crypto_connection_reference;
				
			};
		}
	}
}
