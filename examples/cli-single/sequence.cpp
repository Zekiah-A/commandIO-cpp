/*
 * Example CLI interface for one function.
 */

#include <commandIO.hpp>

using namespace commandIO;

int sequence(int a, int b, int n) {
  if (n < 0)
    return -99;
  if (n == 0)
    return a;
  if (n == 1)
    return b;
  return sequence(a, b, n - 1) + sequence(a, b, n - 2);
}


int main(int argc, char* argv[]) {
  CliIO io(argc, argv);

  interface(
    io,
    sequence, argv[0], "Give the n-th number in a sequence.",
      param("-a", 0, "first number"),
      param("-b", 1, "second number"),
      param("n", "desired number in the sequence"));

  return 0;
}
