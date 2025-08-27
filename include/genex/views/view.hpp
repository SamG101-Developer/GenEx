#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_view(I first, S last) -> generator<iter_value_t<I>> {
        for (auto it = first; it != last; ++it) {
            co_yield *it;
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_view_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_view_range =
        input_range<Rng>;

    DEFINE_VIEW(view) {
        template <typename I, typename S> requires can_view_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_view(std::move(first), std::move(last));
            return view_view(std::move(gen));
        }

        template <typename Rng> requires can_view_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_view_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(view);
}
