module;
#include <coroutine>
#include <genex/macros.hpp>

export module genex.views.split;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename E>
    concept splittable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, I, std::remove_cvref_t<E> const*>;;

    template <typename Rng, typename E>
    concept splittable_range =
        forward_range<Rng> and
        splittable_iters<iterator_t<Rng>, sentinel_t<Rng>, E>;
}


namespace genex::views::detail::impl {
    template <typename I, typename S, typename E>
    requires concepts::splittable_iters<I, S, E>
    auto do_split(I first, S last, E val) -> generator<std::vector<iter_value_t<I>>> {
        GENEX_ITER_GUARD;
        while (first != last) {
            auto subrange = std::vector<iter_value_t<I>>{};
            for (; first != last and val != *first; ++first) {
                subrange.push_back(*first);
            }
            co_yield std::move(subrange);
            if (first != last) { ++first; }
        }
    }
}


namespace genex::views {
    struct split_fn {
        template <typename I, typename S, typename E>
        requires detail::concepts::splittable_iters<I, S, E>
        GENEX_INLINE constexpr auto operator()(I first, S last, E val) const {
            return detail::impl::do_split(std::move(first), std::move(last), std::move(val));
        }

        template <typename Rng, typename E>
        requires detail::concepts::splittable_range<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E val) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_split(std::move(first), std::move(last), std::move(val));
        }

        template <typename E>
        GENEX_INLINE constexpr auto operator()(E val) const {
            return meta::bind_back(split_fn{}, std::move(val));
        }
    };

    export inline constexpr split_fn split{};
}
