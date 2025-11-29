module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.ptr;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
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


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires (concepts::ptr_gettable_iters<I, S> and unique_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<std::add_pointer_t<typename iter_value_t<I>::element_type>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S>
    requires (concepts::ptr_gettable_iters<I, S> and shared_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<std::add_pointer_t<typename iter_value_t<I>::element_type>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <typename I, typename S>
    requires (concepts::ptr_gettable_iters<I, S> and weak_ptr<iter_value_t<I>>)
    auto do_ptr(I first, S last) -> generator<std::add_pointer_t<typename iter_value_t<I>::element_type>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield (*first).lock().get();
        }
    }
}


namespace genex::views {
    struct ptr_fn {
        template <typename I, typename S>
        requires detail::concepts::ptr_gettable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_ptr(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::ptr_gettable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_ptr(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(ptr_fn{});
        }
    };

    export inline constexpr ptr_fn ptr{};
}
