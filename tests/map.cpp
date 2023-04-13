#include "khct/map.hpp"

using namespace khct;

constexpr auto map1 = make_map<int, int>({{3, 4}, {4, 5}});
constexpr auto map2 = make_map<int, int>({{5, 6}, {6, 7}});

static_assert(map1[3] == 4);
static_assert(map1[4] == 5);
static_assert(!map1[10]);

constexpr auto map3 = merge(map1, map2);
static_assert(map3[3] == 4);
static_assert(map3[6] == 7);

int main() {}
