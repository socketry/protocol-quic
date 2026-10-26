//
//  Callback.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//

#pragma once

#include <ruby.h>

#include <stdexcept>
#include <exception>
#include <system_error>

namespace Ruby::Protocol::QUIC {

	struct Invocation {
		VALUE receiver;
		ID method;
		int argc;
		const VALUE * argv;
	};

	inline VALUE invoke(VALUE argument)
	{
		auto invocation = reinterpret_cast<Invocation *>(argument);

		return rb_funcallv(invocation->receiver, invocation->method, invocation->argc, invocation->argv);
	}

	// Invoke a Ruby method from inside an ngtcp2 callback. A Ruby exception must not unwind through ngtcp2 or C++ frames, so it is captured here and converted into a C++ exception, which the callback wrappers turn into NGTCP2_ERR_CALLBACK_FAILURE.
	inline VALUE protected_call(VALUE receiver, const char * name, int argc = 0, const VALUE * argv = nullptr)
	{
		ID method = rb_intern(name);

		// Include private methods, like rb_funcall which used to call the hooks:
		if (!rb_obj_respond_to(receiver, method, TRUE)) return Qnil;

		Invocation invocation{receiver, method, argc, argv};
		int state = 0;

		VALUE result = rb_protect(invoke, reinterpret_cast<VALUE>(&invocation), &state);

		if (state) {
			VALUE error = rb_errinfo();
			rb_set_errinfo(Qnil);

			VALUE message = rb_funcall(error, rb_intern("full_message"), 0);
			rb_warn("%s: %s", name, StringValueCStr(message));

			throw std::runtime_error(name);
		}

		return result;
	}

	// Run native code from a Ruby method, converting C++ exceptions into Ruby exceptions so that they never escape into the Ruby VM.
	template <typename Function>
	inline VALUE guard(Function && function)
	{
		// rb_raise does not return, so no object with a destructor may be alive at that point:
		char message[256] = {0};

		try {
			return function();
		} catch (const std::exception & error) {
			snprintf(message, sizeof(message), "%s", error.what());
		}

		rb_raise(rb_eRuntimeError, "%s", message);
	}

}
