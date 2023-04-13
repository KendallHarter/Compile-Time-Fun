#include "khct/string.hpp"

using namespace khct;

static_assert(string{"hi"}.splice<0, 1>().pad_left<4>() == "    h");
static_assert((string{"hi"} | splice<0, 1>) == "h");
static_assert((string{"test"} | pad_left<2>) == "  test");

static_assert(string{"test"}.pad_right<2>() == "test  ");
static_assert((string{"test"} | pad_right<2>) == "test  ");

static_assert(string{"a"} + string{"Bc"} == "aBc");

int main() {}
