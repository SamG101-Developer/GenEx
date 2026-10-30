module;
#include <genex/detail/macros.hpp>

export module genex.views.join;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.access;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.meta;
import std;

namespace genex::views::detail::concepts {
  template <typename I, typename S>
  concept joinable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    input_range<iter_value_t<I>>;

  template <typename Rng>
  concept joinable_range =
    non_dangling_range<Rng> and
    input_range<Rng> and
    joinable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}

namespace genex::views::detail::impl {
  struct join_sentinel {};

  template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
  struct join_iterator : genex::detail::iterator_ops<join_iterator<I, S>> {
    // When the outer iterator dereferences to a prvalue range (e.g. transform/intersperse
    // that yield elements by value), the inner range is a temporary and its iterators would
    // dangle. In that case we cache the current inner range inside the iterator itself. The
    // cache is boxed on the heap because `inner_it`/`inner_st` point into it: copying or
    // moving the iterator must not change where the cached range lives.
    static constexpr bool ref_is_glvalue = std::is_reference_v<iter_reference_t<I>>;
    using inner_cache_t = std::conditional_t<
      ref_is_glvalue, std::monostate, std::shared_ptr<iter_value_t<I>>>;

    I it;
    S st;
    GENEX_NO_UNIQUE_ADDRESS inner_cache_t inner_cache;
    iterator_t<iter_value_t<I>> inner_it;
    sentinel_t<iter_value_t<I>> inner_st;

    using value_type = range_value_t<iter_value_t<I>>;
    using reference = range_reference_t<iter_value_t<I>>;
    using difference_type = iter_difference_t<I>;
    using iterator_category = std::conditional_t<
      ref_is_glvalue and std::forward_iterator<I>,
      std::forward_iterator_tag,
      std::input_iterator_tag>;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr join_iterator() = default;

    GENEX_INLINE constexpr join_iterator(I first, S last) :
      it(std::move(first)), st(std::move(last)) {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto next() -> void {
      ++inner_it;
      if (inner_it == inner_st) {
        ++it;
        fwd_to_valid();
      }
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      GENEX_ASSERT(it != st);
      GENEX_ASSERT(inner_it != inner_st);
      return *inner_it;
    }

    GENEX_INLINE friend constexpr auto operator==(const join_iterator &self, const join_iterator &that) -> bool {
      return self.it == that.it;
    }

    GENEX_INLINE friend constexpr auto operator==(const join_iterator &self, join_sentinel) -> bool {
      return self.it == self.st;
    }

  private:
    GENEX_INLINE constexpr auto satisfy_inner() -> decltype(auto) {
      if constexpr (ref_is_glvalue) {
        return (*it);
      }
      else {
        inner_cache = std::make_shared<iter_value_t<I>>(*it);
        return (*inner_cache);
      }
    }

    constexpr auto fwd_to_valid() -> void {
      while (it != st) {
        auto &&inner = satisfy_inner();
        inner_it = iterators::begin(inner);
        inner_st = iterators::end(inner);
        if (inner_it != inner_st) { return; }
        ++it;
      }
      inner_it = iterator_t<iter_value_t<I>>{};
      inner_st = sentinel_t<iter_value_t<I>>{};
    }
  };

  template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
  struct join_view : view_base {
    I it;
    S st;

    GENEX_INLINE constexpr join_view(I first, S last) noexcept(
      SAFE_MOVE(I) and SAFE_MOVE(S)) :
      it(std::move(first)), st(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return join_iterator(it, st);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return join_sentinel();
    }

    [[nodiscard]] constexpr auto size() const
      requires std::forward_iterator<I> and requires { iterators::distance(it, st); } {
      auto total_size = static_cast<iter_difference_t<I>>(0);
      for (auto cur = it; cur != st; ++cur) {
        auto &&inner = *cur;
        total_size += iterators::distance(iterators::begin(inner), iterators::end(inner));
      }
      return total_size;
    }
  };
}

namespace genex::views {
  struct join_fn {
    template <typename I, typename S>
      requires detail::concepts::joinable_iters<I, S>
    GENEX_INLINE constexpr auto operator()(I first, S last) const noexcept(
      SAFE_IMPL_CTOR(join_view, I, S) and
      SAFE_MOVE(I) and SAFE_MOVE(S)) {
      return detail::impl::join_view(std::move(first), std::move(last));
    }

    template <typename Rng>
      requires detail::concepts::joinable_range<Rng>
    GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng&) and
      SAFE_IMPL_CTOR(join_view, iterator_t<Rng>, sentinel_t<Rng>)) {
      auto [first, last] = iterators::iter_pair(rng);
      return detail::impl::join_view(std::move(first), std::move(last));
    }

    GENEX_INLINE constexpr auto operator()() const noexcept(
      SAFE_CTOR(join_fn)) {
      return meta::bind_back(join_fn{});
    }
  };

  export inline constexpr join_fn join{};
}
