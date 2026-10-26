//
//  Connection.h
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Connection.hpp>
#include <Protocol/QUIC/TLS/Context.hpp>

#include <cstddef>
#include <unordered_map>

namespace Ruby::Protocol::QUIC {

	// A native Stream refers to its native Connection (its destructor calls Connection::remove_stream, and
	// ~Connection walks the remaining streams). Ruby may free a connection and its streams in the same GC cycle, in
	// any order, so the connection counts its live streams and defers its own deletion until the last one is gone.
	struct StreamOwner {
		std::size_t live_streams = 0;
		bool released = false;
		
		// The context of this connection's TLS session; see retain_context.
		::Protocol::QUIC::TLS::Context * tls_context = nullptr;

		virtual ~StreamOwner() = default;
	};

	// A TLS session refers to its context (~Session calls ptls_free, which uses the context). The context's Ruby
	// object may be freed before a connection that uses it, in the same GC cycle or earlier when the connection's
	// deletion is deferred, so contexts count the connections that use them and are deleted after the last one.
	struct ContextUsers {
		std::size_t count = 0;
		bool released = false;
	};

	inline std::unordered_map<const ::Protocol::QUIC::TLS::Context *, ContextUsers> & context_users()
	{
		static std::unordered_map<const ::Protocol::QUIC::TLS::Context *, ContextUsers> users;
		return users;
	}

	inline void retain_context(::Protocol::QUIC::TLS::Context * context)
	{
		context_users()[context].count += 1;
	}

	// A connection that used the context is gone.
	inline void release_context_user(::Protocol::QUIC::TLS::Context * context)
	{
		auto & users = context_users();
		auto iterator = users.find(context);
		if (iterator == users.end()) return;
		
		if (--iterator->second.count > 0) return;
		
		bool released = iterator->second.released;
		users.erase(iterator);
		
		if (released) delete context;
	}

	// The context's Ruby object is freed: delete it now, or after the last connection that uses it.
	inline void release_context(::Protocol::QUIC::TLS::Context * context)
	{
		auto & users = context_users();
		auto iterator = users.find(context);
		
		if (iterator != users.end() && iterator->second.count > 0) {
			iterator->second.released = true;
		} else {
			if (iterator != users.end()) users.erase(iterator);
			delete context;
		}
	}

	// Delete a connection, then let go of its TLS context.
	inline void destroy_connection(StreamOwner * connection)
	{
		auto context = connection->tls_context;
		delete connection;
		
		if (context) release_context_user(context);
	}

	// Delete a connection from its GC free function, or defer it while streams still refer to it.
	inline void release_connection(StreamOwner * connection)
	{
		if (connection->live_streams > 0) {
			connection->released = true;
		} else {
			destroy_connection(connection);
		}
	}

}

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Ruby_Protocol_QUIC_Connection;

extern const rb_data_type_t Ruby_Protocol_QUIC_Connection_type;

void Init_Ruby_Protocol_QUIC_Connection(VALUE Protocol_QUIC);

::Protocol::QUIC::Connection * Ruby_Protocol_QUIC_Connection_get(VALUE self);

#ifdef __cplusplus
}
#endif
