module;
#include <genex/macros.hpp>

export module genex.views2.zip_with;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.views2.transform;
import genex.views2.zip;
import std;

namespace genex::views::detail::concepts {
  template <typename Rng1, typename Rng2, typename F>
  concept zip_withable_range =
    input_range<Rng1> and
    input_range<Rng2> and
    std::invocable<F&, range_reference_t<Rng1>, range_reference_t<Rng2>>;
}

namespace genex::views::detail::impl {
  template <typename F>
  struct zip_applier {
    GENEX_NO_UNIQUE_ADDRESS F f;

    template <typename Tup>
    GENEX_INLINE constexpr auto operator()(Tup &&tup) const -> decltype(auto) {
      return std::apply(f, std::forward<Tup>(tup));
    }
  };
}

namespace genex::views {
  struct zip_with_fn {
    template <typename Rng1, typename Rng2, typename F>
      requires detail::concepts::zip_withable_range<Rng1, Rng2, F>
    GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, F f) const noexcept(
      SAFE_CALL(decltype(zip), Rng1&, Rng2&) and SAFE_MOVE(F)) {
      return transform(
        zip(rng1, rng2),
        detail::impl::zip_applier<F>{std::move(f)});
    }

    template <typename Rng2, typename F>
      requires (input_range<Rng2> and not range<F>)
    GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, F f) const noexcept(
      SAFE_CTOR(zip_with_fn) and SAFE_MOVE(Rng2) and SAFE_MOVE(F)) {
      return meta::bind_back(zip_with_fn{}, std::forward<Rng2>(rng2), std::move(f));
    }
  };

  export inline constexpr zip_with_fn zip_with{};
}