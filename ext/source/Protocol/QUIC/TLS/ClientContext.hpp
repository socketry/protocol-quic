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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif // HAVE_CONFIG_H

#include <picotls.h>
#include <picotls/openssl.h>

class TLSClientContext {
public:
  TLSClientContext();
  ~TLSClientContext();

  int init(const char *private_key_file, const char *cert_file);

  ptls_context_t *get_native_handle();

  void enable_keylog();

private:
  int load_private_key(const char *private_key_file);


};

#endif // TLS_CLIENT_CONTEXT_PICOTLS_H
