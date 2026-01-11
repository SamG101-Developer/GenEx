module;
#include <genex/macros.hpp>

export module genex.views2.concat;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.algorithms.tuple;
import genex.iterators.access;
import genex.iterators.distance;
import std;


namespace genex::views2::detail::concepts {
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
        (input_range<Rngs> and ...) and
        concatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views2::detail::impl {
    struct concat_sentinel {};

    template <typename... Rngs>
    requires concepts::concatable_range<Rngs...>
    struct concat_iterator {
        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;

        using value_type = std::common_type_t<iter_value_t<iterator_t<Rngs>>...>;
        using reference_type = std::common_reference_t<iter_reference_t<iterator_t<Rngs>>...>;
        using difference_type = std::common_type_t<iter_difference_t<iterator_t<Rngs>>...>;
        using iterator_category =
            std::conditional_t<(std::random_access_iterator<iterator_t<Rngs>> and ...), std::random_access_iterator_tag,
            std::conditional_t<(std::bidirectional_iterator<iterator_t<Rngs>> and ...), std::bidirectional_iterator_tag,
            std::conditional_t<(std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>>>;
        using iterator_concept = iterator_category;

        GENEX_INLINE constexpr concat_iterator() = default;

        GENEX_INLINE constexpr concat_iterator(std::tuple<iterator_t<Rngs>...> first, std::tuple<sentinel_t<Rngs>...> last) :
            its(std::move(first)), sts(std::move(last)) {
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator==(this Self &&self, concat_iterator const &that) -> bool {
            return self.its == that.its;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator==(this Self &&self, concat_sentinel const&) -> bool {
            return self.template is_end_impl<0>();
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator!=(this Self &&self, concat_iterator const &that) -> bool {
            return not (self == that);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator!=(this Self &&self, concat_sentinel const& that) -> bool {
            return not (self == that);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator*(this Self &&self) -> decltype(auto) {
            return self.template deref_impl<0>();
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator++(this Self &&self) -> concat_iterator& {
            self.template inc_impl<0>();
            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator++(this Self &&self, int) -> concat_iterator {
            auto temp = self;
            ++self;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator--(this Self &&self) -> concat_iterator&
        requires (bidirectional_range<Rngs> && ...) {
            self.template dec_impl<sizeof...(Rngs) - 1>();
            self.bwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator--(this Self &&self, int) -> concat_iterator
        requires (bidirectional_range<Rngs> && ...) {
            auto temp = self;
            --self;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator+=(this Self &&self, difference_type n) -> concat_iterator&
        requires (random_access_range<Rngs> && ...) {
            self.add(n);
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator+(this Self &&self, difference_type n) -> concat_iterator
        requires (random_access_range<Rngs> && ...) {
            auto temp = self;
            temp += n;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator+(this Self &&self, concat_iterator const &that) -> difference_type
        requires (random_access_range<Rngs> && ...) {
            return concat_iterator::dist(self, that);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator-(this Self &&self, difference_type n) -> concat_iterator&
        requires (random_access_range<Rngs> && ...) {
            self.sub(n);
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator-(this Self &&self, difference_type n) -> concat_iterator
        requires (random_access_range<Rngs> && ...) {
            auto temp = self;
            temp -= n;
            return temp;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator-(this Self &&self, concat_iterator const &that) -> difference_type
        requires (random_access_range<Rngs> && ...) {
            return concat_iterator::dist(self, that);
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self &&self) -> void {
            self.template fwd_to_valid_impl<0>();
        }

        template <typename Self>
        GENEX_INLINE constexpr auto bwd_to_valid(this Self &&self) -> void {
            self.template bwd_to_valid_impl<sizeof...(Rngs) - 1>();
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid_impl(this Self &&self) -> void {
            if constexpr (I < sizeof...(Rngs)) {
                auto const &it = genex::get<I>(self.its);
                auto const &st = genex::get<I>(self.sts);
                if (it == st) { self.template fwd_to_valid_impl<I + 1>(); }
            }
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto bwd_to_valid_impl(this Self &&self) -> void {
            if constexpr (I > 0) {
                auto const &it = genex::get<I>(self.its);
                auto const &st = genex::get<I - 1>(self.sts);
                if (it == st) { self.template bwd_to_valid_impl<I - 1>(); }
            }
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto deref_impl(this Self &&self) -> reference_type {
            if constexpr (I < sizeof...(Rngs)) {
                auto const &it = genex::get<I>(self.its);
                auto const &st = genex::get<I>(self.sts);
                if (it != st) { return *it; }
                return self.template deref_impl<I + 1>();
            }
            else {
                std::unreachable();
            }
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto inc_impl(this Self &&self) -> void {
            if constexpr (I < sizeof...(Rngs)) {
                auto &it = genex::get<I>(self.its);
                auto const &st = genex::get<I>(self.sts);
                if (it != st) { ++it; }
                else { self.template inc_impl<I + 1>(); }
            }
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto dec_impl(this Self &&self) -> void {
            if constexpr (I > 0) {
                auto &it = genex::get<I>(self.its);
                auto const &st = genex::get<I - 1>(self.sts);
                if (it != st) { --it; }
                else { self.template dec_impl<I - 1>(); }
            }
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto is_end_impl(this Self &&self) -> bool {
            if constexpr (I < sizeof...(Rngs)) {
                auto const &it = genex::get<I>(self.its);
                auto const &st = genex::get<I>(self.sts);
                if (it != st) { return false; }
                return self.template is_end_impl<I + 1>();
            }
            else {
                return true;
            }
        }

        template <typename Self>
        GENEX_INLINE constexpr auto add(this Self &&self, difference_type n) -> void
        requires (random_access_range<Rngs> && ...) {
            self.template add_impl<0>(n);
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto add_impl(this Self &&self, difference_type n) -> void
        requires (random_access_range<Rngs> && ...) {
            if constexpr (I >= sizeof...(Rngs)) { return; }
            auto const &it = genex::get<I>(self.its);
            auto const &st = genex::get<I>(self.sts);
            const auto dist = st - it;
            n < dist ? (it += n) : (it = st, self.template add_impl<I + 1>(n - dist));
        }

        template <typename Self>
        GENEX_INLINE constexpr auto sub(this Self &&self, difference_type n) -> void
        requires (random_access_range<Rngs> && ...) {
            self.template sub_impl<sizeof...(Rngs) - 1>(n);
        }

        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto sub_impl(this Self &&self, difference_type n) -> void
        requires (random_access_range<Rngs> && ...) {
            if constexpr (I == 0) { return; }
            auto const &it = genex::get<I>(self.its);
            auto const &st = genex::get<I - 1>(self.sts);
            const auto dist = it - st;
            n < dist ? (it -= n) : (it = st, self.template sub_impl<I - 1>(n - dist));
        }

        GENEX_INLINE static constexpr auto dist(concat_iterator const& a, concat_iterator const& b) -> difference_type
        requires (random_access_range<Rngs> && ...) {
            return concat_iterator::dist_impl<0>(a, b);
        }

        template <std::size_t I>
        GENEX_INLINE static constexpr auto dist_impl(concat_iterator const& a, concat_iterator const& b) -> difference_type
        requires (random_access_range<Rngs> && ...) {
            if constexpr (I >= sizeof...(Rngs)) { return 0; }
            auto const &a_it = genex::get<I>(a.its);
            auto const &a_st = genex::get<I>(a.sts);
            auto const &b_it = genex::get<I>(b.its);
            return a_it != a_st or b_it != a_st ? a_it - b_it : dist_impl<I + 1>(a, b);
        }
    };

    template <typename... Rngs>
    requires concepts::concatable_range<Rngs...>
    struct concat_view {
        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;

        GENEX_INLINE constexpr concat_view(std::tuple<iterator_t<Rngs>...> first, std::tuple<sentinel_t<Rngs>...> last) :
            its(std::move(first)), sts(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return concat_iterator<Rngs...>(std::move(self.its), std::move(self.sts));
        }

        template <typename Self>
        GENEX_ITER_END {
            return concat_sentinel{};
        }

        template <typename Self>
        GENEX_ITER_SIZE
        requires (sized_range<Rngs> && ...) {
            return self.template size_impl<0>();
        }

    private:
        template <std::size_t I, typename Self>
        GENEX_INLINE constexpr auto size_impl(this Self &&self) -> std::size_t
        requires (sized_range<Rngs> && ...) {
            if constexpr (I >= sizeof...(Rngs)) { return 0; }
            auto const &it = genex::get<I>(self.its);
            auto const &st = genex::get<I>(self.sts);
            return iterators::distance(it, st) + self.template size_impl<I + 1>(self);
        }
    };
}


namespace genex::views2 {
    struct concat_fn {
        template <typename... Is, typename... Ss>
        requires detail::concepts::concatable_iters<std::tuple<Is...>, std::tuple<Ss...>>
        GENEX_INLINE constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const {
            return detail::impl::concat_view<std::ranges::subrange<Is, Ss>...>(std::move(first), std::move(last));
        }

        template <typename... Rngs>
        requires (detail::concepts::concatable_range<Rngs...> and sizeof...(Rngs) > 1)
        GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const {
            return detail::impl::concat_view<Rngs...>(
                std::make_tuple(iterators::begin(std::forward<Rngs>(ranges))...),
                std::make_tuple(iterators::end(std::forward<Rngs>(ranges))...));
        }

        template <typename Rng2>
        requires detail::concepts::concatable_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
            return [rng2=std::forward<Rng2>(rng2)]<typename Rng1>(Rng1 &&rng1) mutable {
                return concat_fn{}(std::forward<Rng1>(rng1), std::move(rng2));
            };
        }
    };

    export inline constexpr concat_fn concat{};
}
