#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename F>
    concept can_for_each_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::invocable<F, iter_value_t<I>>;

    template <typename Rng, typename F>
    concept can_for_each_range =
        input_range<Rng> and
        can_for_each_iters<iterator_t<Rng>, sentinel_t<Rng>, F>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename F> requires concepts::can_for_each_iters<I, S, F>
    auto do_for_each(I first, S last, F &&f) -> void {
        if (first == last) { return; }
        for (; first != last; ++first) {
            std::invoke(std::forward<F>(f), std::forward<decltype(*first)>(*first));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(for_each) {
        template <typename I, typename S, typename F> requires concepts::can_for_each_iters<I, S, F>
        auto operator()(I first, S last, F &&f) const -> void {
            detail::do_for_each(std::move(first), std::move(last), std::forward<F>(f));
        }

        template <typename Rng, typename F> requires concepts::can_for_each_range<Rng, F>
        auto operator()(Rng &&rng, F &&f) const -> void {
            (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<F>(f));
        }

        template <typename F> requires (not input_range<std::remove_cvref_t<F>>)
        auto operator()(F &&f) const -> auto {
            return
                [FWD_CAPTURES(f)]<typename Rng> requires concepts::can_for_each_range<Rng, F>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(f));
            };
        }
    };

    EXPORT_GENEX_VIEW(for_each);
}
