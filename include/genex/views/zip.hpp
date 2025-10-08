#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


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


namespace genex::views::detail {
    template <typename... Is, typename... Ss>
    requires concepts::zippable_iters<std::tuple<Is...>, std::tuple<Ss...>>
    auto do_zip(std::tuple<Is...> first, std::tuple<Ss...> last) -> generator<std::tuple<iter_value_t<Is>...>> {
        auto starts = std::move(first);
        auto ends = std::move(last);
        while (not any_iterator_finished(starts, ends)) {
            co_yield deref_tuple(starts);
            advance_tuple(starts);
        }
    }
}


namespace genex::views {
    struct zip_fn {
        template <typename... Is, typename... Ss>
        requires detail::concepts::zippable_iters<std::tuple<Is...>, std::tuple<Ss...>>
        GENEX_INLINE constexpr auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const {
            return detail::do_zip(std::move(first), std::move(last));
        }

        template <typename... Rngs>
        requires detail::concepts::zippable_range<Rngs...> and (sizeof...(Rngs) > 1)
        GENEX_INLINE constexpr auto operator()(Rngs &&... ranges) const {
            return detail::do_zip(std::make_tuple(iterators::begin(ranges)...), std::make_tuple(iterators::end(ranges)...));
        }

        template <typename Rng2>
        requires detail::concepts::zippable_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
            return meta::bind_back(zip_fn{}, std::forward<Rng2>(rng2));
        }
    };

    inline constexpr zip_fn zip{};
}
