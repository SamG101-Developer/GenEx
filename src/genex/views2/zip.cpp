module;
#include <genex/macros.hpp>

export module genex.views2.zip;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.access;
import genex.operations.tuples;
import std;


namespace genex::views2::detail::concepts {
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


namespace genex::views2::detail::impl {
    struct zip_sentinel {};

    template <typename ...Rngs>
    requires concepts::zippable_range<Rngs...>
    struct zip_iterator {
        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;

        using value_type = std::tuple<iter_value_t<iterator_t<Rngs>>...>;
        using reference_type = std::tuple<iter_reference_t<iterator_t<Rngs>>...>;
        using difference_type = std::ptrdiff_t;
        using iterator_category =
            std::conditional_t<(std::random_access_iterator<iterator_t<Rngs>> and ...), std::random_access_iterator_tag,
            std::conditional_t<(std::bidirectional_iterator<iterator_t<Rngs>> and ...), std::bidirectional_iterator_tag,
            std::conditional_t<(std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>>>;
        using iterator_concept = iterator_category;

        GENEX_INLINE constexpr zip_iterator() = default;

        GENEX_INLINE constexpr zip_iterator(std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts) :
            its(std::move(its)), sts(std::move(sts)) {
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator==(this Self &&self, zip_sentinel const&) -> bool {
            return self.any_iterator_finished(self.its, self.sts);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator==(this Self &&self, zip_iterator const &that) -> bool {
            return self.its == that.its;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator!=(this Self &&self, zip_sentinel const& that) -> bool {
            return not (self == that);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator!=(this Self &&self, zip_iterator const &that) -> bool {
            return not (self.its == that.its);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator*(this Self &&self) -> decltype(auto) {
            return deref_tuple(self.its);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator++(this Self &&self) -> Self& {
            advance_tuple(self.its);
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator++(this Self &&self, int) -> zip_iterator {
            auto temp = self;
            ++self;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator--(this Self &&self) -> Self&
        requires (bidirectional_range<Rngs> and ...) {
            retreat_tuple(self.its);
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator--(this Self &&self, int) -> zip_iterator
        requires (bidirectional_range<Rngs> and ...) {
            auto temp = self;
            --self;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator+=(this Self &&self, difference_type n) -> zip_iterator&
        requires (random_access_range<Rngs> and ...) {
            advance_tuple(self.its, n);
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator+(this Self &&self, difference_type n) -> zip_iterator
        requires (random_access_range<Rngs> and ...) {
            auto temp = self;
            temp += n;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator+(this Self &&self, zip_iterator const &that) -> difference_type
        requires (random_access_range<Rngs> and ...) {
            return distance_tuple(self.its, that.its);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator-=(this Self &&self, difference_type n) -> zip_iterator&
        requires (random_access_range<Rngs> and ...) {
            self.retreat_tuple(self.its, n);
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator-(this Self &&self, difference_type n) -> zip_iterator
        requires (random_access_range<Rngs> and ...) {
            auto temp = self;
            temp -= n;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator-(this Self &&self, zip_iterator const &that) -> difference_type
        requires (random_access_range<Rngs> and ...) {
            return distance_tuple(self.its, that.its);
        }

    private:
        template <std::size_t... Is>
        constexpr auto any_iterator_finished_impl(
            std::tuple<iterator_t<Rngs>...> const &its, std::tuple<sentinel_t<Rngs>...> const &sts, std::index_sequence<Is...>) -> bool {
            return (... or (std::get<Is>(its) == std::get<Is>(sts)));
        }

        constexpr auto any_iterator_finished(
            std::tuple<iterator_t<Rngs>...> const &its, std::tuple<sentinel_t<Rngs>...> const &sts) -> bool {
            return any_iterator_finished_impl(its, sts, std::index_sequence_for<Rngs...>{});
        }
    };

    template <typename ...Rngs>
    requires concepts::zippable_range<Rngs...>
    struct zip_view {
        std::tuple<iterator_t<Rngs>...> m_its;
        std::tuple<sentinel_t<Rngs>...> m_sts;

        GENEX_INLINE constexpr zip_view(std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts) :
            m_its(std::move(its)), m_sts(std::move(sts)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return zip_iterator<Rngs...>(self.m_its, self.m_sts);
        }

        template <typename Self>
        GENEX_ITER_END {
            return zip_sentinel();
        }
    };
}


namespace genex::views2 {
    struct zip_fn {
        template <typename... Is, typename... Ss>
        requires detail::concepts::zippable_iters<std::tuple<Is...>, std::tuple<Ss...>>
        GENEX_INLINE constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const {
            return detail::impl::zip_view<std::ranges::subrange<Is, Ss>...>(std::move(first), std::move(last));
        }

        template <typename... Rngs>
        requires detail::concepts::zippable_range<Rngs...> and (sizeof...(Rngs) > 1)
        GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const {
            return detail::impl::zip_view<Rngs...>(
                std::make_tuple(iterators::begin(std::forward<Rngs>(ranges))...),
                std::make_tuple(iterators::end(std::forward<Rngs>(ranges))...));
        }

        template <typename Rng2>
        requires detail::concepts::zippable_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
            return meta::bind_back(zip_fn{}, std::forward<Rng2>(rng2));
        }
    };

    export constexpr zip_fn zip{};
}
