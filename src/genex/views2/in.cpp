module;
#include <genex/macros.hpp>

export module genex.views2.in;
export import genex.pipe;
import genex.views2.filter;
import genex.concepts;
import genex.meta;
import genex.algorithms.contains;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    concept inable_iters =
        std::input_iterator<I1> and
        std::forward_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirectly_comparable<I1, I2, operations::eq, Proj1, Proj2>;

    template <typename Rng1, typename Rng2, typename Proj1, typename Proj2>
    concept inable_range =
        input_range<Rng1> and
        forward_range<Rng2> and
        inable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Proj1, Proj2>;
}

namespace genex::views::detail::impl {
    template <bool Negate, typename I2, typename S2, typename Proj2>
    struct membership_pred {
        I2 first2;
        S2 last2;
        GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;

        template <typename V>
        GENEX_INLINE constexpr auto operator()(V &&v) const -> bool {
            const auto found = genex::contains(first2, last2, std::forward<V>(v), proj2);
            if constexpr (Negate) { return not found; }
            else { return found; }
        }
    };
}

namespace genex::views {
    template <bool Negate>
    struct in_base_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::inable_iters<I1, S1, I2, S2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            auto pred = detail::impl::membership_pred<Negate, I2, S2, Proj2>{std::move(first2), std::move(last2), std::move(proj2)};
            return genex::views::filter(std::move(first1), std::move(last1), std::move(pred), std::move(proj1));
        }

        template <typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::inable_range<Rng1, Rng2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            auto pred = detail::impl::membership_pred<Negate, iterator_t<Rng2>, sentinel_t<Rng2>, Proj2>{std::move(first2), std::move(last2), std::move(proj2)};
            return genex::views::filter(std::move(first1), std::move(last1), std::move(pred), std::move(proj1));
        }

        template <typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires (forward_range<Rng2> and not range<Proj1>)
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return meta::bind_back(in_base_fn{}, std::forward<Rng2>(rng2), std::move(proj1), std::move(proj2));
        }
    };

    using in_fn = in_base_fn<false>;
    using not_in_fn = in_base_fn<true>;

    export inline constexpr in_fn in{};
    export inline constexpr not_in_fn not_in{};
}
