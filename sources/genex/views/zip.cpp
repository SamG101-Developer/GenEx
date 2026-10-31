module;
#include <genex/detail/macros.hpp>

export module genex.views.zip;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.detail.range_closure;
import genex.detail.tuple;
import genex.iterators.access;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <typename T1, typename T2>
  struct zippable_iters_helper : std::false_type {
  };

  template <typename... Is, typename... Ss>
  struct zippable_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
    static constexpr bool value =
      sizeof...(Is) > 0 and
      sizeof...(Is) == sizeof...(Ss) and
      (std::input_iterator<Is> and ...) and
      (std::sentinel_for<Ss, Is> and ...);
  };

  template <typename T1, typename T2>
  concept zippable_iters = zippable_iters_helper<T1, T2>::value;

  template <typename... Rngs>
  concept zippable_range =
    (non_dangling_range<Rngs> and ...) and
    sizeof...(Rngs) > 0 and
    (input_range<Rngs> and ...) and
    zippable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}

namespace genex::views::detail::impl {
  template <typename... Rngs>
  requires concepts::zippable_range<Rngs...>
  struct zip_sentinel {
    std::tuple<sentinel_t<Rngs>...> sts;
  };

  template <typename... Rngs>
  requires concepts::zippable_range<Rngs...>
  struct zip_iterator : genex::detail::iterator_ops<zip_iterator<Rngs...>> {
    std::tuple<iterator_t<Rngs>...> its;

    using value_type = std::tuple<iter_value_t<iterator_t<Rngs>>...>;
    using reference = std::tuple<iter_reference_t<iterator_t<Rngs>>...>;
    using difference_type = std::common_type_t<iter_difference_t<iterator_t<Rngs>>...>;
    using iterator_concept = std::conditional_t<
      (std::random_access_iterator<iterator_t<Rngs>> and ...), std::random_access_iterator_tag, std::conditional_t<
      (std::bidirectional_iterator<iterator_t<Rngs>> and ...), std::bidirectional_iterator_tag, std::conditional_t<
      (std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>>>;
    using iterator_category = legacy_iter_category_t<reference, iterator_concept>;

    GENEX_INLINE constexpr zip_iterator() = default;

    GENEX_INLINE constexpr explicit zip_iterator(
      std::tuple<iterator_t<Rngs>...> its) :
      its(std::move(its)) {
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return genex::detail::deref_tuple(its);
    }

    GENEX_INLINE constexpr auto next() -> void {
      genex::detail::advance_tuple(its);
    }

    GENEX_INLINE constexpr auto prev() -> void
    requires (std::bidirectional_iterator<iterator_t<Rngs>> and ...) {
      genex::detail::retreat_tuple(its);
    }

    GENEX_INLINE constexpr auto advance_by(
      const difference_type n) -> void
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...) {
      genex::detail::advance_tuple(its, n);
    }

    GENEX_INLINE constexpr auto distance_to(
      const zip_iterator &that) const -> difference_type
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...) {
      return static_cast<difference_type>(
        genex::detail::distance_tuple(that.its, its));
    }

    GENEX_INLINE friend constexpr auto operator-(
      const zip_sentinel<Rngs...> &that,
      const zip_iterator &self) -> difference_type
    requires (std::sized_sentinel_for<sentinel_t<Rngs>, iterator_t<Rngs>> and ...) {
      return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> difference_type {
        return std::min({static_cast<difference_type>(std::get<Is>(that.sts) - std::get<Is>(self.its))...});
      }(std::index_sequence_for<Rngs...>{});
    }

    GENEX_INLINE friend constexpr auto operator-(
      const zip_iterator &self,
      const zip_sentinel<Rngs...> &that) -> difference_type
    requires (std::sized_sentinel_for<sentinel_t<Rngs>, iterator_t<Rngs>> and ...) {
      return -(that - self);
    }

    /// A zip ends as soon as *any* of its ranges does, so the sentinel test is
    /// a disjunction over all of them rather than a comparison of the iterator tuples.
    GENEX_INLINE friend constexpr auto operator==(
      const zip_iterator &self, const zip_sentinel<Rngs...> &that) -> bool {
      return genex::detail::any_iterator_finished(self.its, that.sts);
    }

    GENEX_INLINE friend constexpr auto operator==(
      const zip_iterator &self, const zip_iterator &that) -> bool {
      return self.its == that.its;
    }
  };

  template <typename... Rngs>
  requires concepts::zippable_range<Rngs...>
  struct zip_view : view_base {
    std::tuple<iterator_t<Rngs>...> m_its;
    std::tuple<sentinel_t<Rngs>...> m_sts;

    GENEX_INLINE constexpr zip_view(
      std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts) noexcept(
      SAFE_MOVE(std::tuple<iterator_t<Rngs>...>) and SAFE_MOVE(std::tuple<sentinel_t<Rngs>...>)) :
      m_its(std::move(its)), m_sts(std::move(sts)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return zip_iterator<Rngs...>(m_its);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return zip_sentinel<Rngs...>{m_sts};
    }

    template <typename Self>
    requires (std::sized_sentinel_for<sentinel_t<Rngs>, iterator_t<Rngs>> and ...)
    [[nodiscard]] GENEX_INLINE constexpr auto size(this Self &&self) -> std::size_t {
      return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::min({static_cast<std::size_t>(std::get<Is>(self.m_sts) - std::get<Is>(self.m_its))...});
      }(std::index_sequence_for<Rngs...>{});
    }
  };
}

namespace genex::views {
  struct zip_fn : pipe_closure {
    template <typename... Is, typename... Ss>
    requires detail::concepts::zippable_iters<std::tuple<Is...>, std::tuple<Ss...>>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      std::tuple<Is...> first, std::tuple<Ss...> last) const noexcept(
      SAFE_CTOR(detail::impl::zip_view<genex::subrange<Is, Ss>...>, std::tuple<Is...>, std::tuple<Ss...>) and
      SAFE_MOVE(std::tuple<Is...>) and SAFE_MOVE(std::tuple<Ss...>)) {
      return detail::impl::zip_view<genex::subrange<Is, Ss>...>(std::move(first), std::move(last));
    }

    template <typename... Rngs>
    requires (detail::concepts::zippable_range<Rngs...> and sizeof...(Rngs) > 1)
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const noexcept(
      (SAFE_CALL(decltype(iterators::begin), Rngs&) and ...) and
      (SAFE_CALL(decltype(iterators::end), Rngs&) and ...) and
      SAFE_MOVE(std::tuple<iterator_t<Rngs>...>) and SAFE_MOVE(std::tuple<sentinel_t<Rngs>...>)) {
      return detail::impl::zip_view<Rngs...>(
        std::make_tuple(iterators::begin(ranges)...),
        std::make_tuple(iterators::end(ranges)...));
    }

    template <typename Rng2>
    requires input_range<Rng2>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
      return genex::detail::bind_range_back(zip_fn{}, std::forward<Rng2>(rng2));
    }
  };

  export inline constexpr zip_fn zip{};
}
