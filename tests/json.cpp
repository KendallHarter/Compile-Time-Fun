#include "khct/json.hpp"

using namespace khct;

// Numbers
static_assert(parse_json<"20 ">() == 20);
static_assert(parse_json<" -30">() == -30);

// Floats
constexpr auto abs_ = [](auto x) { return x < 0 ? -x : x; };
static_assert(parse_json<"3.25">() == 3.25);
static_assert(parse_json<"-3.5">() == -3.5);
static_assert(parse_json<"1e20">() == 1e20);
static_assert(parse_json<"-5e+4">() == -5e+4);
static_assert(abs_(parse_json<"25e-2">() - 25e-2) <= 0.0001);

// Strings
static_assert(parse_json<R"( "hi" )">() == string{"hi"});
static_assert(parse_json<R"( "\"hi\"" )">() == string{R"(\"hi\")"});

// Special values
static_assert(parse_json<"true">() == true);
static_assert(parse_json<"false">() == false);
static_assert(parse_json<"null">() == null);

// Arrays
static_assert(parse_json<"[]">() == std::tuple{});
static_assert(parse_json<"[[]]">() == std::tuple{std::tuple{}});
static_assert(parse_json<"[1]">() == std::tuple{1});
static_assert(parse_json<"[1, 2]">() == std::tuple{1, 2});
static_assert(parse_json<"[true, false, null]">() == std::tuple{true, false, null});

// Objects
static_assert(parse_json<"{}">().get<"">() == nil);
static_assert(parse_json<R"({"hi": 4})">().get<"hi">() == 4);
static_assert(parse_json<R"({"hi": 202, "sup": true})">().get<"sup">() == true);
// constexpr auto test_map = parse_json<R"({
//    "array": [1, 2, 3],
//    "float": 1.2e10,
//    "object": {
//       "int": 20
//    }
// })">();

// Errors
static_assert(parse_json<"9999999999999999999999999999999999999999999999">() == json_error::number_too_large);
static_assert(parse_json<"2ee20">() == json_error::invalid_double);
static_assert(parse_json<"2,3">() == json_error::remaining_input);
static_assert(parse_json<"a">() == json_error::unexpected_input);

int main() {}
