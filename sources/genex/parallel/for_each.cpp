module;
#include <genex/macros.hpp>

export module genex.parallel.for_each;
export import genex.pipe;
export import genex.parallel.params;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;

namespace genex::parallel::detail::concepts {
  template <typename I, typename S, typename F>
  concept par_for_eachable_iters =
    std::random_access_iterator<I> and
    std::sized_sentinel_for<S, I> and
    std::copy_constructible<F> and
    std::invocable<F&, iter_reference_t<I>>;

  template <typename Rng, typename F>
  concept par_for_eachable_range =
    random_access_range<Rng> and
    sized_range<Rng> and
    par_for_eachable_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}

namespace genex::parallel::detail::impl {
  template <typename I, typename F>
  GENEX_INLINE auto for_each_chunk(I first, const I last, F &f) -> void {
    for (; first != last; ++first) { meta::invoke(f, *first); }
  }

  template <typename I, typename S, typename F>
    requires concepts::par_for_eachable_iters<I, S, F>
  auto do_for_each(I first, S last, F f, const params p) -> void {
    using diff_t = iter_difference_t<I>;

    const auto total = last - first;
    if (total <= 0) { return; }

    const auto workers = static_cast<diff_t>(parallel::detail::worker_count(static_cast<std::size_t>(total), p));
    if (workers <= 1) { return for_each_chunk(first, first + total, f); }

    // Spread the remainder over the leading chunks so no two differ in size by more than one, matching how
    // `views::divide` cuts a range. lo(workers) == total, so the chunks exactly tile the input.
    const auto base = total / workers;
    const auto rem = total % workers;
    const auto lo = [&first, base, rem](const diff_t i) -> I {
      return first + i * base + (i < rem ? i : rem);
    };

    // One slot per chunk. Each worker writes only its own element, so the concurrent writes do not race, and the
    // slots keep the exceptions ordered by chunk rather than by whichever thread happened to fail first.
    auto errors = std::vector<std::exception_ptr>(static_cast<std::size_t>(workers));
    auto threads = std::vector<std::jthread>();
    threads.reserve(static_cast<std::size_t>(workers) - 1);

    // Chunk 0 stays on the calling thread, so only `workers - 1` threads are spawned.
    auto spawned = diff_t{1};
    try {
      for (; spawned < workers; ++spawned) {
        threads.emplace_back(
          [f, b = lo(spawned), e = lo(spawned + 1),
            slot = &errors[static_cast<std::size_t>(spawned)]]() mutable -> void {
            try { for_each_chunk(b, e, f); }
            catch (...) { *slot = std::current_exception(); }
          });
      }
    }
    catch (...) {
      // The system refused a thread. Swallow it: the chunks that never got one are picked up by the loop below, on
      // this thread, so the call still completes rather than dropping part of the range.
    }

    for (auto i = diff_t{0}; i < workers; ++i) {
      if (i != 0 and i < spawned) { continue; }
      try { for_each_chunk(lo(i), lo(i + 1), f); }
      catch (...) { errors[static_cast<std::size_t>(i)] = std::current_exception(); }
    }

    // Join everything before touching the errors: an exception must not escape while workers are still running.
    threads.clear();
    for (auto &e : errors) {
      if (e != nullptr) { std::rethrow_exception(e); }
    }
  }
}

namespace genex::parallel {
  struct for_each_fn {
    template <typename I, typename S, typename F>
      requires detail::concepts::par_for_eachable_iters<I, S, F>
    GENEX_INLINE auto operator()(I first, S last, F f, const params p = {}) const -> void {
      detail::impl::do_for_each(std::move(first), std::move(last), std::move(f), p);
    }

    template <typename Rng, typename F>
      requires detail::concepts::par_for_eachable_range<Rng, F>
    GENEX_INLINE auto operator()(Rng &&rng, F f, const params p = {}) const -> void {
      auto [first, last] = iterators::iter_pair(rng);
      detail::impl::do_for_each(std::move(first), std::move(last), std::move(f), p);
    }

    template <typename F>
      requires (not range<F>)
    GENEX_INLINE constexpr auto operator()(F f, const params p = {}) const noexcept(
      SAFE_CTOR(for_each_fn) and SAFE_MOVE(F)) {
      return meta::bind_back(for_each_fn{}, std::move(f), p);
    }
  };

  export inline constexpr for_each_fn for_each{};
}
