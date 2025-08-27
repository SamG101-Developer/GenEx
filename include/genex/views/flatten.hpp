#pragma once
#include <coroutine>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_flatten(I first, S last) -> generator<range_value_t<iter_value_t<I>>> {
        for (; first != last; ++first) {
            for (auto &&x : *first) {
                co_yield x;
            }
        }
    }

    template <typename I, typename S, typename New>
    auto do_flatten_with(I first, S last, New &&sep) -> generator<range_value_t<iter_value_t<I>>> {
        for (; first != last; ++first) {
            for (auto inner_first = iterators::begin(*first); inner_first != iterators::end(*first); ++inner_first) {
                co_yield *inner_first;
            }
            if (iterators::next(first) != last) { co_yield sep; }
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_flatten_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        input_range<iter_value_t<I>>;

    template <typename Rng>
    concept can_flatten_range =
        input_range<Rng> and
        input_range<range_value_t<Rng>>;

    template <typename I, typename S, typename New>
    concept can_flatten_with_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::convertible_to<New, range_value_t<iter_value_t<I>>>;

    template <typename Rng, typename New>
    concept can_flatten_with_range =
        input_range<Rng> and
        input_range<range_value_t<Rng>> and
        std::convertible_to<New, range_value_t<range_value_t<Rng>>>;

    DEFINE_VIEW(flatten) {
        template <typename I, typename S> requires can_flatten_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_flatten(std::move(first), std::move(last));
            return flatten_view(std::move(gen));
        }

        template <typename Rng> requires can_flatten_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_flatten_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(flatten_with) {
        template <typename I, typename S, typename New> requires can_flatten_with_iters<I, S, New>
        constexpr auto operator()(I first, S last, New &&sep) const -> auto {
            auto gen = detail::do_flatten_with(std::move(first), std::move(last), std::forward<New>(sep));
            return flatten_with_view(std::move(gen));
        }

        template <typename Rng, typename New> requires can_flatten_with_range<Rng, New>
        constexpr auto operator()(Rng &&rng, New &&sep) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<New>(sep));
        }

        template <typename New> requires (not input_range<std::remove_cvref_t<New>>)
        constexpr auto operator()(New &&sep) const -> auto {
            return
                [FWD_CAPTURES(sep)]<typename Rng> requires can_flatten_with_range<Rng, New>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<New>(sep));
            };
        }
    };

    EXPORT_GENEX_VIEW(flatten);
    EXPORT_GENEX_VIEW(flatten_with);
}
