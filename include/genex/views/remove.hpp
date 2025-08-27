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
    template <typename I, typename S, typename E, typename Proj>
    auto do_remove(I first, S last, E &&elem, Proj &&proj) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Proj>(proj), *first) == elem) { continue; }
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Pred, typename Proj>
    auto do_remove_if(I first, S last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            co_yield *first;
        }
    }
}


namespace genex::views {
    template <typename I, typename S, typename E, typename Proj>
    concept can_remove_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<Proj, iter_reference_t<I>> and
        std::equality_comparable_with<std::invoke_result_t<Proj, iter_reference_t<I>>, E>;

    template <typename Rng, typename E, typename Proj>
    concept can_remove_range =
        input_range<Rng> and
        std::invocable<Proj, range_reference_t<Rng>> and
        std::equality_comparable_with<std::invoke_result_t<Proj, range_reference_t<Rng>>, E>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_remove_if_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<Proj, iter_reference_t<I>> and
        std::predicate<Pred, std::invoke_result_t<Proj, iter_reference_t<I>>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_remove_if_range =
        input_range<Rng> and
        std::invocable<Proj, range_reference_t<Rng>> and
        std::predicate<Pred, std::invoke_result_t<Proj, range_reference_t<Rng>>>;

    DEFINE_VIEW(remove) {
        template <typename I, typename S, typename E, typename Proj = meta::identity> requires can_remove_iters<I, S, E, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_remove(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
            return remove_view(std::move(gen));
        }

        template <typename Rng, typename E, typename Proj = meta::identity> requires can_remove_range<Rng, E, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<Proj>(proj));
        }

        template <typename E, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<E>>)
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the remove.
            return
                [FWD_CAPTURES(elem, proj)]<typename Rng> requires can_remove_range<Rng, E, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(elem), std::move(proj));
            };
        }
    };

    DEFINE_VIEW(remove_if) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_remove_if_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_remove_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return remove_if_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires can_remove_if_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the remove.
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_remove_if_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };


    EXPORT_GENEX_VIEW(remove);
    EXPORT_GENEX_VIEW(remove_if);
}
