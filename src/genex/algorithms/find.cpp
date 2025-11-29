module;
#include <genex/macros.hpp>

export module genex.algorithms.find;
import genex.concepts;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;


namespace genex::algorithms::detail::concepts {
    export template <typename I, typename S, typename E, typename Proj>
    concept findable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::convertible_to<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Proj>
    concept findable_range =
        input_range<Rng> and
        findable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename Proj>
    requires concepts::findable_iters<I, S, E, Proj>
    GENEX_INLINE constexpr auto do_find(I first, S last, E &&elem, Proj &&proj) -> I {
        for (; first != last; ++first) {
            if (meta::invoke(proj, *first) == elem) { break; }
        }
        return first;
    }
}


namespace genex {
    struct find_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires algorithms::detail::concepts::findable_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> I {
            return algorithms::detail::impl::do_find(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires algorithms::detail::concepts::findable_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E&& elem, Proj &&proj = {}) const -> iterator_t<Rng> {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_find(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };


    export inline constexpr find_fn find{};
}
