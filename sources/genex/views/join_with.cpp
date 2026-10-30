module;
#include <genex/detail/macros.hpp>

export module genex.views.join_with;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.access;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S, typename New>
  concept joinable_with_iters =
    std::forward_iterator<I> and
    std::sentinel_for<S, I> and
    input_range<iter_value_t<I>> and
    std::convertible_to<New, range_value_t<iter_value_t<I>>>;

  template <typename Rng, typename New>
  concept joinable_with_range =
    non_dangling_range<Rng> and
    forward_range<Rng> and
    joinable_with_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}

namespace genex::views::detail::impl {
  struct join_with_sentinel {};

  template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
  struct join_with_iterator : genex::detail::iterator_ops<join_with_iterator<I, S, New>> {
    I it;
    S st;
    New new_value;
    bool use_new = false;
    iterator_t<iter_value_t<I>> inner_it;
    sentinel_t<iter_value_t<I>> inner_st;

    using value_type = range_value_t<iter_value_t<I>>;
    using reference = std::common_reference_t<range_reference_t<iter_value_t<I>>, const New&>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::conditional_t<
      std::forward_iterator<I>,
      std::forward_iterator_tag,
      std::input_iterator_tag>;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr join_with_iterator() = default;

    GENEX_INLINE constexpr join_with_iterator(I first, S last, New new_val) :
      it(std::move(first)), st(std::move(last)), new_value(std::move(new_val)) {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto next() -> void {
      if (use_new) {
        use_new = false;
        ++it;
        fwd_to_valid();
        return;
      }
      ++inner_it;
      if (inner_it == inner_st) {
        use_new = true;
      }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      GENEX_ASSERT(it != st);
      GENEX_ASSERT(inner_it != inner_st or use_new);
      return use_new
        ? static_cast<reference>(new_value)
        : static_cast<reference>(*inner_it);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const join_with_iterator &self, const join_with_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(const join_with_iterator &self, join_with_sentinel) -> bool {
      if (self.it == self.st) { return true; }

      if (self.use_new) {
        auto next_it = self.it;
        ++next_it;
        return next_it == self.st;
      }

      return false;
    }

  private:
    constexpr auto fwd_to_valid() -> void {
      while (it != st) {
        inner_it = iterators::begin(*it);
        inner_st = iterators::end(*it);
        if (inner_it != inner_st) { return; }
        ++it;
      }
      inner_it = iterator_t<iter_value_t<I>>{};
      inner_st = sentinel_t<iter_value_t<I>>{};
    }
  };

  template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
  struct join_with_view : view_base {
    I it;
    S st;
    New new_value;

    GENEX_INLINE constexpr join_with_view(I first, S last, New new_val) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(New)) :
      it(std::move(first)), st(std::move(last)), new_value(std::move(new_val)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return join_with_iterator(it, st, new_value);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return join_with_sentinel();
    }

    [[nodiscard]] constexpr auto size() const
      requires std::forward_iterator<I> and requires { iterators::distance(it, st); } {
      // Empty ranges are skipped, along with the separator they'd have had, and the last range has none after it.
      auto total_size = 0z;
      auto last_is_empty = true;
      for (auto cur = it; cur != st; ++cur) {
        const auto inner_size = iterators::distance(iterators::begin(*cur), iterators::end(*cur));
        last_is_empty = inner_size == 0;
        if (not last_is_empty) { total_size += inner_size + 1z; }
      }
      return last_is_empty ? total_size : total_size - 1z;
    }
  };
}

namespace genex::views {
  struct join_with_fn {
    template <typename I, typename S, typename New>
      requires detail::concepts::joinable_with_iters<I, S, New>
    GENEX_INLINE constexpr auto operator()(I first, S last, New new_value) const noexcept(
      SAFE_IMPL_CTOR(join_with_view, I, S, New) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(New)) {
      return detail::impl::join_with_view(std::move(first), std::move(last), std::move(new_value));
    }

    template <typename Rng, typename New>
      requires detail::concepts::joinable_with_range<Rng, New>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, New new_value) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(join_with_view, iterator_t<Rng>, sentinel_t<Rng>, New) and
      SAFE_MOVE(New)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::join_with_view(std::move(first), std::move(last), std::move(new_value));
    }

    template <typename New>
    GENEX_INLINE constexpr auto operator()(New new_value) const noexcept(
      SAFE_CTOR(join_with_fn) and SAFE_MOVE(New)) {
      return meta::bind_back(join_with_fn{}, std::move(new_value));
    }
  };

  export inline constexpr join_with_fn join_with{};
}
