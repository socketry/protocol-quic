//
//  Defer.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 16/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

namespace Protocol
{
	namespace QUIC
	{
		template <typename Callback>
		struct Defer {
			Callback _callback;
			
			Defer(Callback callback) : _callback(callback) {}
			~Defer() {_callback();}
		};
		
		template <typename Callback>
		Defer<Callback> defer(Callback callback) {
			return Defer<Callback>(callback);
		}
	}
}
