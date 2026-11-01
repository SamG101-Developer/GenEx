module;
#include <genex/detail/macros.hpp>

export module genex.pipe;
export import genex.closure;
import genex.concepts;
import genex.detail.range_closure;
import genex.iterators.access;
import genex.meta;
import std;

namespace genex::detail {
  /// Applies "f" and then "g", so that a pipeline over an owned range rebuilds
  /// its views once per "begin" or "end", rather than once per view per call.
  template <typename F, typename G>
  struct composed_adaptor : pipe_closure {
    // Not "GENEX_NO_UNIQUE_ADDRESS". Both of these and this struct share the
    // "pipe_closure" base, and with them overlapping, "reverse | take(2)" threw
    // "vector too long" on MSVC.
    F f;
    G g;

    // The pipe between them is so that a step producing a container can still
    // be followed by a view. Directly calling this with an rvalue is the one
    // way left to get a view into something about to be destroyed, so that's
    // rejected.
    template <typename Self, typename Rng>
    GENEX_INLINE constexpr auto operator()(
      this Self &&self, Rng &&rng) -> decltype(auto) {
      static_assert(
        non_dangling_range<Rng> or not enable_view<std::remove_cvref_t<decltype(meta::invoke(self.f, rng) | self.g)>>,
        "A view would dangle here: pipe an rvalue into this, rather than calling it, so the pipe can keep it alive.");
      return meta::invoke(self.f, rng) | self.g;
    }
  };

  /// Keeping a composition alive keeps alive everything in it.
  template <typename F, typename G>
  inline constexpr bool is_retained_closure_v<composed_adaptor<F, G>> =
    is_retained_closure_v<F> or is_retained_closure_v<G>;

  /// A view that keeps alive the storage another view points into. "Base" is
  /// the piped range: held by value when it is an rvalue, and by reference
  /// otherwise. The inner view is rebuilt from "Base" for each "begin" and
  /// "end", because moving a container can invalidate its iterators (a small
  /// "std::string", for example). Nothing else about a view has to change to be
  /// owned, as genex iterators hold copies of what they use.
  ///
  /// It deliberately has no @c view_base: piping one treats it like an rvalue
  /// container, and it is move-only.
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
    [[nodiscard]] GENEX_INLINE constexpr auto begin(this Self &&self) {
      return iterators::begin(meta::invoke(self.m_adaptor, self.m_base));
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto end(this Self &&self) {
      return iterators::end(meta::invoke(self.m_adaptor, self.m_base));
    }

    template <typename Self>
    requires requires(Self &s) { meta::invoke(s.m_adaptor, s.m_base).size(); }
    [[nodiscard]] GENEX_INLINE constexpr auto size(this Self &&self) {
      return meta::invoke(self.m_adaptor, self.m_base).size();
    }

    template <typename Self>
    requires requires(Self &s) { meta::invoke(s.m_adaptor, s.m_base).reserve_hint(); }
    [[nodiscard]] GENEX_INLINE constexpr auto reserve_hint(this Self &&self) {
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
  inline constexpr auto is_owning_view_v = false;

  template <typename Base, typename Adaptor>
  inline constexpr auto is_owning_view_v<owning_view<Base, Adaptor>> = true;

  template <typename T>
  concept rvalue_container =
    not non_dangling_range<T> and
    input_range<std::remove_reference_t<T>&>;

  /// Whether piping "T" into "F" returns a view that needs storage kept alive.
  /// The result decides it, so only views are ever wrapped: a pipeline ending
  /// in an action, an algorithm, @c to or @c materialize is called as normal.
  template <typename T, typename F>
  concept needs_owner =
    (rvalue_container<T> or is_retained_closure_v<std::remove_cvref_t<F>>) and
    std::invocable<std::decay_t<F>&, std::remove_reference_t<T>&> and
    enable_view<
      std::remove_cvref_t<std::invoke_result_t<std::decay_t<F>&, std::remove_reference_t<T>&>>>;
}

namespace genex {
  // Usage: auto range = vector | genex::views::___
  export template <typename T, pipeable F>
    requires (not pipeable<T>)
    and (not detail::needs_owner<T, F>)
    and std::invocable<F, T>
    and std::is_object_v<std::invoke_result_t<F, T>>
  [[nodiscard]] GENEX_INLINE constexpr auto operator|(
    T &&lhs, F &&rhs) -> decltype(auto) {
    return meta::invoke(
      std::forward<F>(rhs), std::forward<T>(lhs));
  }

  // Usage: vector | genex::actions::___
  export template <typename T, pipeable F>
    requires (not pipeable<T>)
    and (not detail::needs_owner<T, F>)
    and std::invocable<F, T>
    and (not std::is_object_v<std::invoke_result_t<F, T>>)
  GENEX_INLINE constexpr auto operator|(
    T &&lhs, F &&rhs) -> decltype(auto) {
    return meta::invoke(
      std::forward<F>(rhs), std::forward<T>(lhs));
  }

  // Usage: auto range = make_vector() | genex::views::___
  export template <typename T, pipeable F>
    requires (not pipeable<T>)
    and detail::needs_owner<T, F>
    and (not detail::is_owning_view_v<T>)
  [[nodiscard]] GENEX_INLINE constexpr auto operator|(
    T &&lhs, F &&rhs) {
    return detail::owning_view<T, std::decay_t<F>>(
      std::forward<T>(lhs), std::forward<F>(rhs));
  }

  // Usage: auto range = make_vector() | genex::views::___ | genex::views::___
  export template <typename T, pipeable F>
    requires (not pipeable<T>)
    and detail::needs_owner<T, F>
    and detail::is_owning_view_v<T>
  [[nodiscard]] GENEX_INLINE constexpr auto operator|(
    T &&lhs, F &&rhs) {
    using composed_t = detail::composed_adaptor<typename T::adaptor_type, std::decay_t<F>>;
    auto composed = composed_t{{}, std::move(lhs).adaptor(), std::forward<F>(rhs)};
    return detail::owning_view<typename T::base_type, composed_t>(
      std::move(lhs).base(), std::move(composed));
  }

  // Usage: auto step = genex::views::___ | genex::views::___
  export template <pipeable T, pipeable F>
  [[nodiscard]] GENEX_INLINE constexpr auto operator|(
    T &&lhs, F &&rhs) {
    return detail::composed_adaptor<std::decay_t<T>, std::decay_t<F>>{
      {}, std::forward<T>(lhs), std::forward<F>(rhs)};
  }

  // Usage: vector |= genex::actions::___
  export template <typename T, pipeable F> requires std::invocable<F, T>
  GENEX_INLINE constexpr auto operator|=(T &&lhs, F &&rhs) -> decltype(auto) {
    return meta::invoke(std::forward<F>(rhs), std::forward<T>(lhs));
  }
}
