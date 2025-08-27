#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S, typename New>
    auto do_intersperse(I first, S last, New &&sep) -> generator<iter_value_t<I>> {
        co_yield *first;
        ++first;
        for (; first != last; ++first) {
            co_yield sep;
            co_yield *first;
        }
    }

    template <typename I, typename S, std::invocable F>
    auto do_intersperse_with(I first, S last, F &&sep) -> generator<iter_value_t<I>> {
        co_yield *first;
        ++first;
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(sep));
            co_yield *first;
        }
    }
}


namespace genex::views {
    template <typename I, typename S, typename New>
    concept can_intersperse_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::convertible_to<New, iter_value_t<I>>;

    template <typename Rng, typename New>
    concept can_intersperse_range =
        input_range<Rng> and
        std::convertible_to<New, range_value_t<Rng>>;

    template <typename I, typename S, typename F>
    concept can_intersperse_with_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::invocable<F> and
        std::convertible_to<std::invoke_result_t<F>, iter_value_t<I>>;

    template <typename Rng, typename F>
    concept can_intersperse_with_range =
        input_range<Rng> and
        std::invocable<F> and
        std::convertible_to<std::invoke_result_t<F>, range_value_t<Rng>>;

    DEFINE_VIEW(intersperse) {
        template <typename I, typename S, typename New> requires can_intersperse_iters<I, S, New>
        constexpr auto operator()(I first, S last, New &&sep) const -> auto {
            auto gen = detail::do_intersperse(std::move(first), std::move(last), std::forward<New>(sep));
            return intersperse_view(std::move(gen));
        }

        template <typename Rng, typename New> requires can_intersperse_range<Rng, New>
        constexpr auto operator()(Rng &&rng, New &&sep) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<New>(sep));
        }

        template <typename New> requires (not input_range<std::remove_cvref_t<New>>)
        constexpr auto operator()(New &&sep) const -> auto {
            return
                [FWD_CAPTURES(sep)]<typename Rng> requires can_intersperse_range<Rng, New>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<New>(sep));
            };
        }
    };

    DEFINE_VIEW(intersperse_with) {
        template <typename I, typename S, typename F> requires can_intersperse_with_iters<I, S, F>
        constexpr auto operator()(I first, S last, F &&sep) const -> auto {
            auto gen = detail::do_intersperse_with(std::move(first), std::move(last), std::forward<F>(sep));
            return intersperse_view(std::move(gen));
        }

        template <typename Rng, typename F> requires can_intersperse_with_range<Rng, F>
        constexpr auto operator()(Rng &&rng, F &&sep) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<F>(sep));
        }

        template <typename F> requires (not input_range<std::remove_cvref_t<F>>)
        constexpr auto operator()(F &&sep) const -> auto {
            return
                [FWD_CAPTURES(sep)]<typename Rng> requires can_intersperse_with_range<Rng, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(sep));
            };
        }
    };

    EXPORT_GENEX_VIEW(intersperse);
    EXPORT_GENEX_VIEW(intersperse_with);
}
