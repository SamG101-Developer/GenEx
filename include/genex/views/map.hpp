#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename F, typename Proj = meta::identity>
    concept can_map_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_unary_invocable<Proj, I> and
        std::indirectly_unary_invocable<F, std::projected<I, Proj>>;

    template <typename Rng, typename F, typename Proj = meta::identity>
    concept can_map_range =
        input_range<Rng> and
        can_map_iters<iterator_t<Rng>, sentinel_t<Rng>, F, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename F, typename Proj> requires concepts::can_map_iters<I, S, F, Proj>
    auto do_map(I first, S last, F &&f, Proj &&proj) -> generator<std::invoke_result_t<F, std::invoke_result_t<Proj, iter_value_t<I>>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(f), std::invoke(std::forward<Proj>(proj), *first));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(map) {
        template <typename I, typename S, typename F, typename Proj = meta::identity> requires concepts::can_map_iters<I, S, F, Proj>
        constexpr auto operator()(I first, S last, F &&f, Proj &&proj = {}) const -> auto {
            // Call the map inner function.
            auto gen = detail::do_map(std::move(first), std::move(last), std::forward<F>(f), std::forward<Proj>(proj));
            return map_view(std::move(gen));
        }

        template <typename Rng, typename F, typename Proj> requires concepts::can_map_range<Rng, F, Proj>
        constexpr auto operator()(Rng &&rng, F &&f, Proj &&proj = {}) const -> auto {
            // Call the map inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<F>(f), std::forward<Proj>(proj));
        }

        template <typename F, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<F>>)
        constexpr auto operator()(F &&f, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the map.
            return
                [FWD_CAPTURES(f, proj)]<typename Rng> requires concepts::can_map_range<Rng, F, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(f), std::move(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(map);
}
