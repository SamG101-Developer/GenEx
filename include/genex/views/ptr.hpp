#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept ptr_gettable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename Rng>
    concept ptr_gettable_range =
        input_range<Rng> and
        ptr_gettable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires (concepts::ptr_gettable_iters<I, S> and unique_ptr<iter_value_t<I>>)
    GENEX_NO_ASAN
    auto do_ptr(I first, S last) -> generator<std::add_pointer_t<typename iter_value_t<I>::element_type>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S>
        requires (concepts::ptr_gettable_iters<I, S> and shared_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<std::add_pointer_t<typename iter_value_t<I>::element_type>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S>
        requires (concepts::ptr_gettable_iters<I, S> and weak_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<std::add_pointer_t<typename iter_value_t<I>::element_type>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield (*first).lock().get();
        }
    }
}


namespace genex::views {
    struct ptr_fn {
        template <typename I, typename S>
            requires concepts::ptr_gettable_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_ptr(std::move(first), std::move(last));
        }

        template <typename Rng>
            requires concepts::ptr_gettable_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last));
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(ptr_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(ptr);
}
