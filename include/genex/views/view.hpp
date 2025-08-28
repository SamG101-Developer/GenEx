#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_view_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_view_range =
        input_range<Rng> and
        can_view_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_view_iters<I, S>
    auto do_view(I first, S last) -> generator<iter_value_t<I>> {
        for (auto it = first; it != last; ++it) {
            co_yield static_cast<iter_value_t<I>>(*it);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(view) {
        template <typename I, typename S> requires concepts::can_view_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_view(std::move(first), std::move(last));
            return view_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_view_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_view_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(view);
}
