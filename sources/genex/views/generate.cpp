module;
#include <genex/detail/macros.hpp>

export module genex.views.generate;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
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

  template <typename F, bool Counted>
  requires concepts::generatable_fn<F>
  struct generate_iterator : genex::detail::iterator_ops<generate_iterator<F, Counted>> {
    GENEX_NO_UNIQUE_ADDRESS meta::box<F> f;
    std::ptrdiff_t count = 0;
    std::optional<std::remove_cvref_t<std::invoke_result_t<F&>>> cur;

    using value_type = std::remove_cvref_t<std::invoke_result_t<F&>>;
    using reference = const value_type&;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr generate_iterator() = default;

    GENEX_INLINE constexpr generate_iterator(F f, const std::ptrdiff_t count) :
      f(std::move(f)), count(count) {
      if (not Counted or count != 0) { cur.emplace(meta::invoke(*this->f)); }
    }

    GENEX_INLINE constexpr auto next() -> void {
      if constexpr (Counted) {
        --count;
        if (count == 0) {
          cur.reset();
          return;
        }
      }
      cur.emplace(meta::invoke(*f));
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *cur;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const generate_iterator &self, const generate_iterator &that) -> bool {
      return self.count == that.count;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const generate_iterator &self, generate_sentinel) -> bool {
      return self.count == 0;
    }
  };

  template <typename F, bool Counted>
  requires concepts::generatable_fn<F>
  struct generate_view : view_base {
    GENEX_NO_UNIQUE_ADDRESS F f;
    std::ptrdiff_t count;

    GENEX_INLINE constexpr generate_view(
      F f, const std::ptrdiff_t count) noexcept(
      SAFE_MOVE(F)) :
      f(std::move(f)), count(count) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return generate_iterator<F, Counted>(f, count);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      if constexpr (Counted) { return generate_sentinel(); }
      else { return unreachable_sentinel(); }
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const -> std::size_t requires Counted {
      return static_cast<std::size_t>(count);
    }
  };
}

namespace genex::views {
  struct generate_fn : pipe_closure {
    template <typename F>
    requires detail::concepts::generatable_fn<F>
    GENEX_INLINE constexpr auto operator()(F f) const noexcept(
      SAFE_CTOR(detail::impl::generate_view<F, false>, F, std::ptrdiff_t) and
      SAFE_MOVE(F)) {
      return detail::impl::generate_view<F, false>(
        std::move(f), std::ptrdiff_t{0});
    }
  };

  struct generate_n_fn : pipe_closure {
    template <typename F, typename Int>
    requires detail::concepts::generatable_fn<F> and std::integral<Int>
    GENEX_INLINE constexpr auto operator()(F f, const Int count) const noexcept(
      SAFE_CTOR(detail::impl::generate_view<F, true>, F, std::ptrdiff_t) and
      SAFE_MOVE(F)) {
      GENEX_ASSERT(count >= 0);
      return detail::impl::generate_view<F, true>(
        std::move(f), static_cast<std::ptrdiff_t>(count));
    }
  };

  export inline constexpr generate_fn generate{};
  export inline constexpr generate_n_fn generate_n{};
}
