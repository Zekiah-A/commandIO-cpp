#pragma once

#include <string>

namespace commandIO {
	/**
	 * Print functions.
	 */
	using std::string;
	using std::to_string;

	/**
	 * Print a C string.
	 *
	 * \param io Input / output object.
	 * \param data C string.
	 */
	template <class I>
	void print(I& io, const char* data) {
	  string s {data};
	  io.write(s);
	}

	/**
	 * Print a string.
	 *
	 * \param io Input / output object.
	 * \param data String.
	 */
	template <class I>
	void print(I& io, string& data) {
	  io.write(data);
	}

	/**
	 * Print a value of basic type.
	 *
	 * \param io Input / output object.
	 * \param data Data.
	 */
	template <class I, class T>
	void print(I& io, T data) {
	  string s {to_string(data)};
	  io.write(s);
	}

	/**
	 * Print any number of values.
	 *
	 * \param io Input / output object.
	 * \param data Value to be printed.
	 * \param args Remaining values.
	 */
	template <class I, class H, class... Tail>
	void print(I& io, H data, Tail... args) {
	  print(io, data);
	  print(io, args...);
	}
}
