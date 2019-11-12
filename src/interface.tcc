#ifndef USERIO_INTERFACE_TCC_
#define USERIO_INTERFACE_TCC_

/**
 * @file interface.tcc
 *
 * Template library for exporting native C and C++ functions as remote
 * procedure calls.
 *
 * For more information about (variadic) templates:
 * @li http://www.drdobbs.com/cpp/extracting-function-parameter-and-return/240000586
 * @li https://eli.thegreenplace.net/2014/variadic-templates-in-c/
 * @li https://en.cppreference.com/w/cpp/language/parameter_pack
 */

#include "args.tcc"
#include "eval.tcc"
#include "help.tcc"
#include "io.tcc"

#define param(args...) pack(args)
#define func(args...) pack(args)

RWIO IO;


/**
 * Interface.
 *
 * This function expects parameter pairs (function pointer, name).
 *
 * A command is read from stdin into @a command, if the value equals @a help,
 * we describe the list of functions. Otherwise, we call the function indexed
 * by @a command.
 *
 * @param args Parameter pairs (function pointer, name).
 *
 * @return @a true to continue @a false to quit.
 */
template <class F, class T, class... Args>
bool interface(F f, T name, const char* descr, Args... defs) {
  Tuple<Args...> t = pack(defs...);

  if (!parse(f, t)) {
    help(f, name, descr, t);
  }

  return true;
}


template <class... Args>
bool interface(Args... args) {
  string command;

  if (IO.interactive()) {
    IO.write("> ");
  }

  command = IO.read();

  if (command == "exit") {
    return false;
  }
  if (command == "help") {
    if (IO.eol() || !selectHelp(IO.read(), args...)) {
      describe(args...);
    }
    return true;
  }

  if (!select(command, args...)) {
    describe(args...);
  }

  return true;
}

#endif
