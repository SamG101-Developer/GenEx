#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S> requires unique_ptr<iter_value_t<I>>
    auto do_ptr(I first, S last) -> generator<typename iter_value_t<I>::element_type*> {
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S> requires shared_ptr<iter_value_t<I>>
    auto do_ptr(I first, S last) -> generator<typename iter_value_t<I>::element_type*> {
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S> requires weak_ptr<iter_value_t<I>>
    auto do_ptr(I first, S last) -> generator<typename iter_value_t<I>::element_type*> {
        for (; first != last; ++first) {
            co_yield (*first).lock().get();
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_ptr_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename Rng>
    concept can_ptr_range =
        input_range<Rng> and
        (unique_ptr<range_value_t<Rng>> or shared_ptr<range_value_t<Rng>> or weak_ptr<range_value_t<Rng>>);

    DEFINE_VIEW(ptr) {
        template <typename I, typename S> requires can_ptr_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_ptr(std::move(first), std::move(last));
            return ptr_view(std::move(gen));
        }

        template <typename Rng> requires can_ptr_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_ptr_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(ptr);
}
