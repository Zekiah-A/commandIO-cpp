#include <catch2/catch_test_macros.hpp>

#include "plugins/cli/io.hpp"

#define __call__(f, argv) f(sizeof(argv) / sizeof(const char*), (char**)argv)

#define main __doc_main__
#include "../examples/doc/doc.cpp"
#undef main

#define main __cli_single_main__
#include "../examples/cli-single/sequence.cpp"
#undef main

TEST_CASE("Documentation test", "[example]") {
	const char* argv[] = {"dummy", "8", "9"};

	__call__(__doc_main__, argv);
	// REQUIRE(_CIO.output == "17\n");
}

TEST_CASE("CLI test", "[example]") {
	const char* argv0[] = {"dummy", "11"};
	const char* argv1[] = {"dummy", "11", "-a", "3"};

	_CIO.clear_output(); // Clear previous output if _CIO supports it
	__call__(__cli_single_main__, argv0);
	REQUIRE(_CIO.output == "89\n");

	_CIO.clear_output();
	__call__(__cli_single_main__, argv1);
	REQUIRE(_CIO.output == "254\n");
}