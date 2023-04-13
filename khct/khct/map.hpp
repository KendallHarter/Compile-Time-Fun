#ifndef KHCT_MAP_HPP
#define KHCT_MAP_HPP

#include "khct/common.hpp"

#include <algorithm>
#include <compare>
#include <ranges>
#include <type_traits>
#include <utility>

namespace khct {

template<typename Key, typename Value, std::size_t Size, typename Comp>
   requires(std::is_empty_v<Comp>)
struct map {
   consteval map() noexcept : values{} {}

   consteval map(const std::pair<Key, Value> (&init)[Size]) noexcept
   {
      std::ranges::copy(init, std::begin(values));
      std::ranges::sort(values, Comp{});
   }

   constexpr std::optional<Value> operator[](const Key& k) const noexcept
   {
      const auto loc = std::lower_bound(
         std::begin(values), std::end(values), k, [](const auto& a, const auto& b) { return Comp{}(a.first, b); });
      if (loc == std::end(values)) {
         return std::nullopt;
      }
      return loc->second;
   }

   constexpr auto begin() const noexcept { return values; }
   constexpr auto begin() noexcept { return values; }
   constexpr auto end() const noexcept { return &values[Size]; }
   constexpr auto end() noexcept { return &values[Size]; }
   constexpr auto size() noexcept { return Size; }

   std::pair<Key, Value> values[Size];
   friend auto operator<=>(const map&, const map&) noexcept = default;
};

template<typename Key, typename Value, std::size_t Size1, std::size_t Size2, typename Comp>
consteval auto merge(const map<Key, Value, Size1, Comp>& lhs, const map<Key, Value, Size2, Comp>& rhs)
   -> map<Key, Value, Size1 + Size2, Comp>
{
   map<Key, Value, Size1 + Size2, Comp> to_ret;
   std::ranges::copy(lhs, to_ret.begin());
   std::ranges::copy(rhs, to_ret.begin() + Size1);
   std::ranges::sort(to_ret, Comp{});
   return to_ret;
}

template<typename Key, typename Value, std::size_t Size, typename Comp = std::less<void>>
   requires(std::is_empty_v<Comp>)
consteval auto make_map(const std::pair<Key, Value> (&init)[Size], Comp = std::less<void>{}) noexcept
   -> map<Key, Value, Size, Comp>
{
   return map<Key, Value, Size, Comp>{init};
}

template<typename Key, typename Comp, std::size_t Size, std::array<Key, Size> Keys, auto... Values>
   requires(sizeof...(Values) == Size && std::is_empty_v<Comp>)
struct multi_type_map {

   template<Key KeyToFind>
   consteval auto get() const noexcept
   {
      constexpr auto loc = std::lower_bound(
         std::begin(Keys), std::end(Keys), KeyToFind, [](const auto& a, const auto& b) { return Comp{}(a, b); });
      if constexpr (loc == std::end(Keys)) {
         return nil;
      }
      else {
         constexpr auto index = std::distance(std::begin(Keys), loc);
         return std::get<index>(std::tuple{Values...});
      }
   }

   friend auto operator<=>(const multi_type_map&, const multi_type_map&) noexcept = default;
};

template<pair... Pairs, typename Comp = std::less<void>>
   requires(all_same<decltype(Pairs.first)...> && std::is_empty_v<Comp>)
consteval auto make_multi_type_map(Comp = std::less<void>{})
{
   using key_type = head<decltype(Pairs.first)...>;
   return multi_type_map<key_type, Comp, sizeof...(Pairs), {Pairs.first...}, Pairs.second...>{};
}

} // namespace khct

#endif // KHCT_MAP_HPP
