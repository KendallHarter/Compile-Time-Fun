#include "khct/common.hpp"

using namespace khct;

constexpr auto test_tuple = tuple<int, double>{1, 2.0};
static_assert(test_tuple.get<0>() == 1);
static_assert(test_tuple.get<1>() == 2.0);
constexpr auto tuple2 = tuple<double, int>{4.0, 4};
static_assert(tuple_cat(test_tuple, tuple2) == tuple{1, 2.0, 4.0, 4});

int main()
{
   // struct move_only_int {
   //    move_only_int(int val) : val{val} {}
   //    move_only_int(const move_only_int&) = delete;
   //    move_only_int(move_only_int&&) = default;
   //    move_only_int& operator=(const move_only_int&) = delete;
   //    move_only_int& operator=(move_only_int&&) = default;

   //    int val;
   // };
   // tuple<move_only_int> move_only_test{{10}};
   // [[maybe_unused]] int second = std::move(move_only_test.get<0>()).val;
}
