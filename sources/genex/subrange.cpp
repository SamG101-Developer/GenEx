module;
#include <genex/macros.hpp>

export module genex.subrange;
import genex.concepts;
import std;

namespace genex {
  export template <typename I, typename S = I>
    requires std::input_or_output_iterator<I> and std::sentinel_for<S, I>
  class subrange {
    GENEX_NO_UNIQUE_ADDRESS I m_it{};
    GENEX_NO_UNIQUE_ADDRESS S m_st{};

  public:
    using iterator = I;
    using sentinel = S;
    using value_type = iter_value_t<I>;
    using reference = iter_reference_t<I>;
    using difference_type = iter_difference_t<I>;
    using size_type = std::make_unsigned_t<difference_type>;

    subrange()
      requires std::default_initializable<I> and std::default_initializable<S>
    = default;

    GENEX_INLINE constexpr subrange(I first, S last) noexcept(
      std::is_nothrow_move_constructible_v<I> and std::is_nothrow_move_constructible_v<S>) :
      m_it(std::move(first)), m_st(std::move(last)) {
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto begin() const noexcept(
      std::is_nothrow_copy_constructible_v<I>) -> I
      requires std::copyable<I> {
      return m_it;
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto begin() noexcept(
      std::is_nothrow_move_constructible_v<I>) -> I
      requires (not std::copyable<I>) {
      return std::move(m_it);
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto end() const noexcept(
      std::is_nothrow_copy_constructible_v<S>) -> S {
      return m_st;
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto empty() const -> bool {
      return m_it == m_st;
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto size() const -> size_type
      requires std::sized_sentinel_for<S, I> {
      return static_cast<size_type>(m_st - m_it);
    }

    /**
     * A cheap upper bound on the length, in the spirit of C++26's @c ranges::reserve_hint. A
     * counted iterator knows how many steps it has left even when the range it walks cannot say
     * how many elements it holds -- which is exactly the shape @c take and @c slice produce over
     * a non-sized range. Only ever a sizing hint: use @c size() when an exact answer is needed.
     */
    GENEX_NODISCARD GENEX_INLINE constexpr auto reserve_hint() const -> size_type
      requires (not std::sized_sentinel_for<S, I>) and requires(const I &i) { i.count(); } {
      return static_cast<size_type>(m_it.count());
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto data() const noexcept -> std::add_pointer_t<iter_element_t<I>>
      requires std::contiguous_iterator<I> {
      return std::to_address(m_it);
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto front() const -> reference
      requires std::forward_iterator<I> {
      GENEX_ASSERT(std::out_of_range, not empty());
      return *m_it;
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto back() const -> reference
      requires std::bidirectional_iterator<I> and std::same_as<I, S> {
      GENEX_ASSERT(std::out_of_range, not empty());
      return *std::prev(m_st);
    }

    GENEX_NODISCARD GENEX_INLINE constexpr auto operator[](const difference_type n) const -> reference
      requires std::random_access_iterator<I> {
      return m_it[n];
    }
  };

  export template <typename I, typename S>
  subrange(I, S) -> subrange<I, S>;
}

template <typename I, typename S>
inline constexpr auto std::ranges::enable_borrowed_range<genex::subrange<I, S>> = true;

template <typename I, typename S>
inline constexpr auto std::ranges::enable_view<genex::subrange<I, S>> = true;
