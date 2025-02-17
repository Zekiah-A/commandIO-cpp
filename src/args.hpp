#pragma once

#include "error.hpp"
#include "tuple.hpp"
#include "types.hpp"

namespace commandIO {

	/// \defgroup args

	template <class... Args>
	using Def = Tuple<Tuple<char const *, char const *>, Args...> const &;

	/*! Count parameters.
	 *
	 * \fn countArgs(int&, int&, D&)
	 * \ingroup args
	 *
	 * \param[in, out] req Number of required parameters.
	 * \param[in, out] opt Number optional parameters.
	 * \param[in] defs Parameter definitions.
	 */
	inline void countArgs_(int &, int &, EmptyC) {}

	// Count required parameter.
	template <class... Args>
	void countArgs_(int &req, int &opt, Def<Args...> defs) {
		countArgs_(++req, opt, defs.tail);
	}

	// Count optional parameter.
	template <class D>
	void countArgs_(int &req, int &opt, D const &defs) {
		countArgs_(req, ++opt, defs.tail);
	}

	// Entry point.
	template <class D>
	void countArgs(int &req, int &opt, D const &defs) {
		countArgs_(req = 0, opt = 0, defs);
	}

	/*! Set default values.
	 *
	 * \fn setDefault(A&, D&)
	 * \ingroup args
	 *
	 * \param[out] argv Arguments.
	 * \param[in] defs Parameter definitions.
	 */
	inline void setDefault(Empty, EmptyC) {}

	// Set required argument.
	template <class A, class... Args>
	void setDefault(A &argv, Def<Args...> defs) {
		setDefault(argv.tail, defs.tail);
	}

	// Set optional argument.
	template <class A, class D>
	void setDefault(A &argv, D const &defs) {
		argv.head = defs.head.tail.head;
		setDefault(argv.tail, defs.tail);
	}

	/*! Update a required argument.
	 *
	 * \fn updateRequired(A&, D&, int, string&)
	 * \ingroup args
	 *
	 * \param[out] argv Arguments.
	 * \param[in] defs Parameter definitions.
	 * \param[in] num Argument number to update.
	 * \param[in] count Parameter number under consideration.
	 * \param[in] value Value.
	 *
	 * \return success on success, an error code otherwise.
	 */
	inline Error updateRequired_(
			Empty, EmptyC, int const, int const, string const &) {
		return Error::EXCESS_PARAM;
	}

	// Update a required argument.
	template <class A, class... Args>
	Error updateRequired_(
			A &argv, Def<Args...> defs,
			int const num, int const count, string const &value) {
		if (num == count) {
			if (not convert(&argv.head, value)) {
				return Error::INVALID_PARAM_TYPE;
			}
			return Error::SUCCESS;
		}

		return updateRequired_(argv.tail, defs.tail, num, count + 1, value);
	}

	template <class T, class... Args>
	Error updateRequired_(
			Tuple<vector<T>> &argv, Def<Args...>,
			int const, int const, string const &value) {
		if (not convert(&argv.head, value)) {
			return Error::INVALID_PARAM_TYPE;
		}
		return Error::SUCCESS;
	}

	// Skip optional parameters.
	template <class A, class D>
	Error updateRequired_(
			A &argv, D const &defs, int const num, int const count,
			string const &value) {
		return updateRequired_(argv.tail, defs.tail, num, count, value);
	}

	// Entry point.
	template <class A, class D>
	Error updateRequired(
			A &argv, D const &defs, int const num, string const &value) {
		return updateRequired_(argv, defs, num, 0, value);
	}

	/*! Update an optional parameter value.
	 *
	 * \fn updateOptional(Tuple<H, Tail...>&, D&, string&)
	 * \ingroup args
	 *
	 * \param[in, out] io Input / output object.
	 * \param[in, out] argv Arguments.
	 * \param[in] defs Parameter definitions.
	 * \param[in] name Parameter name under consideration.
	 *
	 * \return success on success, an error code otherwise.
	 */
	template <class I>
	Error updateOptional(I &, Empty, EmptyC, string const &) {
		return Error::UNKNOWN_PARAM;
	}

	// Update flag parameter.
	template <class I, class... Tail, class D>
	Error updateOptional(
			I &io, Tuple<bool, Tail...> &argv, D const &defs, string const &name) {
		if (defs.head.head == name) {
			argv.head = not argv.head;
			return Error::SUCCESS;
		}

		return updateOptional(io, argv.tail, defs.tail, name);
	}

	// Update optional parameter.
	template <class I, class H, class... Tail, class D>
	Error updateOptional(
			I &io, Tuple<H, Tail...> &argv, D const &defs, string const &name) {
		if (defs.head.head == name) {
			if (io.eol()) {
				return Error::MISSING_VALUE;
			}
			if (not convert(&argv.head, io.read())) {
				return Error::INVALID_PARAM_TYPE;
			}
			return Error::SUCCESS;
		}
		return updateOptional(io, argv.tail, defs.tail, name);
	}
}
