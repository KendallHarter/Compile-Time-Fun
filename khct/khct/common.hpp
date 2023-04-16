#ifndef KHCT_COMMON_HPP
#define KHCT_COMMON_HPP

#include <compare>
#include <concepts>

namespace khct {

struct nil_t {
   friend constexpr bool operator==(const nil_t&, const nil_t&) noexcept = default;
};
inline constexpr auto nil{nil_t{}};

/// @brief A simple pair to use as template parameters because std::pair can't be
/// @tparam T1 The first type
/// @tparam T2 The second type
template<typename T1, typename T2>
struct pair {
   [[no_unique_address]] T1 first;
   [[no_unique_address]] T2 second;
   friend constexpr auto operator<=>(const pair&, const pair&) noexcept = default;
};

template<typename T, typename... Rest>
inline constexpr bool all_same = (std::same_as<T, Rest> && ...);

namespace detail {

template<typename T, typename...>
struct head_struct {
   using type = T;
};

} // namespace detail

template<typename... Ts>
using head = detail::head_struct<Ts...>::type;

} // namespace khct

#endif // KHCT_COMMON_HPP
