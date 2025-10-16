#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S>
    concept enumerable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept enumerable_range =
        input_range<Rng> and
        enumerable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S>
    requires concepts::enumerable_iters<I, S>
    auto do_enumerate(I first, S last) -> generator<std::pair<std::size_t, iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        auto i = 0;
        for (; first != last; ++first) {
            co_yield {i++, static_cast<iter_value_t<I>>(*first)};
        }
    }
}


namespace genex::views {
    struct enumerate_fn {
        template <typename I, typename S>
        requires detail::concepts::enumerable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_enumerate(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::enumerable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_enumerate(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(enumerate_fn{});
        }
    };

    inline constexpr enumerate_fn enumerate{};
}
