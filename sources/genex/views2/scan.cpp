module;
#include <genex/macros.hpp>

export module genex.views2.scan;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename F, typename Init>
  using scan_acc_t = std::remove_cvref_t<std::invoke_result_t<F&, Init, iter_reference_t<I>>>;

  template <typename I, typename S, typename F, typename Init>
  concept scannable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::invocable<F&, Init, iter_reference_t<I>> and
    std::movable<scan_acc_t<I, F, Init>> and
    std::constructible_from<scan_acc_t<I, F, Init>, Init> and
    std::invocable<F&, scan_acc_t<I, F, Init>&, iter_reference_t<I>>;

  template <typename Rng, typename F, typename Init>
  concept scannable_range =
    input_range<Rng> and
    scannable_iters<iterator_t<Rng>, sentinel_t<Rng>, F, Init>;

  template <typename I, typename S, typename F>
  concept summable_iters =
    scannable_iters<I, S, F, iter_value_t<I>> and
    std::constructible_from<iter_value_t<I>, iter_reference_t<I>>;

  template <typename Rng, typename F>
  concept summable_range =
    input_range<Rng> and
    summable_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}

namespace genex::views::detail::impl {
  struct scan_sentinel {};

  template <typename I, typename S, typename F, typename Acc>
    requires std::input_iterator<I> and std::sentinel_for<S, I>
  struct scan_iterator {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS meta::box<F> op;
    std::optional<Acc> acc;

    using value_type = Acc;
    using reference_type = const Acc&;
    using reference = reference_type;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;
    GENEX_ITER_OPS(scan_iterator);

    GENEX_INLINE constexpr scan_iterator() = default;

    GENEX_INLINE constexpr scan_iterator(I first, S last, F op, std::optional<Acc> seed) :
      it(std::move(first)), st(std::move(last)), op(std::move(op)), acc(std::move(seed)) {
      if (this->it == this->st) { this->acc.reset(); }
      else if (this->acc.has_value()) { this->acc = meta::invoke(*this->op, *this->acc, *this->it); }
      else { this->acc.emplace(*this->it); }
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_NEXT {
      ++self.it;
      if (self.it == self.st) { self.acc.reset(); }
      else { self.acc = meta::invoke(*self.op, *self.acc, *self.it); }
      return self;
    }

    template <typename Self>
    GENEX_VIEW_CUSTOM_PREV = delete;

    template <typename Self>
    GENEX_VIEW_CUSTOM_DEREF {
      return *self.acc;
    }

    GENEX_VIEW_ITER_EQ(scan_iterator, scan_iterator) {
      return self.it == that.it;
    }

    GENEX_VIEW_ITER_EQ(scan_iterator, scan_sentinel) {
      GENEX_IGNORE(that);
      return self.it == self.st;
    }
  };

  template <typename I, typename S, typename F, typename Acc>
    requires std::input_iterator<I> and std::sentinel_for<S, I>
  struct scan_view {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS F op;
    std::optional<Acc> seed;

    GENEX_INLINE constexpr scan_view(I first, S last, F op, std::optional<Acc> seed) :
      it(std::move(first)), st(std::move(last)), op(std::move(op)), seed(std::move(seed)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return scan_iterator<I, S, F, Acc>(self.it, self.st, self.op, self.seed);
    }

    template <typename Self>
    GENEX_ITER_END {
      GENEX_IGNORE(self);
      return scan_sentinel();
    }

    template <typename Self>
    GENEX_ITER_SIZE {
      return iterators::distance(self.it, self.st);
    }
  };
}

namespace genex::views {
  struct scan_fn {
    template <typename I, typename S, typename Init, typename F = std::plus<>>
      requires detail::concepts::scannable_iters<I, S, F, Init>
    GENEX_INLINE constexpr auto operator()(I first, S last, Init init, F op = {}) const noexcept(
      SAFE_CTOR(
        detail::impl::scan_view<I, S, F, detail::concepts::scan_acc_t<I, F, Init>>, I, S, F,
        std::optional<detail::concepts::scan_acc_t<I, F, Init>>) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Init) and SAFE_MOVE(F)) {
      using acc_t = detail::concepts::scan_acc_t<I, F, Init>;
      return detail::impl::scan_view<I, S, F, acc_t>(
        std::move(first), std::move(last), std::move(op), std::optional<acc_t>(std::move(init)));
    }

    template <typename Rng, typename Init, typename F = std::plus<>>
      requires detail::concepts::scannable_range<Rng, F, Init>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, Init init, F op = {}) const noexcept(
      SAFE_CTOR(
        detail::impl::scan_view<
        iterator_t<Rng>, sentinel_t<Rng>, F, detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>>,
        iterator_t<Rng>, sentinel_t<Rng>, F,
        std::optional<detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>>) and
      SAFE_MOVE(Init) and SAFE_MOVE(F)) {
      using acc_t = detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>;
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::scan_view<iterator_t<Rng>, sentinel_t<Rng>, F, acc_t>(
        std::move(first), std::move(last), std::move(op), std::optional<acc_t>(std::move(init)));
    }

    template <typename Init, typename F = std::plus<>>
      requires (not range<Init>)
    GENEX_INLINE constexpr auto operator()(Init init, F op = {}) const noexcept(
      SAFE_CTOR(scan_fn) and SAFE_MOVE(Init) and SAFE_MOVE(F)) {
      return meta::bind_back(scan_fn{}, std::move(init), std::move(op));
    }
  };

  struct partial_sum_fn {
    template <typename I, typename S, typename F = std::plus<>>
      requires detail::concepts::summable_iters<I, S, F>
    GENEX_INLINE constexpr auto operator()(I first, S last, F op = {}) const noexcept(
      SAFE_CTOR(
        detail::impl::scan_view<I, S, F, iter_value_t<I>>, I, S, F, std::optional<iter_value_t<I>>) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(F)) {
      return detail::impl::scan_view<I, S, F, iter_value_t<I>>(
        std::move(first), std::move(last), std::move(op), std::nullopt);
    }

    template <typename Rng, typename F = std::plus<>>
      requires detail::concepts::summable_range<Rng, F>
    GENEX_INLINE constexpr auto operator()(Rng &&rng, F op = {}) const noexcept(
      SAFE_CTOR(
        detail::impl::scan_view<iterator_t<Rng>, sentinel_t<Rng>, F, range_value_t<Rng>>,
        iterator_t<Rng>, sentinel_t<Rng>, F, std::optional<range_value_t<Rng>>) and
      SAFE_MOVE(F)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::scan_view<iterator_t<Rng>, sentinel_t<Rng>, F, range_value_t<Rng>>(
        std::move(first), std::move(last), std::move(op), std::nullopt);
    }

    template <typename F = std::plus<>>
      requires (not range<F>)
    GENEX_INLINE constexpr auto operator()(F op = {}) const noexcept(
      SAFE_CTOR(partial_sum_fn) and SAFE_MOVE(F)) {
      return meta::bind_back(partial_sum_fn{}, std::move(op));
    }
  };

  export inline constexpr scan_fn scan{};
  export inline constexpr partial_sum_fn partial_sum{};
}
