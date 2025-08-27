#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_take(I first, S last, const size_t n) -> generator<iter_value_t<I>> {
        auto i = 0;
        for (; first != last; ++first) {
            if (i++ >= n) { break; }
            co_yield *first;
        }
    }

    template <typename I, typename S>
    auto do_take_last(I first, S last, size_t n) -> generator<iter_value_t<I>> {
        auto size = iterators::distance(first, last);
        auto i = 0;
        for (; first != last; ++first) {
            if (i++ < size - n) { continue; }
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Pred, typename Proj>
    auto do_take_while(I first, S last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { break; }
            co_yield *first;
        }
    }

    template <typename I, typename S, typename Pred, typename Proj>
    auto do_take_until(I first, S last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { break; }
            co_yield *first;
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_take_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_take_range =
        input_range<Rng>;

    template <typename I, typename S>
    concept can_take_last_iters =
        bidirectional_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_take_last_range =
        bidirectional_range<Rng>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_take_while_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<Proj, iter_value_t<I>> and
        std::predicate<Pred, std::invoke_result_t<Proj, iter_value_t<I>>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_take_while_range =
        input_range<Rng> and
        std::invocable<Proj, range_value_t<Rng>> and
        std::predicate<Pred, std::invoke_result_t<Proj, range_value_t<Rng>>>;

    DEFINE_VIEW(take) {
        template <typename I, typename S> requires can_take_iters<I, S>
        constexpr auto operator()(I first, S last, const size_t n) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_take(std::move(first), std::move(last), n);
            return take_view(std::move(gen));
        }

        template <typename Rng> requires can_take_range<Rng>
        constexpr auto operator()(Rng &&rng, const size_t n) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), n);
        }

        constexpr auto operator()(std::size_t n) const -> auto {
            // Create a closure that takes a range and applies the take.
            return
                [FWD_CAPTURES(n)]<typename Rng> requires can_take_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    DEFINE_VIEW(take_last) {
        template <typename I, typename S> requires can_take_last_iters<I, S>
        constexpr auto operator()(I first, S last, size_t n) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_take_last(std::move(first), std::move(last), n);
            return take_last_view(std::move(gen));
        }

        template <typename Rng> requires can_take_last_range<Rng>
        constexpr auto operator()(Rng &&rng, size_t n) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), n);
        }

        constexpr auto operator()(size_t n) const -> auto {
            // Create a closure that takes a range and applies the take last.
            return
                [FWD_CAPTURES(n)]<typename Rng> requires can_take_last_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    DEFINE_VIEW(take_while) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_take_while_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_take_while(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return take_while_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires can_take_while_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the take last.
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_take_while_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };

    DEFINE_VIEW(take_until) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires can_take_while_iters<I, S, Pred, Proj>
        constexpr auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            auto gen = detail::do_take_until(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return take_until_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires can_take_while_range<Rng, Pred, Proj>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the move inner function.
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the take last.
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires can_take_while_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(take);
    EXPORT_GENEX_VIEW(take_last);
    EXPORT_GENEX_VIEW(take_while);
    EXPORT_GENEX_VIEW(take_until);
}
