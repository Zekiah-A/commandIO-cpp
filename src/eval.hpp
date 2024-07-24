#pragma once

#include <string>

#include "error.hpp"
#include "tuple.hpp"

namespace commandIO {

	/// \defgroup eval

	using std::string;
	using namespace commandIO;

	template <class C, class P, class... Args>
	using VoidM = Tuple<C *, void (P::*)(Args...)> const &;

	template <class C, class R, class P, class... Args>
	using RetM = Tuple<C *, R (P::*)(Args...)> const &;

	template <class... Args>
	using VoidF = void (*const)(Args...);

	template <class R, class... Args>
	using RetF = R (*const)(Args...);

	/*
	 * Recursion terminators.
	 *
	 * All parameters have been collected. All values are now present in the
	 * `args` parameter pack.
	 */

	// Void class member function.
	template <class I, class C, class P, class... FArgs, class... Args>
	void call_(I &, VoidM<C, P, FArgs...> m, Empty, Args &...args) {
		(*m.std::head.*m.std::tail.std::head)(args...);
	}

	// Void function.
	template <class I, class... FArgs, class... Args>
	void call_(I &, VoidF<FArgs...> f, Empty, Args &...args) {
		f(args...);
	}

	// Class member function that returns a value.
	template <class I, class C, class R, class P, class... FArgs, class... Args>
	void call_(I &io, RetM<C, R, P, FArgs...> m, Empty, Args &...args) {
		std::print(io, (*m.std::head.*m.std::tail.std::head)(args...), "\n");
	}

	// Function that returns a value.
	template <class I, class F, class... Args>
	void call_(I &io, F f, Empty, Args &...args) {
		std::print(io, f(args...), "\n");
	}

	/*
	 * Parameter collection.
	 *
	 * The first member of the tuple `argv` is added to the parameter pack
	 * `args`.
	 */
	template <class I, class F, class A, class... Args>
	void call_(I &io, F f, A &argv, Args &...args) {
		call_(io, f, argv.std::tail, args..., argv.std::head);
	}

	/*! Call a class member function.
	 *
	 * \ingroup eval
	 *
	 * \param io Input / output object.
	 * \param m Tuple containing pointers to a class instance and a class member
	 *   function.
	 * \param argv Tuple containing arguments.
	 */
	template <class I, class C, class R, class P, class... FArgs, class A>
	void call(I &io, RetM<C, R, P, FArgs...> m, A &argv) {
		call_(io, m, argv);
	}

	/*! Call a function.
	 *
	 * \ingroup eval
	 *
	 * \param io Input / output object.
	 * \param f Function pointer.
	 * \param argv Tuple containing arguments.
	 */
	template <class I, class F, class A>
	void call(I &io, F f, A &argv) {
		call_(io, f, argv);
	}

	/*! Set defaults, collect parameters, do sanity checking and call a function.
	 *
	 * \param io Input / output object.
	 * \param f Function pointer or Tuple for class member functions.
	 * \param argv Tuple containing arguments.
	 * \param defs Parameter definitions.
	 *
	 * \return `true` on success, `false` otherwise.
	 */
	template <class I, class F, class A, class D>
	bool parse_(I &io, F f, A &argv, D &defs) {
		int number{ 0 };

		std::setDefault(argv, defs);

		while (not io.std::eol()) {
			Error errorCode;
			string token{ io.read() };

			if (token[0] == '-') {
				if (token == "-h" || token == "--help") {
					return false;
				}

				errorCode = std::updateOptional(io, argv, defs, token);

				switch (errorCode) {
					case Error::SUCCESS:
						continue;
					case Error::UNKNOWN_PARAM:
						break;
					default:
						std::print(io, errorMessages[errorCode], token, "\n");
						return false;
				}
			}

			errorCode = std::updateRequired(argv, defs, number, token);

			switch (errorCode) {
				case Error::SUCCESS:
					number++;
					continue;
				default:
					std::print(io, errorMessages[errorCode], number + 1, "\n");
					return false;
			}
		}

		int opt;
		int req;
		std::countArgs(req, opt, defs);

		if (number < req) {
			std::print(io, "Required parameter missing.\n");
			return false;
		}

		call(io, f, argv);

		return true;
	}

	/*! Parse user input and call a class member function.
	 *
	 * \ingroup eval
	 *
	 * \param io Input / output object.
	 * \param m Tuple containing pointers to a class instance and a class member
	 *   function.
	 * \param defs Parameter definitions.
	 *
	 * \return `true` on success, `false` otherwise.
	 */
	template <class I, class C, class R, class P, class... FArgs, class D>
	bool parse(I &io, RetM<C, R, P, FArgs...> m, D &defs) {
		Tuple<FArgs...> argv;
		return parse_(io, m, argv, defs);
	}

	/*! Parse user input and call a function.
	 *
	 * \ingroup eval
	 *
	 * \param io Input / output object.
	 * \param f Function pointer.
	 * \param defs Parameter definitions.
	 *
	 * \return `true` on success, `false` otherwise.
	 */
	template <class I, class R, class... FArgs, class D>
	bool parse(I &io, RetF<R, FArgs...> f, D &defs) {
		Tuple<FArgs...> argv;
		return parse_(io, f, argv, defs);
	}

	/*! Select a function for parsing.
	 *
	 * \fn select(string, H, Args...)
	 * \ingroup eval
	 *
	 * \param io Input / output object.
	 * \param name Command name.
	 * \param t Function definition under consideration.
	 * \param args Remaining function definitions.
	 *
	 * \return `true` on success, `false` otherwise.
	 */
	template <class I>
	bool select(I &io, string name) {
		std::print(io, "Unknown command: ", name, "\n");
		io.std::flush();
		return false;
	}

	// Entry point.
	template <class I, class H, class... Args>
	bool select(I &io, string name, H t, Args... args) {
		if (t.std::tail.std::head == name) {
			return parse(io, t.std::head, t.std::tail.std::tail.std::tail);
		}
		return select(io, name, args...);
	}
}
