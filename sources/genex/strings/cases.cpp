module;
#include <genex/detail/macros.hpp>

export module genex.strings.cases;
export import genex.pipe;
import genex.concepts;
import genex.views.transform;
import std;

namespace genex::strings::detail::concepts {
  template <typename I, typename S>
  concept casable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    char_like<iter_value_t<I>>;

  template <typename Rng>
  concept casable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    casable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::strings::detail::impl {
  struct to_upper_fn {
    // No locale-independent case mapping for UTF code units.
    template <typename C>
    requires char_like<C>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(C c) const -> C {
      return c;
    }

    template <typename C>
    requires wide_char_like<C>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(C c) const -> C {
      return static_cast<C>(std::towupper(static_cast<std::wint_t>(c)));
    }

    template <typename C>
    requires strict_char_like<C>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(C c) const -> C {
      return static_cast<C>(std::toupper(static_cast<unsigned char>(c)));
    }
  };

  struct to_lower_fn {
    // No locale-independent case mapping for UTF code units.
    template <typename C>
    requires char_like<C>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(C c) const -> C {
      return c;
    }

    template <typename C>
    requires wide_char_like<C>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(C c) const -> C {
      return static_cast<C>(std::towlower(static_cast<std::wint_t>(c)));
    }

    template <typename C>
    requires strict_char_like<C>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(C c) const -> C {
      return static_cast<C>(std::tolower(static_cast<unsigned char>(c)));
    }
  };
}

namespace genex::strings {
  struct upper_case_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::casable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const {
      return views::transform(
        std::move(first), std::move(last), detail::impl::to_upper_fn{});
    }

    template <typename Rng>
    requires detail::concepts::casable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const {
      return views::transform(
        std::forward<Rng>(rng), detail::impl::to_upper_fn{});
    }
  };

  struct lower_case_fn : pipe_closure {
    template <typename I, typename S>
    requires detail::concepts::casable_iters<I, S>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last) const {
      return views::transform(
        std::move(first), std::move(last), detail::impl::to_lower_fn{});
    }

    template <typename Rng>
    requires detail::concepts::casable_range<Rng>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng) const {
      return views::transform(
        std::forward<Rng>(rng), detail::impl::to_lower_fn{});
    }
  };

  export inline constexpr upper_case_fn upper_case{};
  export inline constexpr lower_case_fn lower_case{};
}
