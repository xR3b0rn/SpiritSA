
#include <utility>
#include <vector>
#include <string>

#include "SpiritSA.h"

struct A
{
  int64_t a, b, c;
  std::variant<int64_t, double> var;
  std::optional<int64_t> opt;
  std::vector<int64_t> vec;
};
struct B
{
  std::variant<int64_t, double> var;
  std::optional<recursive_wrapper<B>> b;
};

int main()
{
  std::string test = "asdf 1 2 3 4 5 6 7 8 9 1 asdf";
  A a;
  rule<const char*, A> r;
  auto ir = lit("asdf") > int_ > int_ > int_ > (int_ | double_) > -int_ > *int_ > lit("asdf");
  r %= ir;
  r.parse(&test[0], &test[0] + test.size(), a);

  test = "5 5";
  std::variant<int64_t, double> var;
  B b;
  rule<const char*, B> r2;
  auto ir2 = (int_ | double_);
  auto ir3 = ir2 > -r2;
  r2 %= ir3;
  r2.parse(&test[0], &test[0] + test.size(), b);
}
