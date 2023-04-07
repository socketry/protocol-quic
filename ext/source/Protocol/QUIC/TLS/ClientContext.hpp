//
//  ClientContext.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 7/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <picotls.h>
#include <picotls/openssl.h>

#include <ngtcp2/ngtcp2.h>
#include <ngtcp2/ngtcp2_crypto.h>
#include <ngtcp2/ngtcp2_crypto_picotls.h>

namespace Protocol
{
	namespace QUIC
	{
		namespace TLS
		{
			class ClientContext
			{
			public:
				ClientContext();
				virtual ~ClientContext();
				
				void set_keylog(bool enabled);
				
				ptls_context_t * native_handle() {return &_context;}
				
				void load_certificate_file(const char * path);
				void load_private_key_file(const char * path);
			private:
				ptls_context_t _context;
				ptls_openssl_sign_certificate_t _sign_certificate;
			};
		}
	}
}
