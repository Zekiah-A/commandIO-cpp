#pragma once

#include "args.hpp"
#include "print.hpp"
#include "types.hpp"

namespace commandIO {

	/// \defgroup help

	char const helpHelp[]{ "Help on a specific command.\n" };
	char const exitHelp[]{ "Exit.\n" };

	string _flagToString(bool value) {
		if (value) {
			return "enabled";
		}
		return "disabled";
	}

	/**
	 * Help on required parameters.
	 *
	 * \fn helpRequired(
	 *   void (*)(H, Tail...), Tuple<Tuple<const char*, const char*>, Args...>&)
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param f Function pointer.
	 * \param defs Parameter definitions.
	 */
	template <class I>
	void helpRequired(I &, void (*)(void), Empty) {}

	// Required parameter.
	template <class I, class H, class... Tail, class... Args>
	void helpRequired(
			I &io, void (*)(H, Tail...),
			Tuple<Tuple<const char *, const char *>, Args...> &defs) {
		H data{};
		print(
				io, "  ", defs.head.head, "\t\t", defs.head.tail.head, " (type ",
				typeOf(data), ")\n");

		void (*f_)(Tail...){};
		helpRequired(io, f_, defs.tail);
	}

	// Skip optional parameter.
	template <class I, class H, class... Tail, class D>
	void helpRequired(I &io, void (*)(H, Tail...), D &defs) {
		void (*f_)(Tail...){};
		helpRequired(io, f_, defs.tail);
	}

	/**
	 * Help on optional parameters.
	 *
	 * \fn helpOptional(void (*)(H, Tail...), D&) {
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param f Function pointer.
	 * \param defs Parameter definitions.
	 */
	template <class I>
	void helpOptional(I &, void (*)(void), Empty) {}

	// Skip required parameter.
	template <class I, class H, class... Tail, class... Args>
	void helpOptional(
			I &io, void (*)(H, Tail...),
			Tuple<Tuple<const char *, const char *>, Args...> &defs) {
		void (*f_)(Tail...){};
		helpOptional(io, f_, defs.tail);
	}

	// Optional parameter of type `flag`.
	template <class I, class... Tail, class D>
	void helpOptional(I &io, void (*)(bool, Tail...), D &defs) {
		print(
				io, "  ", defs.head.head, "\t\t", defs.head.tail.tail.head,
				" (type flag, default: ", _flagToString(defs.head.tail.head), ")\n");

		void (*f_)(Tail...){};
		helpOptional(io, f_, defs.tail);
	}

	// Optional parameter.
	template <class I, class H, class... Tail, class D>
	void helpOptional(I &io, void (*)(H, Tail...), D &defs) {
		H data{};
		print(
				io, "  ", defs.head.head, "\t\t", defs.head.tail.tail.head, " (type ",
				typeOf(data), ", default: ", defs.head.tail.head, ")\n");

		void (*f_)(Tail...){};
		helpOptional(io, f_, defs.tail);
	}

	/**
	 * Help on return type.
	 *
	 * \fn returnType(R (*)(FArgs...))
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param - Function pointer.
	 */
	template <class I, class... FArgs>
	void returnType(I &, void (*)(FArgs...)) {}

	// Entry point.
	template <class I, class R, class... FArgs>
	void returnType(I &io, R (*)(FArgs...)) {
		R data{};
		print(io, "\nreturns:\n  ", typeOf(data), "\n");
	}

	/**
	 * Give a full description of a command.
	 *
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param f Function pointer.
	 * \param name Command name.
	 * \param descr Command description.
	 * \param defs Parameter definitions.
	 */
	template <class I, class R, class... FArgs, class D>
	void help(I &io, R (*f)(FArgs...), string name, string descr, D &defs) {
		print(io, name, ": ", descr, "\n");

		int req;
		int opt;
		countArgs(req, opt, defs);

		if (req) {
			print(io, "\npositional arguments:\n");
			void (*f_)(FArgs...){};
			helpRequired(io, f_, defs);
		}

		if (opt) {
			print(io, "\noptional arguments:\n");
			void (*f_)(FArgs...){};
			helpOptional(io, f_, defs);
		}

		returnType(io, f);
	}

	/**
	 * Give a full description of a command.
	 *
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param m Tuple containing pointers to a class instance and a class member
	 *   function.
	 * \param name Command name.
	 * \param descr Command description.
	 * \param defs Parameter definitions.
	 */
	template <class I, class C, class R, class P, class... FArgs, class D>
	void help(
			I &io, Tuple<C *, R (P::*)(FArgs...)>,
			string name, string descr, D &defs) {
		R (*f_)
		(FArgs...){};
		help(io, f_, name, descr, defs);
	}

	/**
	 * Select a command for help.
	 *
	 * \fn selectHelp(string, H, Tail...)
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param name Command name.
	 * \param t Function definition under consideration.
	 * \param args Remaining function definitions.
	 *
	 * \return `true` on success, `false` otherwise.
	 */
	template <class I>
	bool selectHelp(I &io, string name) {
		bool result{ true };

		if (name == "help") {
			print(
					io, name, ": ", helpHelp, "\npositional arguments:\n",
					"  name\t\tcommand name (type string)\n");
		} else if (io.interactive && name == "exit") {
			print(io, name, ": ", exitHelp);
		} else {
			print(io, "Unknown command: ", name, "\n");
			result = false;
		}

		io.flush();

		return result;
	}

	// Entry point.
	template <class I, class H, class... Tail>
	bool selectHelp(I &io, string name, H t, Tail... args) {
		if (t.tail.head == name) {
			help(io, t.head, t.tail.head, t.tail.tail.head, t.tail.tail.tail);
			return true;
		}

		return selectHelp(io, name, args...);
	}

	/**
	 * Short description of all available functions.
	 *
	 * \fn describe(Args...)
	 * \ingroup help
	 *
	 * \param io Input / output object.
	 * \param args Function definitions.
	 */
	template <class I>
	void _describe(I &io) {
		print(io, "  help\t\t", helpHelp);
		if (io.interactive) {
			print(io, "  exit\t\t", exitHelp);
		}
		io.flush();
	}

	// Short description of one function.
	template <class I, class H, class... Tail>
	void _describe(I &io, H t, Tail... args) {
		print(io, "  ", t.tail.head, "\t\t", t.tail.tail.head, "\n");
		_describe(io, args...);
	}

	// Entry point.
	template <class I, class... Args>
	void describe(I &io, Args... args) {
		print(io, "Available commands:\n");
		_describe(io, args...);
	}
}
