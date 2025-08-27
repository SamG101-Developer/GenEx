#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S, typename Proj, typename Pred>
    auto do_filter(I first, S last, Pred &&pred, Proj &&proj) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                co_yield *first;
            }
        }
    }
}


namespace genex::views {
    template <typename I, typename S, typename Pred, typename Proj>
    concept can_filter_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_filter_range =
        input_range<Rng> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>>;

    DEFINE_VIEW(filter) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_filter_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the filter inner function.
            auto gen = detail::do_filter(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return filter_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires can_filter_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the filter inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the filter.
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_filter_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(filter);
}
