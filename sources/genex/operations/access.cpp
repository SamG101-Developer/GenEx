module;
#include <genex/detail/macros.hpp>

export module genex.operations.access;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.prev;
import std;

namespace genex::operations::detail::concepts {
  template <typename Rng>
  concept accessible_range =
    random_access_range<Rng>;

  template <typename Rng>
  concept frontable_range =
    input_range<Rng>;

  template <typename Rng>
  concept backable_range =
    bidirectional_range<Rng>;
}

namespace genex::operations {
  struct at_fn {
    template <typename Rng>
    requires detail::concepts::accessible_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, const std::size_t n) const noexcept(
      SAFE_NOEX(rng[n]) and SAFE_CTOR(borrowed_reference_t<Rng>, decltype(rng[n])))
      -> borrowed_reference_t<Rng> {
      return rng[n];
    }
  };

  struct front_fn {
    template <typename Rng>
    requires detail::concepts::frontable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_NOEX(*iterators::begin(rng)) and
      SAFE_CTOR(borrowed_reference_t<Rng>, decltype(*iterators::begin(rng))))
      -> borrowed_reference_t<Rng> {
      return *iterators::begin(rng);
    }
  };

  struct back_fn {
    template <typename Rng>
    requires detail::concepts::backable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const noexcept(
      SAFE_NOEX(*iterators::prev(iterators::end(rng))) and
      SAFE_CTOR(borrowed_reference_t<Rng>, decltype(*iterators::prev(iterators::end(rng)))))
      -> borrowed_reference_t<Rng> {
      return *iterators::prev(iterators::end(rng));
    }
  };

  export inline constexpr at_fn at{};
  export inline constexpr front_fn front{};
  export inline constexpr back_fn back{};
}
