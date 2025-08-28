#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename New>
    concept can_intersperse_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::constructible_from<New, iter_value_t<I>> and
        std::common_with<iter_value_t<I>, New> and
        std::is_copy_constructible_v<New>;

    template <typename Rng, typename New>
    concept can_intersperse_range =
        input_range<Rng> and
        can_intersperse_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;

    template <typename I, typename S, typename F>
    concept can_intersperse_with_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::invocable<F> and
        std::convertible_to<std::invoke_result_t<F>, iter_value_t<I>> and
        std::common_with<iter_value_t<I>, std::invoke_result_t<F>>;

    template <typename Rng, typename F>
    concept can_intersperse_with_range =
        input_range<Rng> and
        can_intersperse_with_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename New> requires concepts::can_intersperse_iters<I, S, New>
    auto do_intersperse(I first, S last, New &&sep) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        co_yield *first;
        ++first;
        for (; first != last; ++first) {
            co_yield sep;
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }

    template <typename I, typename S, std::invocable F> requires concepts::can_intersperse_with_iters<I, S, F>
    auto do_intersperse_with(I first, S last, F &&sep) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        co_yield *first;
        ++first;
        for (; first != last; ++first) {
            co_yield std::invoke(std::forward<F>(sep));
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(intersperse) {
        template <typename I, typename S, typename New> requires concepts::can_intersperse_iters<I, S, New>
        auto operator()(I first, S last, New &&sep) const -> auto {
            auto gen = detail::do_intersperse(std::move(first), std::move(last), std::forward<New>(sep));
            return intersperse_view(std::move(gen));
        }

        template <typename Rng, typename New> requires concepts::can_intersperse_range<Rng, New>
        auto operator()(Rng &&rng, New &&sep) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<New>(sep));
        }

        template <typename New> requires (not input_range<std::remove_cvref_t<New>>)
        auto operator()(New &&sep) const -> auto {
            return
                [FWD_CAPTURES(sep)]<typename Rng> requires concepts::can_intersperse_range<Rng, New>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<New>(sep));
            };
        }
    };

    DEFINE_VIEW(intersperse_with) {
        template <typename I, typename S, typename F> requires concepts::can_intersperse_with_iters<I, S, F>
        auto operator()(I first, S last, F &&sep) const -> auto {
            auto gen = detail::do_intersperse_with(std::move(first), std::move(last), std::forward<F>(sep));
            return intersperse_view(std::move(gen));
        }

        template <typename Rng, typename F> requires concepts::can_intersperse_with_range<Rng, F>
        auto operator()(Rng &&rng, F &&sep) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<F>(sep));
        }

        template <typename F> requires (not input_range<std::remove_cvref_t<F>>)
        auto operator()(F &&sep) const -> auto {
            return
                [FWD_CAPTURES(sep)]<typename Rng> requires concepts::can_intersperse_with_range<Rng, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(sep));
            };
        }
    };

    EXPORT_GENEX_VIEW(intersperse);
    EXPORT_GENEX_VIEW(intersperse_with);
}
