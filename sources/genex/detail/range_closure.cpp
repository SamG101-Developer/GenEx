module;
#include <genex/detail/macros.hpp>

export module genex.detail.range_closure;
import genex.concepts;
import genex.iterators.access;
import genex.meta;
import genex.subrange;
import std;

export namespace genex::detail {
  /// A closure holding a range by value. Any view it creates points into that
  /// range, so it must outlive the view.
  template <typename F>
  struct retained_closure : F {
  };

  template <typename F>
  inline constexpr auto is_retained_closure_v = false;

  template <typename F>
  inline constexpr auto is_retained_closure_v<retained_closure<F>> = true;

  template <typename Rng>
  concept referable_range =
    non_dangling_range<Rng> and
    std::is_lvalue_reference_v<Rng>;

  /// Binds a second range to a closure without a dangling iterator. An lvalue
  /// is referred to rather than copied, a view or a borrowed range is safe to
  /// hold as is, and any other rvalue is moved into a closure the pipe will
  /// keep alive.
  template <typename Fn, typename Rng, typename... Args>
  GENEX_INLINE constexpr auto bind_range_back(
    Fn fn, Rng &&rng, Args &&... args) {
    auto closure = meta::bind_back(
      std::move(fn), std::forward<Rng>(rng), std::forward<Args>(args)...);
    return retained_closure<decltype(closure)>{std::move(closure)};
  }

  template <typename Fn, typename Rng, typename... Args>
  requires non_dangling_range<Rng>
  GENEX_INLINE constexpr auto bind_range_back(
    Fn fn, Rng &&rng, Args &&... args) {
    return meta::bind_back(
      std::move(fn), std::forward<Rng>(rng), std::forward<Args>(args)...);
  }

  template <typename Fn, typename Rng, typename... Args>
  requires referable_range<Rng>
  GENEX_INLINE constexpr auto bind_range_back(
    Fn fn, Rng &&rng, Args &&... args) {
    return meta::bind_back(
      std::move(fn),
      genex::subrange(
        iterators::begin(rng),
        iterators::end(rng)),
        std::forward<Args>(args)...);
  }
}
