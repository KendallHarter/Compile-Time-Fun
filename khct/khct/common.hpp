#ifndef KHCT_COMMON_HPP
#define KHCT_COMMON_HPP

#include <compare>
#include <concepts>
#include <utility>

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

namespace detail {

template<std::size_t I, auto First, auto... Rest>
consteval auto get_at_index() noexcept
{
   if constexpr (I == 0) {
      return First;
   }
   else {
      return get_at_index<I - 1, Rest...>();
   }
}

template<typename T>
struct type_holder {
   using type = T;
   friend constexpr auto operator<=>(const type_holder&, const type_holder&) noexcept = default;
};

template<typename T, std::size_t I>
struct tuple_base {
   [[no_unique_address]] T value;
   friend constexpr auto operator<=>(const tuple_base&, const tuple_base&) noexcept = default;
};

template<pair... TypesAndIndexes>
struct tuple_impl : tuple_base<typename decltype(TypesAndIndexes.first)::type, TypesAndIndexes.second>... {

   template<std::size_t I>
   constexpr const auto& get() const& noexcept
   {
      constexpr auto use_pair = get_at_index<I, TypesAndIndexes...>();
      return static_cast<const tuple_base<typename decltype(use_pair.first)::type, use_pair.second>*>(this)->value;
   }

   template<std::size_t I>
   constexpr auto& get() & noexcept
   {
      return const_cast<const tuple_impl*>(this)->get<I>();
   }

   template<std::size_t I>
   constexpr auto get() && noexcept
   {
      return get<I>();
   }

   friend constexpr auto operator<=>(const tuple_impl&, const tuple_impl&) noexcept = default;
};

template<typename... Ts, std::size_t... Is>
consteval auto make_tuple(std::index_sequence<Is...>) noexcept
{
   return static_cast<tuple_impl<pair{type_holder<Ts>{}, Is}...>*>(nullptr);
}

} // namespace detail

template<typename... Ts>
struct tuple : std::remove_reference_t<decltype(*detail::make_tuple<Ts...>(std::index_sequence_for<Ts...>{}))> {
   using base = std::remove_reference_t<decltype(*detail::make_tuple<Ts...>(std::index_sequence_for<Ts...>{}))>;

   // TODO: Move only types?
   constexpr tuple(const Ts&... values) : base{values...} {}

   friend constexpr auto operator<=>(const tuple&, const tuple&) noexcept = default;

   inline static constexpr auto size = sizeof...(Ts);
};

// TODO: The comparison operator sometimes doesn't work even though it should be
//       Notably the true_ and false_ structures won't match unless there is another type
//       that matches exactly (I have no idea why though)
template<typename... Ts1, typename... Ts2>
   requires((std::equality_comparable_with<Ts1, Ts2> && ...))
constexpr bool operator==(const tuple<Ts1...>& lhs, const tuple<Ts2...>& rhs) noexcept
{
   return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return ((lhs.template get<Is>() == rhs.template get<Is>()) && ...);
   }(std::index_sequence_for<Ts1...>{});
}

template<typename... Ts1, typename... Ts2>
constexpr tuple<Ts1..., Ts2...> tuple_cat(const tuple<Ts1...>& t1, const tuple<Ts2...>& t2) noexcept
{
   return [&]<std::size_t... Is1, std::size_t... Is2>(std::index_sequence<Is1...>, std::index_sequence<Is2...>) {
      return tuple<Ts1..., Ts2...>{t1.template get<Is1>()..., t2.template get<Is2>()...};
   }(std::index_sequence_for<Ts1...>{}, std::index_sequence_for<Ts2...>{});
}

template<std::size_t I, typename... Ts>
constexpr auto get(const tuple<Ts...> t) noexcept
{
   return t.template get<I>();
}

template<typename... Types>
concept has_common_type = requires() { typename std::common_type<Types...>::type; };

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
