module;
#include <genex/detail/macros.hpp>

export module genex.views.scan;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename F, typename Init>
  using scan_acc_t = std::remove_cvref_t<
    std::invoke_result_t<F&, Init, iter_reference_t<I>>>;

  template <typename I, typename S, typename F, typename Init>
  concept scannable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    std::invocable<F&, Init, iter_reference_t<I>> and
    std::movable<scan_acc_t<I, F, Init>> and
    std::constructible_from<scan_acc_t<I, F, Init>, Init> and
    std::invocable<F&, scan_acc_t<I, F, Init>&, iter_reference_t<I>> and
    std::move_constructible<F>;

  template <typename Rng, typename F, typename Init>
  concept scannable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    scannable_iters<iterator_t<Rng>, sentinel_t<Rng>, F, Init>;

  template <typename I, typename S, typename F>
  concept summable_iters =
    scannable_iters<I, S, F, iter_value_t<I>> and
    std::constructible_from<iter_value_t<I>, iter_reference_t<I>>;

  template <typename Rng, typename F>
  concept summable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    summable_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}

namespace genex::views::detail::impl {
  struct scan_sentinel {};

  template <typename I, typename S, typename F, typename Acc>
  requires std::input_iterator<I> and std::sentinel_for<S, I>
  struct scan_iterator :
    genex::detail::iterator_ops<scan_iterator<I, S, F, Acc>> {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS meta::box<F> op;
    std::optional<Acc> acc;

    using value_type = Acc;
    using reference = const Acc&;
    using pointer = void;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr scan_iterator() = default;

    GENEX_INLINE constexpr scan_iterator(
      I first, S last, F op, std::optional<Acc> seed) :
      it(std::move(first)), st(std::move(last)),
      op(std::move(op)), acc(std::move(seed)) {
      if (this->it == this->st) { this->acc.reset(); }
      else if (this->acc.has_value()) {
        this->acc = meta::invoke(*this->op, *this->acc, *this->it);
      }
      else { this->acc.emplace(*this->it); }
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++it;
      if (it == st) { acc.reset(); }
      else { acc = meta::invoke(*op, *acc, *it); }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *acc;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const scan_iterator &self, const scan_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const scan_iterator &self, scan_sentinel) -> bool {
      return self.it == self.st;
    }
  };

  template <typename I, typename S, typename F, typename Acc>
  requires std::input_iterator<I> and std::sentinel_for<S, I>
  struct scan_view : view_base {
    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS F op;
    std::optional<Acc> seed;

    GENEX_INLINE constexpr scan_view(
      I first, S last, F op, std::optional<Acc> seed) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(F) and
      SAFE_MOVE(std::optional<Acc>)) :
      it(std::move(first)), st(std::move(last)),
      op(std::move(op)), seed(std::move(seed)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return scan_iterator<I, S, F, Acc>(it, st, op, seed);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return scan_sentinel();
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const
      requires requires { iterators::distance(it, st); } {
      return iterators::distance(it, st);
    }
  };
}

namespace genex::views {
  struct scan_fn : pipe_closure {
    template <typename I, typename S, typename Init, typename F = std::plus<>>
    requires detail::concepts::scannable_iters<I, S, F, Init>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, Init init, F op = {}) const noexcept(
      SAFE_CTOR(
        detail::impl::scan_view<I, S, F, detail::concepts::scan_acc_t<I, F, Init>>, I, S, F,
        std::optional<detail::concepts::scan_acc_t<I, F, Init>>) and
      SAFE_CTOR(detail::concepts::scan_acc_t<I, F, Init>, Init) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(Init) and SAFE_MOVE(F)) {
      using acc_t = detail::concepts::scan_acc_t<I, F, Init>;
      return detail::impl::scan_view<I, S, F, acc_t>(
        std::move(first), std::move(last), std::move(op),
        std::optional<acc_t>(std::move(init)));
    }

    template <typename Rng, typename Init, typename F = std::plus<>>
    requires detail::concepts::scannable_range<Rng, F, Init>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Init init, F op = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(
        detail::impl::scan_view<
        iterator_t<Rng>, sentinel_t<Rng>, F, detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>>,
        iterator_t<Rng>, sentinel_t<Rng>, F,
        std::optional<detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>>) and
      SAFE_CTOR(detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>, Init) and
      SAFE_MOVE(Init) and SAFE_MOVE(F)) {
      using acc_t = detail::concepts::scan_acc_t<iterator_t<Rng>, F, Init>;
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::scan_view<
        iterator_t<Rng>, sentinel_t<Rng>, F, acc_t>(
        std::move(first), std::move(last), std::move(op),
        std::optional<acc_t>(std::move(init)));
    }

    template <typename Init, typename F = std::plus<>>
    requires closure_value<Init>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Init &&init, F op = {}) const noexcept(
      SAFE_CTOR(scan_fn) and SAFE_CTOR(std::decay_t<Init>, Init) and
      SAFE_MOVE(F)) {
      return meta::bind_back(
        scan_fn{}, std::forward<Init>(init), std::move(op));
    }
  };

  struct partial_sum_fn : pipe_closure {
    template <typename I, typename S, typename F = std::plus<>>
    requires detail::concepts::summable_iters<I, S, F>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      I first, S last, F op = {}) const noexcept(
      SAFE_CTOR(detail::impl::scan_view<I, S, F, iter_value_t<I>>, I, S, F, std::optional<iter_value_t<I>>) and
      SAFE_MOVE(I) and SAFE_MOVE(S) and SAFE_MOVE(F)) {
      return detail::impl::scan_view<I, S, F, iter_value_t<I>>(
        std::move(first), std::move(last), std::move(op), std::nullopt);
    }

    template <typename Rng, typename F = std::plus<>>
    requires detail::concepts::summable_range<Rng, F>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Rng &&rng, F op = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_CTOR(
        detail::impl::scan_view<iterator_t<Rng>, sentinel_t<Rng>, F, range_value_t<Rng>>,
        iterator_t<Rng>, sentinel_t<Rng>, F, std::optional<range_value_t<Rng>>) and
      SAFE_MOVE(F)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::scan_view<
        iterator_t<Rng>, sentinel_t<Rng>, F, range_value_t<Rng>>(
        std::move(first), std::move(last), std::move(op), std::nullopt);
    }

    template <typename F>
    requires closure_callable<F>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      F op) const noexcept(
      SAFE_CTOR(partial_sum_fn) and SAFE_MOVE(F)) {
      return meta::bind_back(partial_sum_fn{}, std::move(op));
    }
  };

  export inline constexpr scan_fn scan{};
  export inline constexpr partial_sum_fn partial_sum{};
}
