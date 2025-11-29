module;
#include <genex/macros.hpp>

export module genex.algorithms.binary_search;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename E, typename Comp, typename Proj>
    concept binary_searchable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_strict_weak_order<Comp, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Comp, typename Proj>
    concept binary_searchable_range =
        forward_range<Rng> and
        binary_searchable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Comp, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename Comp, typename Proj>
    requires concepts::binary_searchable_iters<I, S, E, Comp, Proj>
    GENEX_INLINE constexpr auto do_binary_search(I first, S last, E&& elem, Comp &&comp, Proj &&proj) -> bool {
        first = std::lower_bound(std::move(first), std::move(last), elem, [&]<typename T, typename U>(T &&a, U &&b) {
            return meta::invoke(comp, meta::invoke(proj, std::forward<T>(a)), std::forward<U>(b));
        });
        return first != last and not meta::invoke(comp, meta::invoke(proj, *first), std::forward<E>(elem));
    }
}



namespace genex {
    struct binary_search_fn {
        template <typename I, typename S, typename E, typename Comp = operations::lt, typename Proj = meta::identity>
        requires algorithms::detail::concepts::binary_searchable_iters<I, S, E, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E&& elem, Comp &&comp = {}, Proj &&proj = {}) const -> bool {
            return algorithms::detail::impl::do_binary_search(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Rng, typename E, typename Comp = operations::lt, typename Proj = meta::identity>
        requires algorithms::detail::concepts::binary_searchable_range<Rng, E, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E&& elem, Comp &&comp = {}, Proj &&proj = {}) const -> bool {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_binary_search(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    export inline constexpr binary_search_fn binary_search{};
}
