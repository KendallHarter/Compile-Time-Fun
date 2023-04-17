#include "khct/map.hpp"
#include "khct/string.hpp"

using namespace khct;

constexpr auto map1 = make_map<int, int>({{3, 4}, {4, 5}});
constexpr auto map2 = make_map<int, int>({{6, 7}, {5, 6}});

static_assert(map1[3] == 4);
static_assert(map1[4] == 5);
static_assert(!map1[10]);

constexpr auto map3 = merge(map1, map2);
static_assert(map3[3] == 4);
static_assert(map3[6] == 7);

constexpr auto type_map = make_multi_type_map<{1, 4}, {3, 2.2}, {2, true}>();

static_assert(type_map.get<2>() == true);

constexpr auto lex_comp = [](auto a, auto b) { return std::ranges::lexicographical_compare(a, b); };

constexpr auto type_map2
   = make_multi_type_map<{string{"banana"}, 2}, {string{"alpaca"}, 'a'}, {string{"test"}, 't'}>(lex_comp);

static_assert(type_map2.get<string{"alpaca"}>() == 'a');
static_assert(type_map2.get<string{"banana"}>() == 2);
static_assert(type_map2.get<string{"test"}>() == 't');

int main() {}
