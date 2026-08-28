module;
#include <genex/macros.hpp>

export module genex.views2.generate;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename F>
  concept generatable_fn =
    std::invocable<F&> and
    not std::is_void_v<std::invoke_result_t<F&>> and
    std::movable<std::remove_cvref_t<std::invoke_result_t<F&>>>;
}

namespace genex::views::detail::impl {
  struct generate_sentinel {};

  template <typename F>
    requires concepts::generatable_fn<F>
  struct generate_iterator {
    GENEX_NO_UNIQUE_ADDRESS meta::box<F> f;
    std::ptrdiff_t count = 0;
    std::optional<std::remove_cvref_t<std::invoke_result_t<F&>>> cur;

    using value_type = std::remove_cvref_t<std::invoke_result_t<F&>>;
    using reference_type = const value_type&;
    using reference = reference_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS_MINIMAL(generate_iterator);

    GENEX_INLINE constexpr generate_iterator() = default;

    GENEX_INLINE constexpr generate_iterator(F f, const std::ptrdiff_t count) :
      f(std::move(f)), count(count) {
      if (count != 0) { cur.emplace(meta::invoke(*this->f)); }
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      if (self.count > 0) { --self.count; }
      if (self.count != 0) { self.cur.emplace(meta::invoke(*self.f)); }
      else { self.cur.reset(); }
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return *self.cur;
    }

    GENEX_VIEW_ITER_EQ(generate_iterator, generate_iterator) {
      return self.count == that.count;
    }

    GENEX_VIEW_ITER_EQ(generate_iterator, generate_sentinel) {
      GENEX_IGNORE(that);
      return self.count == 0;
    }
  };

  template <typename F>
    requires concepts::generatable_fn<F>
  struct generate_view {
    GENEX_NO_UNIQUE_ADDRESS F f;
    std::ptrdiff_t count;

    GENEX_INLINE constexpr generate_view(F f, const std::ptrdiff_t count) :
      f(std::move(f)), count(count) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return generate_iterator<F>(self.f, self.count);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return generate_sentinel();
    }
  };
}

namespace genex::views {
  struct generate_fn {
    template <typename F>
      requires detail::concepts::generatable_fn<F>
    GENEX_INLINE constexpr auto operator()(F f) const noexcept(
      SAFE_CTOR(detail::impl::generate_view<F>, F, std::ptrdiff_t) and SAFE_MOVE(F)) {
      return detail::impl::generate_view<F>(std::move(f), std::ptrdiff_t{-1});
    }
  };

  struct generate_n_fn {
    template <typename F, typename Int>
      requires detail::concepts::generatable_fn<F> and std::integral<Int>
    GENEX_INLINE constexpr auto operator()(F f, const Int count) const noexcept(
      SAFE_CTOR(detail::impl::generate_view<F>, F, std::ptrdiff_t) and SAFE_MOVE(F)) {
      GENEX_ASSERT(std::out_of_range, count >= 0);
      return detail::impl::generate_view<F>(std::move(f), static_cast<std::ptrdiff_t>(count));
    }
  };

  export inline constexpr generate_fn generate{};
  export inline constexpr generate_n_fn generate_n{};
}