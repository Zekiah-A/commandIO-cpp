#pragma once

#include "interface.hpp"

// I/O plugins.
#include "plugins/cli/io.hpp"
#include "plugins/repl/io.hpp"
#include "tuple.hpp"

namespace commandIO {
	/**
	 * RPC interface.
	 *
	 * This function expects parameter pairs (function pointer, documentation).
	 *
	 * \param io Input / output object.
	 * \param args Parameter pairs (function pointer, documentation).
	 */
	template <class I, class... Args>
	bool interface(I &io, Args... args) {
		return commandInterface(io, args...);
	}

	/**
	 * Recursion terminator for `interface()`.
	 *
	 * \private
	 */
	template <class... Args>
	bool interface(Tuple<>, Args...) {
		return true;
	}

	/**
	 * Multiple RPC interfaces.
	 *
	 * \param t Tuple of input / output objects.
	 * \param args Parameter pairs (function pointer, documentation).
	 */
	template <class H, class... Tail, class... Args>
	bool interface(Tuple<H, Tail...> t, Args... args) {
		return commandInterface(*t.head, args...) && interface(t.tail, args...);
	}
}
