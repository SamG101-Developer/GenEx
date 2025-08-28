#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_cycle_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_cycle_range =
        forward_range<Rng> and
        can_cycle_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_cycle_iters<I, S>
    auto do_cycle(I first, S last) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        while (true) {
            for (auto it = first; it != last; ++it) {
                co_yield static_cast<iter_value_t<I>>(*it);
            }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(cycle) {
        template <typename I, typename S> requires concepts::can_cycle_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_cycle(std::move(first), std::move(last));
            return cycle_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_cycle_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_cycle_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(cycle);
}
