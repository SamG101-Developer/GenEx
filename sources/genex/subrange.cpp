module;
#include <genex/detail/macros.hpp>

export module genex.subrange;
import genex.concepts;
import genex.iterators.access;
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
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      m_it(std::move(first)), m_st(std::move(last)) {
    }

    /// A whole range's iterators. Only one that does not dangle (an lvalue,
    /// or a borrowed range), as the subrange holds no range of its own.
    template <typename Rng>
    requires (not std::same_as<std::remove_cvref_t<Rng>, subrange>)
      and non_dangling_range<Rng>
      and std::convertible_to<iterator_t<Rng>, I> and std::convertible_to<sentinel_t<Rng>, S>
    GENEX_INLINE constexpr subrange(Rng &&rng) noexcept(
      SAFE_CALL(decltype(iterators::begin), Rng&) and
      SAFE_CALL(decltype(iterators::end), Rng&)) :
      m_it(iterators::begin(rng)), m_st(iterators::end(rng)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const noexcept(
      SAFE_CTOR(I, const I&)) -> I
      requires std::copyable<I> {
      return m_it;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() noexcept(
      SAFE_MOVE(I)) -> I
      requires (not std::copyable<I>) {
      return std::move(m_it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const noexcept(
      SAFE_CTOR(S, const S&)) -> S {
      return m_st;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto empty() const -> bool {
      return m_it == m_st;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const -> size_type
      requires std::sized_sentinel_for<S, I> {
      return static_cast<size_type>(m_st - m_it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto reserve_hint() const -> size_type
      requires (not std::sized_sentinel_for<S, I>) and
      requires(const I &i) { i.count(); } {
      return static_cast<size_type>(m_it.count());
    }

    [[nodiscard]] GENEX_INLINE constexpr auto data() const noexcept
      -> std::add_pointer_t<iter_element_t<I>>
    requires std::contiguous_iterator<I> {
      return std::to_address(m_it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto front() const -> reference
    requires std::forward_iterator<I> {
      GENEX_ASSERT(not empty());
      return *m_it;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto back() const -> reference
    requires std::bidirectional_iterator<I> and std::same_as<I, S> {
      GENEX_ASSERT(not empty());
      auto last = m_st;
      --last;
      return *last;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto operator[](
      const difference_type n) const -> reference
    requires std::random_access_iterator<I> {
      return m_it[n];
    }
  };

  export template <typename I, typename S>
  subrange(I, S) -> subrange<I, S>;

  export template <typename Rng>
  requires non_dangling_range<Rng>
  subrange(Rng &&) -> subrange<iterator_t<Rng>, sentinel_t<Rng>>;
}

template <typename I, typename S>
inline constexpr auto genex::enable_borrowed_range<genex::subrange<I, S>> = true;

template <typename I, typename S>
inline constexpr auto genex::enable_view<genex::subrange<I, S>> = true;
