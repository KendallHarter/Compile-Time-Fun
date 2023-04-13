#ifndef KHCT_STRING_HPP
#define KHCT_STRING_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <concepts>
#include <ranges>
#include <string>

namespace khct {

// It would be nice to be able to make these functions generic across any statically sized structs,
// but it doesn't really seem possible and would be extremely complex if it were
template<std::size_t Size>
struct string {
   consteval string() : value{} {}

   consteval string(const char (&c)[Size]) : value{} { std::copy(c, c + Size, value); }

   // Need + 1 size for the null character
   template<std::size_t Start, std::size_t End>
   consteval string<End - Start + 1> splice() const noexcept
   {
      static_assert(End >= Start);
      static_assert(End <= Size);
      string<End - Start + 1> to_ret{};
      std::copy(value + Start, value + Start + End, to_ret.begin());
      return to_ret;
   }

   template<std::size_t Padsize, char Filler = ' '>
   consteval string<Padsize + Size> pad_left() const noexcept
   {
      string<Padsize + Size> to_ret{};
      std::fill(to_ret.value, to_ret.value + Padsize, Filler);
      std::ranges::copy(*this, to_ret.value + Padsize);
      return to_ret;
   }

   template<std::size_t Padsize, char Filler = ' '>
   consteval string<Padsize + Size> pad_right() const noexcept
   {
      string<Padsize + Size> to_ret{};
      std::ranges::copy(*this, to_ret.begin());
      std::fill(to_ret.begin() + Size - 1, to_ret.end() - 1, Filler);
      return to_ret;
   }

   constexpr auto begin() const noexcept { return value; }
   constexpr auto begin() noexcept { return value; }
   constexpr auto end() const noexcept { return &value[Size]; }
   constexpr auto end() noexcept { return &value[Size]; }
   constexpr auto size() noexcept { return Size; }

   char value[Size];
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

template<std::size_t Start, std::size_t End>
struct splice_struct {
   template<std::size_t Size>
   friend consteval auto operator|(const string<Size>& str, splice_struct<Start, End>) noexcept
   {
      return str.template splice<Start, End>();
   }
};

template<std::size_t Padsize, char Filler = ' '>
struct pad_left_struct {
   template<std::size_t Size>
   friend consteval auto operator|(const string<Size>& str, pad_left_struct<Padsize, Filler>) noexcept
   {
      return str.template pad_left<Padsize, Filler>();
   }
};

template<std::size_t Padsize, char Filler = ' '>
struct pad_right_struct {
   template<std::size_t Size>
   friend consteval auto operator|(const string<Size>& str, pad_right_struct<Padsize, Filler>) noexcept
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
