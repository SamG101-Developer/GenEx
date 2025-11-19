#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::detail::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    concept equatable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirect_equivalence_relation<Comp, std::projected<I1, Proj1>, std::projected<I2, Proj2>>;

    template <typename Rng1, typename Rng2, typename Comp, typename Proj1, typename Proj2>
    concept equatable_ranges =
        input_range<Rng1> and
        input_range<Rng2> and
        equatable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Comp, Proj1, Proj2>;
}


namespace genex::algorithms::detail::impl {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::equatable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    GENEX_INLINE constexpr auto do_equals(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp, Proj1 &&proj1, Proj2 &&proj2) -> bool {
        for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
            if (not meta::invoke(comp, meta::invoke(proj1, *first1), meta::invoke(proj2, *first2))) { return false; }
        }
        return first1 == last1 and first2 == last2;
    }
}


namespace genex {
    struct equals_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires algorithms::detail::concepts::equatable_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> bool {
            return algorithms::detail::impl::do_equals(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }

        template <typename Rng1, typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires algorithms::detail::concepts::equatable_ranges<Rng1, Rng2, Comp, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp &&comp = {}, Proj1 &&proj1 = {}, Proj2 &&proj2 = {}) const -> bool {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return algorithms::detail::impl::do_equals(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::forward<Comp>(comp), std::forward<Proj1>(proj1), std::forward<Proj2>(proj2));
        }
    };

    inline constexpr equals_fn equals{};
}
