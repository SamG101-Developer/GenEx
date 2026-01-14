module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.address_of;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept addressable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_readable<I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept addressable_range =
        input_range<Rng> and
        addressable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::addressable_iters<I, S>
    auto do_address_of(I first, S last) -> generator<std::add_pointer_t<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            co_yield std::addressof(*first);
        }
    }
}


namespace genex::views {
    struct address_of_fn {
        template <typename I, typename S>
        requires detail::concepts::addressable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_address_of(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::addressable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_address_of(std::move(first), std::move(last));
        }

        GENEX_INLINE auto operator()() const {
            return meta::bind_back(address_of_fn{});
        }
    };

    export inline constexpr address_of_fn address_of{};
}
