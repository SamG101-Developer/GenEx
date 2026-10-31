module;
#include <genex/detail/macros.hpp>

export module genex.views.interleave;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.detail.range_closure;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename T1, typename T2>
  using interleave_common_t = std::common_type_t<
    std::remove_cvref_t<iter_value_t<T1>>,
    std::remove_cvref_t<iter_value_t<T2>>>;

  template <typename I1, typename S1, typename I2, typename S2>
  concept interleavable_iters =
    std::input_iterator<I1> and
    std::input_iterator<I2> and
    std::sentinel_for<S1, I1> and
    std::sentinel_for<S2, I2> and
    std::convertible_to<iter_value_t<I1>, interleave_common_t<I1, I2>> and
    std::convertible_to<iter_value_t<I2>, interleave_common_t<I1, I2>>;

  template <typename Rng1, typename Rng2>
  concept interleavable_range =
    non_dangling_range<Rng1> and
    non_dangling_range<Rng2> and
    input_range<Rng1> and
    input_range<Rng2> and
    interleavable_iters<
      iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>>;
}

namespace genex::views::detail::impl {
  template <typename I1, typename S1, typename I2, typename S2>
  requires concepts::interleavable_iters<I1, S1, I2, S2>
  struct interleave_iterator :
    genex::detail::iterator_ops<interleave_iterator<I1, S1, I2, S2>> {
    I1 it1;
    S1 st1;
    I2 it2;
    S2 st2;
    bool use_first = true;
    bool extend = false;

    using value_type = concepts::interleave_common_t<I1, I2>;
    using reference = value_type;
    using difference_type = std::common_type_t<
      iter_difference_t<I1>, iter_difference_t<I2>>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr interleave_iterator() = default;

    GENEX_INLINE constexpr interleave_iterator(
      I1 first1, S1 last1, I2 first2, S2 last2, const bool extend) :
      it1(std::move(first1)), st1(std::move(last1)),
      it2(std::move(first2)), st2(std::move(last2)),
      extend(extend) {
    }

    GENEX_INLINE constexpr auto next() -> void {
      if (done()) { return; }
      if (use_first) {
        if (it1 != st1) { ++it1; }
        else if (extend and it2 != st2) { ++it2; }
      }
      else {
        if (it2 != st2) { ++it2; }
        else if (extend and it1 != st1) { ++it1; }
      }
      use_first = not use_first;
      if (it1 == st1 and it2 != st2) { use_first = false; }
      if (it2 == st2 and it1 != st1) { use_first = true; }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      if (use_first) {
        if (it1 != st1) { return *it1; }
        return *it2;
      }
      else {
        if (it2 != st2) { return *it2; }
        return *it1;
      }
    }

    GENEX_INLINE friend constexpr auto operator==(
      const interleave_iterator &self, const interleave_iterator &) -> bool {
      return self.done();
    }

  private:
    GENEX_INLINE constexpr auto done() const -> bool {
      const auto end1 = it1 == st1;
      const auto end2 = it2 == st2;
      return extend ? (end1 and end2) : (end1 or end2);
    }
  };

  template <typename I1, typename S1, typename I2, typename S2>
  requires concepts::interleavable_iters<I1, S1, I2, S2>
  struct interleave_view : view_base {
    I1 it1;
    S1 st1;
    I2 it2;
    S2 st2;
    bool extend;

    GENEX_INLINE constexpr interleave_view(
      I1 first1, S1 last1, I2 first2, S2 last2, const bool extend) noexcept(
      SAFE_MOVE(I1) and SAFE_MOVE(S1) and SAFE_MOVE(I2) and SAFE_MOVE(S2)) :
      it1(std::move(first1)), st1(std::move(last1)),
      it2(std::move(first2)), st2(std::move(last2)),
      extend(extend) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return interleave_iterator(it1, st1, it2, st2, extend);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return interleave_iterator(st1, st1, st2, st2, extend);
    }

    template <typename Self>
    requires std::sized_sentinel_for<S1, I1> and std::sized_sentinel_for<S2, I2>
    [[nodiscard]] GENEX_INLINE constexpr auto size(this Self &&self) -> std::size_t {
      const auto n1 = iterators::distance(self.it1, self.st1);
      const auto n2 = iterators::distance(self.it2, self.st2);
      return static_cast<std::size_t>(self.extend ? n1 + n2 : 2 * std::min(n1, n2));
    }
  };
}

namespace genex::views {
  struct interleave_fn : pipe_closure {
    template <typename I1, typename S1, typename I2, typename S2>
    requires detail::concepts::interleavable_iters<I1, S1, I2, S2>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I1 first1, S1 last1, I2 first2, S2 last2, bool extend = true) const noexcept(
      SAFE_CTOR(detail::impl::interleave_view<I1, S1, I2, S2>, I1, S1, I2, S2, bool) and
      SAFE_MOVE(I1) and SAFE_MOVE(S1) and SAFE_MOVE(I2) and SAFE_MOVE(S2)) {
      return detail::impl::interleave_view(
        std::move(first1), std::move(last1), std::move(first2), std::move(last2),
        extend);
    }

    template <typename Rng1, typename Rng2>
    requires detail::concepts::interleavable_range<Rng1, Rng2>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng1 &&rng1, Rng2 &&rng2, bool extend = true) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng1&) and
      SAFE_CALL(decltype(iterators::iter_pair), Rng2&) and
      SAFE_CTOR(
        detail::impl::interleave_view<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>>,
        iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, bool)) {
      auto [first1, last1] = iterators::iter_pair(rng1);
      auto [first2, last2] = iterators::iter_pair(rng2);
      return detail::impl::interleave_view(
        std::move(first1), std::move(last1), std::move(first2), std::move(last2),
        extend);
    }

    template <typename Rng2>
    requires range<Rng2>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng2 &&rng2, bool extend = true) const {
      return genex::detail::bind_range_back(
        interleave_fn{}, std::forward<Rng2>(rng2), extend);
    }
  };

  export inline constexpr interleave_fn interleave{};
}
