#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_flatten_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        input_range<iter_value_t<I>>;

    template <typename Rng>
    concept can_flatten_range =
        input_range<Rng> and
        can_flatten_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename I, typename S, typename New>
    concept can_flatten_with_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::convertible_to<New, range_value_t<iter_value_t<I>>>;

    template <typename Rng, typename New>
    concept can_flatten_with_range =
        input_range<Rng> and
        can_flatten_with_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_flatten_iters<I, S>
    auto do_flatten(I first, S last) -> generator<range_value_t<iter_value_t<I>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            for (auto &&x : *first) { co_yield static_cast<range_value_t<iter_value_t<I>>>(x); }
        }
    }

    template <typename I, typename S, typename New> requires concepts::can_flatten_with_iters<I, S, New>
    auto do_flatten_with(I first, S last, New &&sep) -> generator<range_value_t<iter_value_t<I>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            for (auto &&x : *first) { co_yield static_cast<range_value_t<iter_value_t<I>>>(x); }
            if (iterators::next(first) != last) { co_yield sep; }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(flatten) {
        template <typename I, typename S> requires concepts::can_flatten_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_flatten(std::move(first), std::move(last));
            return flatten_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_flatten_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_flatten_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(flatten_with) {
        template <typename I, typename S, typename New> requires concepts::can_flatten_with_iters<I, S, New>
        auto operator()(I first, S last, New &&sep) const -> auto {
            auto gen = detail::do_flatten_with(std::move(first), std::move(last), std::forward<New>(sep));
            return flatten_with_view(std::move(gen));
        }

        template <typename Rng, typename New> requires concepts::can_flatten_with_range<Rng, New>
        auto operator()(Rng &&rng, New &&sep) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<New>(sep));
        }

        template <typename New>
        auto operator()(New &&sep) const -> auto {
            return
                [FWD_CAPTURES(sep)]<typename Rng> requires concepts::can_flatten_with_range<Rng, New>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<New>(sep));
            };
        }
    };

    EXPORT_GENEX_VIEW(flatten);
    EXPORT_GENEX_VIEW(flatten_with);
}
