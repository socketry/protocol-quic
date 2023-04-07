//
//  Session.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 7/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Session.hpp"

namespace Protocol
{
	namespace QUIC
	{
		namespace TLS
		{
			Session::Session()
			{
				ngtcp2_crypto_picotls_ctx_init(&_context);
			}
			
			Session::~Session()
			{
				ngtcp2_crypto_picotls_deconfigure_session(&_context);
				
				delete[] _context.handshake_properties.additional_extensions;
				
				if (_context.ptls) {
					ptls_free(_context.ptls);
				}
			}
			
			std::string Session::cipher_name() const {
				auto cipher = ptls_get_cipher(_context.ptls);
				return cipher->aead->name;
			}
			
			std::string Session::selected_protocol() const {
				auto protocol = ptls_get_negotiated_protocol(_context.ptls);
				
				if (protocol) {
					return protocol;
				} else {
					return {};
				}
			}
		}
	}
}
