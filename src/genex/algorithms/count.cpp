module;
#include <genex/macros.hpp>

export module genex.algorithms.count;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::algorithms::detail::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept can_count_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Proj>
    concept can_count_range =
        input_range<Rng> and
        can_count_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::algorithms::detail::impl {
    template <typename I, typename S, typename E, typename Proj>
    requires concepts::can_count_iters<I, S, E, Proj>
    GENEX_INLINE constexpr auto do_count(I first, S last, E&& elem, Proj &&proj) -> std::size_t {
        auto count = 0uz;
        for (; first != last; ++first) {
            if (meta::invoke(proj, *first) == elem) { ++count; }
        }
        return count;
    }
}


namespace genex {
    struct count_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires algorithms::detail::concepts::can_count_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> std::size_t {
            return algorithms::detail::impl::do_count(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }

        template <typename Rng, typename E, std::invocable<E> Proj = meta::identity>
        requires algorithms::detail::concepts::can_count_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> std::size_t {
            auto [first, last] = iterators::iter_pair(rng);
            return algorithms::detail::impl::do_count(std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    export inline constexpr count_fn count{};
}
