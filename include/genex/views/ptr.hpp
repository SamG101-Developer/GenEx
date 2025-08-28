#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_ptr_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename Rng>
    concept can_ptr_range =
        input_range<Rng> and
        can_ptr_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires (concepts::can_ptr_iters<I, S> and unique_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<typename iter_value_t<I>::element_type*> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S> requires (concepts::can_ptr_iters<I, S> and shared_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<typename iter_value_t<I>::element_type*> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S> requires (concepts::can_ptr_iters<I, S> and weak_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<typename iter_value_t<I>::element_type*> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield (*first).lock().get();
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(ptr) {
        template <typename I, typename S> requires concepts::can_ptr_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_ptr(std::move(first), std::move(last));
            return ptr_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_ptr_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_ptr_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(ptr);
}
