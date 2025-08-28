#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>


// todo: optimized views for random_access_iterators (see slice)


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_drop_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename I, typename S>
    concept can_drop_last_iters =
        std::bidirectional_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_drop_while_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename Rng>
    concept can_drop_range =
        input_range<Rng> and
        can_drop_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename Rng>
    concept can_drop_last_range =
        bidirectional_range<Rng> and
        can_drop_last_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_drop_while_range =
        input_range<Rng> and
        can_drop_while_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_drop_iters<I, S>
    auto do_drop(I first, S last, const size_t n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        auto i = 0;
        for (; first != last; ++first) {
            if (i++ < n) { continue; }
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S> requires concepts::can_drop_last_iters<I, S>
    auto do_drop_last(I first, S last, size_t n) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        const auto size = iterators::distance(first, last);
        size_t i = 0;
        for (; first != last; ++first) {
            if (i++ >= size - n) { break; }
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S, typename Pred, typename Proj> requires concepts::can_drop_while_iters<I, S, Pred, Proj>
    auto do_drop_while(I first, S last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        auto dropping = true;
        for (; first != last; ++first) {
            if (dropping && std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            dropping = false;
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S, typename Pred, typename Proj> requires concepts::can_drop_while_iters<I, S, Pred, Proj>
    auto do_drop_until(I first, S last, Pred &&pred, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        auto dropping = true;
        for (; first != last; ++first) {
            if (dropping && !std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { continue; }
            dropping = false;
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(drop) {
        template <typename I, typename S> requires concepts::can_drop_iters<I, S>
        auto operator()(I first, S last, const size_t n) const -> auto {
            // Call the drop inner function.
            auto gen = detail::do_drop(std::move(first), std::move(last), n);
            return drop_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_drop_range<Rng>
        auto operator()(Rng &&rng, const size_t n) const -> auto {
            // Call the drop inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), n);
        }

        auto operator()(size_t n) const -> auto {
            // Create a closure that takes a range and applies the drop.
            return
                [FWD_CAPTURES(n)]<typename Rng> requires concepts::can_drop_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    DEFINE_VIEW(drop_last) {
        template <typename I, typename S> requires concepts::can_drop_last_iters<I, S>
        auto operator()(I first, S last, size_t n) const -> auto {
            // Call the drop inner function.
            auto gen = detail::do_drop_last(std::move(first), std::move(last), n);
            return drop_last_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_drop_last_range<Rng>
        auto operator()(Rng &&rng, size_t n) const -> auto {
            // Call the drop inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), n);
        }

        auto operator()(size_t n) const -> auto {
            // Create a closure that takes a range and applies the drop.
            return
                [FWD_CAPTURES(n)]<typename Rng> requires concepts::can_drop_last_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    DEFINE_VIEW(drop_while) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_drop_while_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the drop inner function.
            auto gen = detail::do_drop_while(std::forward<I>(first), std::forward<S>(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return drop_while_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_drop_while_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the drop inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the drop.
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires concepts::can_drop_while_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };

    DEFINE_VIEW(drop_until) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_drop_while_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the drop inner function.
            auto gen = detail::do_drop_until(std::forward<I>(first), std::forward<S>(last), std::forward<Pred>(pred), std::forward<Proj>(proj));
            return drop_until_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_drop_while_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            // Call the drop inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the drop.
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires concepts::can_drop_while_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(drop);
    EXPORT_GENEX_VIEW(drop_last);
    EXPORT_GENEX_VIEW(drop_while);
    EXPORT_GENEX_VIEW(drop_until);
}
