#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename T1, typename T2>
    struct can_zip_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct can_zip_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (std::input_iterator<Is> and ...) and
            (std::sentinel_for<Ss, Is> and ...) and
            (std::movable<Is> and ...);
    };

    template <typename T1, typename T2>
    concept can_zip_iters = can_zip_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept can_zip_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...) and
        can_zip_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views::detail {
    template <typename... Is, typename... Ss> requires concepts::can_zip_iters<std::tuple<Is...>, std::tuple<Ss...>>
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
    DEFINE_VIEW(zip) {
        template <typename... Is, typename... Ss> requires concepts::can_zip_iters<std::tuple<Is...>, std::tuple<Ss...>>
        auto operator()(std::tuple<Is...> first, std::tuple<Ss...> last) const -> auto {
            auto gen = detail::do_zip(std::move(first), std::move(last));
            return zip_view(std::move(gen));
        }

        template <typename... Rngs> requires (concepts::can_zip_range<Rngs...> and sizeof...(Rngs) > 1)
        auto operator()(Rngs &&... ranges) const -> auto {
            return (*this)(std::make_tuple(iterators::begin(std::forward<Rngs>(ranges))...), std::make_tuple(iterators::end(std::forward<Rngs>(ranges))...));
        }

        template <typename Rng2> requires concepts::can_zip_range<Rng2>
        auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires concepts::can_zip_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_VIEW(zip);
}
