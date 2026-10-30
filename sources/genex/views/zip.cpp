module;
#include <genex/macros.hpp>

export module genex.views2.zip;
export import genex.pipe;
import genex.concepts;
import genex.subrange;
import genex.meta;
import genex.iterators.access;
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
  struct zip_iterator {
    std::tuple<iterator_t<Rngs>...> its;

    using value_type = std::tuple<iter_value_t<iterator_t<Rngs>>...>;
    using reference_type = std::tuple<iter_reference_t<iterator_t<Rngs>>...>;
    using difference_type = std::common_type_t<iter_difference_t<iterator_t<Rngs>>...>;
    using iterator_category = std::conditional_t<
      (std::random_access_iterator<iterator_t<Rngs>> and ...), std::random_access_iterator_tag, std::conditional_t<
        (std::bidirectional_iterator<iterator_t<Rngs>> and ...), std::bidirectional_iterator_tag, std::conditional_t<
          (std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>>>;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr zip_iterator() = default;

    GENEX_INLINE constexpr explicit zip_iterator(std::tuple<iterator_t<Rngs>...> its) :
      its(std::move(its)) {
    }

    // A zip ends as soon as *any* of its ranges does, so the sentinel test is a disjunction over
    // all of them rather than a comparison of the iterator tuples.
    GENEX_VIEW_ITER_EQ(zip_iterator, zip_sentinel<Rngs...>) {
      return genex::any_iterator_finished(self.its, that.sts);
    }

    GENEX_VIEW_ITER_EQ(zip_iterator, zip_iterator) {
      return self.its == that.its;
    }

    GENEX_INLINE friend constexpr auto operator!=(
      zip_iterator const &self, zip_sentinel<Rngs...> const &that) -> bool {
      return not(self == that);
    }

    GENEX_INLINE friend constexpr auto operator!=(
      zip_iterator const &self, zip_iterator const &that) -> bool {
      return not(self == that);
    }

    template <typename Self>
    GENEX_INLINE constexpr auto operator*(this Self &&self) -> decltype(auto) {
      return genex::deref_tuple(self.its);
    }

    template <typename Self>
    GENEX_INLINE constexpr auto operator++(this Self &&self) -> zip_iterator& {
      genex::advance_tuple(self.its);
      return self;
    }

    template <typename Self>
    GENEX_INLINE constexpr auto operator++(this Self &&self, int) -> zip_iterator {
      auto temp = self;
      ++self;
      return temp;
    }

    template <typename Self>
      requires (std::bidirectional_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator--(this Self &&self) -> zip_iterator& {
      genex::retreat_tuple(self.its);
      return self;
    }

    template <typename Self>
      requires (std::bidirectional_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator--(this Self &&self, int) -> zip_iterator {
      auto temp = self;
      --self;
      return temp;
    }

    template <typename Self>
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator+=(this Self &&self, difference_type n) -> zip_iterator& {
      genex::advance_tuple(self.its, n);
      return self;
    }

    template <typename Self>
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator-=(this Self &&self, difference_type n) -> zip_iterator& {
      genex::retreat_tuple(self.its, n);
      return self;
    }

    template <typename Self>
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator+(this Self &&self, difference_type n) -> zip_iterator {
      auto temp = self;
      temp += n;
      return temp;
    }

    template <typename Self>
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator-(this Self &&self, difference_type n) -> zip_iterator {
      auto temp = self;
      temp -= n;
      return temp;
    }

    template <typename Self>
      requires (std::random_access_iterator<iterator_t<Rngs>> and ...)
    GENEX_INLINE constexpr auto operator-(this Self &&self, zip_iterator const &that) -> difference_type {
      return static_cast<difference_type>(genex::distance_tuple(self.its, that.its));
    }
  };

  template <typename... Rngs>
    requires concepts::zippable_range<Rngs...>
  struct zip_view {
    std::tuple<iterator_t<Rngs>...> m_its;
    std::tuple<sentinel_t<Rngs>...> m_sts;

    GENEX_INLINE constexpr zip_view(std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts) :
      m_its(std::move(its)), m_sts(std::move(sts)) {
    }

    template <typename Self>
    GENEX_ITER_BEGIN {
      return zip_iterator<Rngs...>(self.m_its);
    }

    template <typename Self>
    GENEX_ITER_END {
      return zip_sentinel<Rngs...>{self.m_sts};
    }

    template <typename Self>
      requires (std::sized_sentinel_for<sentinel_t<Rngs>, iterator_t<Rngs>> and ...)
    GENEX_NODISCARD GENEX_INLINE constexpr auto size(this Self &&self) -> std::size_t {
      return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::min({static_cast<std::size_t>(std::get<Is>(self.m_sts) - std::get<Is>(self.m_its))...});
      }(std::index_sequence_for<Rngs...>{});
    }
  };
}

namespace genex::views {
  struct zip_fn {
    template <typename... Is, typename... Ss>
      requires detail::concepts::zippable_iters<std::tuple<Is...>, std::tuple<Ss...>>
    GENEX_INLINE constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const noexcept(
      SAFE_IMPL_CTOR(zip_view, std::tuple<Is...>, std::tuple<Ss...>) and
      SAFE_MOVE(std::tuple<Is...>) and SAFE_MOVE(std::tuple<Ss...>)) {
      return detail::impl::zip_view<genex::subrange<Is, Ss>...>(std::move(first), std::move(last));
    }

    template <typename... Rngs>
      requires (detail::concepts::zippable_range<Rngs...> and sizeof...(Rngs) > 1)
    GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const noexcept(
      SAFE_MOVE(std::tuple<iterator_t<Rngs>...>) and SAFE_MOVE(std::tuple<sentinel_t<Rngs>...>)) {
      return detail::impl::zip_view<Rngs...>(
        std::make_tuple(iterators::begin(ranges)...),
        std::make_tuple(iterators::end(ranges)...));
    }

    template <typename Rng2>
      requires detail::concepts::zippable_range<Rng2>
    GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const noexcept(
      SAFE_CTOR(zip_fn)) {
      return meta::bind_back(zip_fn{}, std::forward<Rng2>(rng2));
    }
  };

  export inline constexpr zip_fn zip{};
}
