#include <catch2/catch_test_macros.hpp>

#include "plugins/repl/io.hpp"

#define main __repl_class_main__
#include "../examples/repl-class/calculator.cpp"
#undef main

TEST_CASE("Class test", "[example]") {
	const char* argv[] = {"add", "6\n", "show"};

	_RIO.prepare(3, (char**)argv);

	__repl_class_main__();

	REQUIRE(_RIO.output == "> > 6\n> ");
}
