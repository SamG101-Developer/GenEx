module;
#include <genex/macros.hpp>

export module genex.pipe;
import genex.concepts;
import genex.iterators.access;
import genex.meta;
import genex.subrange;
import std;

namespace genex::detail {
  /**
   * A closure holding a range by value. Any view it creates points into that range, so it must outlive the view.
   */
  template <typename F>
  struct retained_closure : F {
  };

  template <typename F>
  inline constexpr bool is_retained_closure_v = false;

  template <typename F>
  inline constexpr bool is_retained_closure_v<retained_closure<F>> = true;

  /**
   * Applies @c f and then @c g, so that a pipeline over an owned range rebuilds its views once per @c begin or @c end,
   * rather than once per view per call.
   */
  template <typename F, typename G>
  struct composed_adaptor {
    GENEX_NO_UNIQUE_ADDRESS F f;
    GENEX_NO_UNIQUE_ADDRESS G g;

    template <typename Self, typename Rng>
    GENEX_INLINE constexpr auto operator()(this Self &&self, Rng &rng) -> decltype(auto) {
      return meta::invoke(self.g, meta::invoke(self.f, rng));
    }
  };

  /**
   * A view that keeps alive the storage another view points into. @c Base is the piped range: held by value when it is
   * an rvalue, and by reference otherwise. The inner view is rebuilt from @c Base
   * for each @c begin and @c end, because moving a container can invalidate its iterators (a small @c std::string, for
   * example). Nothing else about a view has to change to be owned, as genex iterators hold copies of what they use.
   *
   * It deliberately has no @c view_base: piping one treats it like an rvalue container, and it is move-only.
   */
  template <typename Base, typename Adaptor>
  class owning_view {
    GENEX_NO_UNIQUE_ADDRESS Base m_base;
    GENEX_NO_UNIQUE_ADDRESS Adaptor m_adaptor;

  public:
    using base_type = Base;
    using adaptor_type = Adaptor;

    GENEX_INLINE constexpr owning_view(Base base, Adaptor adaptor) :
      m_base(std::forward<Base>(base)), m_adaptor(std::move(adaptor)) {
    }

    owning_view(const owning_view &) = delete;
    owning_view(owning_view &&) = default;
    auto operator=(const owning_view &) -> owning_view& = delete;
    auto operator=(owning_view &&) -> owning_view& = default;
    ~owning_view() = default;

    template <typename Self>
    GENEX_ITER_BEGIN {
      return iterators::begin(meta::invoke(self.m_adaptor, self.m_base));
    }

    template <typename Self>
    GENEX_ITER_END {
      return iterators::end(meta::invoke(self.m_adaptor, self.m_base));
    }

    template <typename Self>
      requires requires(Self &s) { meta::invoke(s.m_adaptor, s.m_base).size(); }
    GENEX_NODISCARD GENEX_INLINE constexpr auto size(this Self &&self) {
      return meta::invoke(self.m_adaptor, self.m_base).size();
    }

    template <typename Self>
      requires requires(Self &s) { meta::invoke(s.m_adaptor, s.m_base).reserve_hint(); }
    GENEX_NODISCARD GENEX_INLINE constexpr auto reserve_hint(this Self &&self) {
      return meta::invoke(self.m_adaptor, self.m_base).reserve_hint();
    }

    GENEX_INLINE constexpr auto base() && -> Base {
      return std::forward<Base>(m_base);
    }

    GENEX_INLINE constexpr auto adaptor() && -> Adaptor {
      return std::move(m_adaptor);
    }
  };

  template <typename T>
  inline constexpr bool is_owning_view_v = false;

  template <typename Base, typename Adaptor>
  inline constexpr bool is_owning_view_v<owning_view<Base, Adaptor>> = true;

  template <typename T>
  concept rvalue_container =
    not non_dangling_range<T> and
    input_range<std::remove_reference_t<T>&>;

  /**
   * Whether piping @c T into @c F returns a view that needs storage kept alive. The result decides it, so only views
   * are ever wrapped: a pipeline ending in an action, an algorithm, @c to or @c materialize is called as normal.
   */
  template <typename T, typename F>
  concept needs_owner =
    (rvalue_container<T> or is_retained_closure_v<std::remove_cvref_t<F>>) and
    std::invocable<std::decay_t<F>&, std::remove_reference_t<T>&> and
    enable_view<
      std::remove_cvref_t<std::invoke_result_t<std::decay_t<F>&, std::remove_reference_t<T>&>>>;

  /**
   * Binds a second range to a closure without a dangling iterator. An lvalue is referred to rather than copied, a view
   * or a borrowed range is safe to hold as is, and any other rvalue is moved into a closure the pipe will keep alive.
   */
  export template <typename Fn, typename Rng, typename... Args>
  GENEX_INLINE constexpr auto bind_range_back(Fn fn, Rng &&rng, Args &&... args) {
    if constexpr (std::is_lvalue_reference_v<Rng>) {
      return meta::bind_back(
        std::move(fn), genex::subrange(iterators::begin(rng), iterators::end(rng)), std::forward<Args>(args)...);
    }
    else if constexpr (non_dangling_range<Rng>) {
      return meta::bind_back(std::move(fn), std::forward<Rng>(rng), std::forward<Args>(args)...);
    }
    else {
      auto closure = meta::bind_back(std::move(fn), std::forward<Rng>(rng), std::forward<Args>(args)...);
      return retained_closure<decltype(closure)>{std::move(closure)};
    }
  }
}

// Usage: auto range = vector | genex::views::___
export template <typename T, typename F> requires (not genex::detail::needs_owner<T, F>) and std::invocable<F, T>
GENEX_INLINE constexpr auto operator|(T &&lhs, F &&rhs) -> decltype(auto) {
  return genex::meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}

// Usage: auto range = make_vector() | genex::views::___
export template <typename T, typename F>
  requires genex::detail::needs_owner<T, F> and (not genex::detail::is_owning_view_v<T>)
GENEX_INLINE constexpr auto operator|(T &&lhs, F &&rhs) {
  return genex::detail::owning_view<T, std::decay_t<F>>(std::forward<T>(lhs), std::forward<F>(rhs));
}

// Usage: auto range = make_vector() | genex::views::___ | genex::views::___
export template <typename T, typename F>
  requires genex::detail::needs_owner<T, F> and genex::detail::is_owning_view_v<T>
GENEX_INLINE constexpr auto operator|(T &&lhs, F &&rhs) {
  using composed_t = genex::detail::composed_adaptor<typename T::adaptor_type, std::decay_t<F>>;
  auto composed = composed_t{std::move(lhs).adaptor(), std::forward<F>(rhs)};
  return genex::detail::owning_view<typename T::base_type, composed_t>(std::move(lhs).base(), std::move(composed));
}

// Usage: vector |= genex::actions::___
export template <typename T, typename F> requires std::invocable<F, T>
GENEX_INLINE constexpr auto operator|=(T &&lhs, F &&rhs) -> decltype(auto) {
  return genex::meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
}
