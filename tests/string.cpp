#include "khct/string.hpp"

using namespace khct;

static_assert(string{"hi"}.splice<0, 1>().pad_left<4>() == "    h");
static_assert((string{"hi"} | splice<0, 1>) == "h");
static_assert(splice<0, 1>(string{"hi"}) == "h");
static_assert(splice<1, 2>(string{"hi"}) == "i");
static_assert((string{"test"} | pad_left<2>) == "  test");

static_assert(string{"test"}.pad_right<2>() == "test  ");
static_assert((string{"test"} | pad_right<2>) == "test  ");
static_assert(pad_right<2>(string{"test"}) == "test  ");

static_assert(string{"a"} + string{"Bc"} == "aBc");

static_assert(split<string{"a,b,c"}, ','>() == std::tuple{string{"a"}, string{"b"}, string{"c"}});
static_assert(split<string{"a,b,c"}, ',', 1>() == std::tuple{string{"a"}, string{"b,c"}});

int main() {}
