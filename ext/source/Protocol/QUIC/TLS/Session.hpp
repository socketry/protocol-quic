//
//  Session.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 7/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ngtcp2/ngtcp2_crypto_picotls.h>
#include <picotls.h>

namespace Protocol
{
	namespace QUIC
	{
		namespace TLS
		{
			class Session
			{
			public:
				Session();
				virtual ~Session();
				
				ngtcp2_crypto_picotls_ctx *native_handle() {return &_context;}
				
				std::string cipher_name() const;
				std::string selected_protocol() const;
				
			private:
				ngtcp2_crypto_picotls_ctx _context;
			};
		}
	}
}
