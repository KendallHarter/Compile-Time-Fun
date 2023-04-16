#ifndef KHCT_JSON_HPP
#define KHCT_JSON_HPP

#include <khct/common.hpp>
#include <khct/string.hpp>

#include <algorithm>
#include <cassert>
#include <compare>
#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>

namespace khct {

namespace json_error {

namespace detail {
struct number_too_large_struct {
   friend constexpr bool operator==(const number_too_large_struct&, const number_too_large_struct&) noexcept = default;
};
struct remaining_input_struct {
   friend constexpr bool operator==(const remaining_input_struct&, const remaining_input_struct&) noexcept = default;
};
struct unexpected_input_struct {
   friend constexpr bool operator==(const unexpected_input_struct&, const unexpected_input_struct&) noexcept = default;
};
struct invalid_double_struct {
   friend constexpr bool operator==(const invalid_double_struct&, const invalid_double_struct&) noexcept = default;
};
} // namespace detail

inline constexpr auto number_too_large = detail::number_too_large_struct{};
inline constexpr auto remaining_input = detail::remaining_input_struct{};
inline constexpr auto unexpected_input = detail::unexpected_input_struct{};
inline constexpr auto invalid_double = detail::invalid_double_struct{};

} // namespace json_error

constexpr bool is_json_error(...) { return false; }
constexpr bool is_json_error(json_error::detail::number_too_large_struct) { return true; }
constexpr bool is_json_error(json_error::detail::remaining_input_struct) { return true; }
constexpr bool is_json_error(json_error::detail::unexpected_input_struct) { return true; }
constexpr bool is_json_error(json_error::detail::invalid_double_struct) { return true; }

struct true_struct {
   friend constexpr bool operator==(const true_struct&, const true_struct&) noexcept = default;
   friend constexpr bool operator==(const true_struct&, bool b) noexcept { return b; }
   friend constexpr bool operator==(bool b, const true_struct&) noexcept { return b; }
   constexpr explicit operator bool() const noexcept { return true; }
};
struct false_struct {
   friend constexpr bool operator==(const false_struct&, const false_struct&) noexcept = default;
   friend constexpr bool operator==(const false_struct&, bool b) noexcept { return !b; }
   friend constexpr bool operator==(bool b, const false_struct&) noexcept { return !b; }
   // TODO: Should these be here?
   friend constexpr bool operator==(const false_struct&, const true_struct&) noexcept { return false; }
   friend constexpr bool operator==(const true_struct&, const false_struct&) noexcept { return false; }
   constexpr explicit operator bool() const noexcept { return false; }
};
struct null_struct {
   friend constexpr bool operator==(const null_struct&, const null_struct&) noexcept = default;
};

inline constexpr auto true_ = true_struct{};
inline constexpr auto false_ = false_struct{};
inline constexpr auto null = null_struct{};

namespace detail {

inline constexpr auto is_num = [](char c) { return c >= '0' && c <= '9'; };
inline constexpr auto is_nonzero_num = [](char c) { return c >= '1' && c <= '9'; };

// Pre: Leading whitespace is stripped
template<string Str>
consteval std::optional<std::uint64_t> to_unsigned_num(std::uint64_t max_value) noexcept
{
   if (!is_num(Str[0])) {
      return std::nullopt;
   }
   std::uint64_t to_ret = 0;
   for (auto iter = std::begin(Str); iter != std::end(Str) && is_num(*iter); ++iter) {
      const auto next_value = to_ret * 10 + *iter - '0';
      if (next_value > max_value || next_value < to_ret) {
         return std::nullopt;
      }
      to_ret = next_value;
   }
   return to_ret;
}

template<string Str>
consteval std::optional<std::int64_t> to_signed_num() noexcept
{
   constexpr auto has_minus = Str[0] == '-';
   constexpr auto no_minus = Str | splice<has_minus, Str.size()>;
   constexpr auto val = to_unsigned_num<no_minus>(std::numeric_limits<std::int64_t>::lowest());
   if constexpr (val) {
      constexpr auto ret_val = static_cast<std::int64_t>(*val);
      return has_minus ? -ret_val : ret_val;
   }
   else {
      return std::nullopt;
   }
}

template<string Str>
consteval std::optional<double> to_double() noexcept
{
   constexpr auto e_loc = std::ranges::find_if(Str, [](char c) { return c == 'e' || c == 'E'; });
   if constexpr (Str[0] == '-') {
      constexpr auto val = to_double<Str | splice<1, Str.size()>>();
      if constexpr (val) {
         return -*val;
      }
      else {
         return std::nullopt;
      }
   }
   else if constexpr (e_loc == std::end(Str)) {
      double to_ret = 0;
      auto iter = std::begin(Str);
      for (; iter != std::end(Str) && is_num(*iter); ++iter) {
         to_ret = to_ret * 10 + *iter - '0';
      }
      assert(*iter == '.');
      ++iter;
      double mult = 0.1;
      for (; iter != std::end(Str) && is_num(*iter); ++iter) {
         to_ret += mult * (*iter - '0');
         mult /= 10;
      }
      return to_ret;
   }
   else {
      constexpr auto e_index = std::distance(std::begin(Str), e_loc);
      constexpr auto left_side = to_signed_num<Str | splice<0, e_index>>();
      constexpr auto right_side_str = Str | splice<e_index + 1, Str.size()>;
      constexpr auto starts_plus = right_side_str[0] == '+';
      constexpr auto right_side = to_signed_num<right_side_str | splice<starts_plus, right_side_str.size()>>();
      if constexpr (!left_side || !right_side) {
         return std::nullopt;
      }
      else if constexpr (*right_side >= 0) {
         double mult = 1;
         for (double i = 0; i < *right_side; i += 1) {
            mult *= 10;
         }
         return *left_side * mult;
      }
      else {
         double mult = 1;
         for (double i = 0; i < -*right_side; i += 1) {
            mult *= .1;
         }
         return *left_side * mult;
      }
   }
}

consteval auto tuple_cat_or_propagate_error(auto so_far, auto rest) noexcept
{
   if constexpr (is_json_error(rest)) {
      return rest;
   }
   else {
      return std::tuple_cat(so_far, rest);
   }
}

// Pre: Leading whitespace is stripped
template<string Str>
consteval auto parse_json_value() noexcept
{
   if constexpr (Str[0] == '{') {
      // TODO: Object
   }
   else if constexpr (Str[0] == '[') {
      constexpr auto next = strip_leading_whitespace<Str | splice<1, Str.size()>>();
      if constexpr (next[0] == ']') {
         return pair{std::tuple{}, next | splice<1, next.size()>};
      }
      else {
         // TODO: Actual arrays
         return pair{json_error::unexpected_input, string{""}};
      }
   }
   else if constexpr (is_nonzero_num(Str[0]) || Str[0] == '-') {
      // Look ahead for a period or e to determine if it's a float
      constexpr auto end_number_loc = std::ranges::find_if_not(
         Str, [](char c) { return c == '-' || c == '.' || c == 'e' || c == '+' || is_num(c); });
      constexpr auto has_period_or_e
         = std::ranges::find_if(Str, [](char c) { return c == '.' || c == 'e' || c == 'E'; }) != std::end(Str);
      constexpr auto ret_str = Str | splice<std::distance(std::begin(Str), end_number_loc), Str.size()>;
      if constexpr (has_period_or_e) {
         constexpr auto val = to_double<Str>();
         if constexpr (val) {
            return pair{*val, ret_str};
         }
         else {
            return pair{json_error::invalid_double, string{""}};
         }
      }
      else {
         if constexpr (Str[0] == '-') {
            constexpr auto val = to_signed_num<Str>();
            if constexpr (val) {
               return pair{*val, ret_str};
            }
            else {
               return pair{json_error::number_too_large, string{""}};
            }
         }
         else {
            constexpr auto val = to_unsigned_num<Str>(std::numeric_limits<std::uint64_t>::max());
            if constexpr (val) {
               return pair{*val, ret_str};
            }
            else {
               return pair{json_error::number_too_large, string{""}};
            }
         }
      }
   }
   else if constexpr (std::string_view(Str.value_).starts_with("true")) {
      return pair{true_, Str | splice<4, Str.size()>};
   }
   else if constexpr (std::string_view(Str.value_).starts_with("false")) {
      return pair{false_, Str | splice<5, Str.size()>};
   }
   else if constexpr (std::string_view(Str.value_).starts_with("null")) {
      return pair{null, Str | splice<4, Str.size()>};
   }
   else {
      return pair{json_error::unexpected_input, string{""}};
   }
}

} // namespace detail

template<string Str>
consteval auto parse_json() noexcept
{
   constexpr auto ret_val_and_rest = detail::parse_json_value<strip_leading_whitespace<Str>()>();
   constexpr auto rest = ret_val_and_rest.second;
   if constexpr (strip_leading_whitespace<rest>().empty()) {
      return ret_val_and_rest.first;
   }
   else {
      return json_error::remaining_input;
   }
}

} // namespace khct

#endif // KHCT_JSON_HPP
