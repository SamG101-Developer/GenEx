module;
#include <genex/detail/macros.hpp>

export module genex.views.address_of;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept addressable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::indirectly_readable<I> and
    requires(I it) { { std::addressof(*it) }; };

  template <typename Rng>
  concept addressable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  /**
   * The @c address_of_iterator yields the addresses of the elements pointed to by the underlying iterator. Note that
   * there is no special "sentinel" type for the @c address_of_iterator, because the end is always known from the
   * underlying range.
   * @tparam I The type of the underlying iterator.
   * @tparam S The type of the underlying sentinel.
   */
  template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
  struct address_of_iterator : genex::detail::adaptor_ops<address_of_iterator<I, S>> {
    I it;

    using value_type = std::add_pointer_t<iter_value_t<I>>;
    using reference = std::add_pointer_t<iter_value_t<I>>;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_concept = iter_strength_t<I>;
    using iterator_category = legacy_iter_category_t<reference, iterator_concept>;

    GENEX_INLINE constexpr address_of_iterator() noexcept = default;

    GENEX_INLINE constexpr explicit address_of_iterator(I first) noexcept(
      std::is_nothrow_move_constructible_v<I>) :
      it(std::move(first)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return std::addressof(*it);
    }
  };

  template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
  struct address_of_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr address_of_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return address_of_iterator<I, S>(it);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return address_of_iterator<I, S>(st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct address_of_fn {

    template <typename I, typename S>
      requires detail::concepts::addressable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(address_of_view, I, S) and SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::address_of_view(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::addressable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(address_of_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::address_of_view(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(address_of_fn)) {
      return meta::bind_back(address_of_fn{});
    }
  };

  export inline constexpr address_of_fn address_of{};
}
