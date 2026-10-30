module;
#include <genex/detail/macros.hpp>

export module genex.views.concat;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.iterators.access;
import genex.iterators.distance;
import genex.subrange;
import std;

namespace genex::views::detail::concepts {
  template <typename T1, typename T2>
  struct concatable_iters_helper : std::false_type {
  };

  template <typename... Is, typename... Ss>
  struct concatable_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
    static constexpr bool value =
      sizeof...(Is) > 0 and
      sizeof...(Is) == sizeof...(Ss) and
      (std::input_iterator<Is> and ...) and
      (std::sentinel_for<Ss, Is> and ...) and
      (std::indirectly_readable<Is> and ...);
  };

  template <typename T1, typename T2>
  concept concatable_iters =
    concatable_iters_helper<T1, T2>::value;

  template <typename... Rngs>
  concept concatable_range =
    (non_dangling_range<Rngs> and ...) and
    (input_range<Rngs> and ...) and
    concatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}

namespace genex::views::detail::impl {
  struct concat_sentinel {};

  template <typename... Rngs>
    requires concepts::concatable_range<Rngs...>
  struct concat_iterator : genex::detail::iterator_ops<concat_iterator<Rngs...>> {
    std::tuple<iterator_t<Rngs>...> its;
    std::tuple<sentinel_t<Rngs>...> sts;
    std::size_t active = 0;

    using value_type = std::common_type_t<iter_value_t<iterator_t<Rngs>>...>;
    using reference = std::common_reference_t<iter_reference_t<iterator_t<Rngs>>...>;
    using difference_type = std::common_type_t<iter_difference_t<iterator_t<Rngs>>...>;
    using iterator_category = std::conditional_t<
      (std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>;
    using iterator_concept = iterator_category;

    GENEX_INLINE constexpr concat_iterator() = default;

    GENEX_INLINE constexpr concat_iterator(std::tuple<iterator_t<Rngs>...> first,
      std::tuple<sentinel_t<Rngs>...> last) :
      its(std::move(first)), sts(std::move(last)) {
      settle<0>();
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return deref_impl<0>();
    }

    GENEX_INLINE constexpr auto next() -> void {
      inc_impl<0>();
    }

    GENEX_INLINE friend constexpr auto operator==(const concat_iterator &self, const concat_iterator &that) -> bool {
      return self.its == that.its;
    }

    GENEX_INLINE friend constexpr auto operator==(const concat_iterator &self, const concat_sentinel &) -> bool {
      return self.active == sizeof...(Rngs);
    }

  private:
    template <std::size_t I, typename Self>
    GENEX_INLINE constexpr auto deref_impl(this Self &&self) -> reference {
      if constexpr (I < sizeof...(Rngs)) {
        if (self.active == I) { return *std::get<I>(self.its); }
        return self.template deref_impl<I + 1>();
      }
      else {
        std::unreachable();
      }
    }

    template <std::size_t I, typename Self>
    GENEX_INLINE constexpr auto inc_impl(this Self &&self) -> void {
      if constexpr (I < sizeof...(Rngs)) {
        if (self.active == I) {
          ++std::get<I>(self.its);
          self.template settle<I>();
          return;
        }
        self.template inc_impl<I + 1>();
      }
    }

    template <std::size_t I, typename Self>
    GENEX_INLINE constexpr auto settle(this Self &&self) -> void {
      if constexpr (I < sizeof...(Rngs)) {
        if (std::get<I>(self.its) == std::get<I>(self.sts)) {
          self.active = I + 1;
          self.template settle<I + 1>();
        }
      }
    }
  };

  template <typename... Rngs>
    requires concepts::concatable_range<Rngs...>
  struct concat_view : view_base {
    std::tuple<iterator_t<Rngs>...> its;
    std::tuple<sentinel_t<Rngs>...> sts;

    GENEX_INLINE constexpr concat_view(
      std::tuple<iterator_t<Rngs>...> first, std::tuple<sentinel_t<Rngs>...> last) noexcept(
      SAFE_MOVE(std::tuple<iterator_t<Rngs>...>) and SAFE_MOVE(std::tuple<sentinel_t<Rngs>...>)) :
      its(std::move(first)), sts(std::move(last)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return concat_iterator<Rngs...>(its, sts);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return concat_sentinel{};
    }

    template <typename Self>
      requires (sized_range<Rngs> and ...)
    [[nodiscard]] GENEX_INLINE constexpr auto size(this Self &&self) -> std::size_t {
      return self.template size_impl<0>();
    }

    // Each range can be inserted in one go, which a single loop stepping between them would have to do one by one.
    template <typename Out>
      requires (sized_range<Rngs> and ...)
      and (std::same_as<iterator_t<Rngs>, sentinel_t<Rngs>> and ...)
      and (std::derived_from<typename std::iterator_traits<iterator_t<Rngs>>::iterator_category,
                             std::forward_iterator_tag> and ...)
      and (requires(Out &out, iterator_t<Rngs> i) { out.insert(out.end(), i, i); } and ...)
    constexpr auto append_to(Out &out) const -> void {
      out.reserve(size());
      [&]<std::size_t... Is>(std::index_sequence<Is...>) -> void {
        (out.insert(out.end(), std::get<Is>(its), std::get<Is>(sts)), ...);
      }(std::index_sequence_for<Rngs...>{});
    }

  private:
    template <std::size_t I, typename Self>
      requires (sized_range<Rngs> and ...)
    GENEX_INLINE constexpr auto size_impl(this Self &&self) -> std::size_t {
      if constexpr (I >= sizeof...(Rngs)) { return 0; }
      else {
        auto const &it = std::get<I>(self.its);
        auto const &st = std::get<I>(self.sts);
        return static_cast<std::size_t>(iterators::distance(it, st)) + self.template size_impl<I + 1>();
      }
    }
  };
}

namespace genex::views {
  struct concat_fn {
    template <typename... Is, typename... Ss>
      requires detail::concepts::concatable_iters<std::tuple<Is...>, std::tuple<Ss...>>
    GENEX_INLINE constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const noexcept(
      SAFE_CTOR(detail::impl::concat_view<genex::subrange<Is, Ss>...>, std::tuple<Is...>, std::tuple<Ss...>) and
      SAFE_MOVE(std::tuple<Is...>) and SAFE_MOVE(std::tuple<Ss...>)) {
      return detail::impl::concat_view<genex::subrange<Is, Ss>...>(std::move(first), std::move(last));
    }

    template <typename... Rngs>
      requires (detail::concepts::concatable_range<Rngs...> and sizeof...(Rngs) > 1)
    GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const noexcept(
      (SAFE_CALL(decltype(iterators::begin), Rngs&) and ...) and
      (SAFE_CALL(decltype(iterators::end), Rngs&) and ...) and
      SAFE_MOVE(std::tuple<iterator_t<Rngs>...>) and SAFE_MOVE(std::tuple<sentinel_t<Rngs>...>)) {
      return detail::impl::concat_view<Rngs...>(
        std::make_tuple(iterators::begin(ranges)...),
        std::make_tuple(iterators::end(ranges)...));
    }
  };

  export inline constexpr concat_fn concat{};
}
