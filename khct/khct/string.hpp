#ifndef KHCT_STRING_HPP
#define KHCT_STRING_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <concepts>
#include <ranges>
#include <string>
#include <tuple>

namespace khct {

// It would be nice to be able to make these functions generic across any statically sized structs,
// but it doesn't really seem possible and would be extremely complex if it were
template<std::size_t RawArraySize>
struct string {
   consteval string() : value_{} {}

   consteval string(const char (&c)[RawArraySize]) : value_{} { std::copy(c, c + RawArraySize, value_); }

public:
   // Need + 1 size for the null character
   template<std::size_t Start, std::size_t End>
   consteval string<End - Start + 1> splice() const noexcept
   {
      static_assert(End >= Start);
      static_assert(End <= RawArraySize);
      string<End - Start + 1> to_ret{};
      std::copy(value_ + Start, value_ + End, to_ret.begin());
      return to_ret;
   }

   template<std::size_t Padsize, char Filler = ' '>
   consteval string<Padsize + RawArraySize> pad_left() const noexcept
   {
      string<Padsize + RawArraySize> to_ret{};
      std::fill(to_ret.value_, to_ret.value_ + Padsize, Filler);
      std::ranges::copy(*this, to_ret.value_ + Padsize);
      return to_ret;
   }

   template<std::size_t Padsize, char Filler = ' '>
   consteval string<Padsize + RawArraySize> pad_right() const noexcept
   {
      string<Padsize + RawArraySize> to_ret{};
      std::ranges::copy(*this, to_ret.begin());
      std::fill(to_ret.begin() + RawArraySize - 1, to_ret.end() - 1, Filler);
      return to_ret;
   }

   constexpr auto begin() const noexcept { return value_; }
   constexpr auto begin() noexcept { return value_; }
   constexpr auto end() const noexcept { return &value_[RawArraySize]; }
   constexpr auto end() noexcept { return &value_[RawArraySize]; }
   constexpr auto size() const noexcept { return RawArraySize - 1; }

   constexpr char operator[](std::size_t index) const noexcept { return value_[index]; }

   char value_[RawArraySize];
   friend auto operator<=>(const string&, const string&) = default;
};

// I couldn't define this as a inline friend for some reason, unfortunately
template<std::size_t SizeL, std::size_t SizeR>
consteval auto operator+(const string<SizeL>& lhs, const string<SizeR>& rhs) noexcept -> string<SizeL + SizeR - 1>
{
   string<SizeL + SizeR - 1> to_ret;
   std::ranges::copy(lhs, to_ret.begin());
   std::ranges::copy(rhs, to_ret.begin() + SizeL - 1);
   return to_ret;
}

namespace detail {

template<string Str, char SplitChar, std::size_t MaxSplits>
consteval auto split_helper() noexcept
{
   constexpr auto split_loc = std::ranges::find(Str, SplitChar);
   if constexpr (split_loc == std::end(Str)) {
      return std::make_tuple(Str);
   }
   else {
      constexpr auto split_index = std::distance(std::begin(Str), split_loc);
      constexpr auto split = Str.template splice<0, split_index>();
      constexpr auto rest = Str.template splice<split_index + 1, Str.size()>();
      if constexpr (MaxSplits == 1) {
         return std::make_tuple(split, rest);
      }
      else {
         constexpr auto next_split_num = MaxSplits == 0 ? 0 : MaxSplits - 1;
         return std::tuple_cat(std::make_tuple(split), split_helper<rest, SplitChar, next_split_num>());
      }
   }
}

} // namespace detail

template<string Str, char SplitChar, std::size_t MaxSplits = 0>
consteval auto split() noexcept
{
   return detail::split_helper<Str, SplitChar, MaxSplits>();
}

namespace detail {

template<std::size_t Start, std::size_t End>
struct splice_struct {
   template<std::size_t Size>
   consteval auto operator()(const string<Size>& str) const noexcept
   {
      return str.template splice<Start, End>();
   }

   template<std::size_t Size>
   friend consteval auto operator|(const string<Size>& str, splice_struct) noexcept
   {
      return str.template splice<Start, End>();
   }
};

template<std::size_t Padsize, char Filler = ' '>
struct pad_left_struct {
   template<std::size_t Size>
   consteval auto operator()(const string<Size>& str) const noexcept
   {
      return str.template pad_left<Padsize, Filler>();
   }

   template<std::size_t Size>
   friend consteval auto operator|(const string<Size>& str, pad_left_struct) noexcept
   {
      return str.template pad_left<Padsize, Filler>();
   }
};

template<std::size_t Padsize, char Filler = ' '>
struct pad_right_struct {
   template<std::size_t Size>
   consteval auto operator()(const string<Size>& str) const noexcept
   {
      return str.template pad_right<Padsize, Filler>();
   }

   template<std::size_t Size>
   friend consteval auto operator|(const string<Size>& str, pad_right_struct) noexcept
   {
      return str.template pad_right<Padsize, Filler>();
   }
};

} // namespace detail

template<std::size_t Start, std::size_t End>
inline constexpr auto splice = detail::splice_struct<Start, End>{};

template<std::size_t Padsize, char Filler = ' '>
inline constexpr auto pad_left = detail::pad_left_struct<Padsize, Filler>{};

template<std::size_t Padsize, char Filler = ' '>
inline constexpr auto pad_right = detail::pad_right_struct<Padsize, Filler>{};

} // namespace khct

#endif // KHCT_STRING_HPP
