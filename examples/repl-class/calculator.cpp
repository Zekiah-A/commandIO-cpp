#include <commandIO.hpp>

using namespace commandIO;


class Calculator {
  public:
    Calculator(void) {}
    void add(int amount) {
      _total += amount;
    }
    void subtract(int amount) {
      _total -= amount;
    }
    int show(void) {
      return _total;
    }
  private:
    int _total = 0;
};


int main(void) {
  ReplIO io;
  Calculator calc;

  while (interface(
    io,
    func(pack(&calc, &Calculator::add), "add", "Increase the total.",
      param("amount", "increase by this amount")),
    func(pack(&calc, &Calculator::subtract), "sub", "Decrease the total.",
      param("amount", "decrease by this amount")),
    func(pack(&calc, &Calculator::show), "show", "Show total.")));

  return 0;
}
