module;
#include <genex/macros.hpp>

export module genex.views.in;
export import genex.pipe;
import genex.concepts;
import genex.generator;
import genex.meta;
import genex.algorithms.contains;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    concept inable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirectly_comparable<I1, I2, operations::eq, Proj1, Proj2>;

    template <typename Rng1, typename Rng2, typename Proj1, typename Proj2>
    concept inable_range =
        input_range<Rng1> and
        input_range<Rng2> and
        inable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Proj1, Proj2>;
}


namespace genex::views::detail::impl {
    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::inable_iters<I1, S1, I2, S2, Proj1, Proj2>
    auto do_not_in(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1, Proj2 proj2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (not genex::contains(first2, last2, meta::invoke(std::move(proj1), *first1), std::move(proj2))) {
                co_yield *first1;
            }
        }
    }

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::inable_iters<I1, S1, I2, S2, Proj1, Proj2>
    auto do_in(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1, Proj2 proj2) -> generator<iter_value_t<I1>> {
        for (; first1 != last1; ++first1) {
            if (genex::contains(first2, last2, meta::invoke(std::move(proj1), *first1), proj2)) {
                co_yield *first1;
            }
        }
    }
}


namespace genex::views {
    struct not_in_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::inable_iters<I1, S1, I2, S2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return detail::impl::do_not_in(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(proj1), std::move(proj2));
        }

        template <typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::inable_range<Rng1, Rng2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return detail::impl::do_not_in(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(proj1), std::move(proj2));
        }

        template <typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires input_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return meta::bind_back(*this, std::move(rng2), std::move(proj1), std::move(proj2));
        }
    };

    struct in_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::inable_iters<I1, S1, I2, S2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return detail::impl::do_in(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(proj1), std::move(proj2));
        }

        template <typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::inable_range<Rng1, Rng2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return detail::impl::do_in(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(proj1), std::move(proj2));
        }

        template <typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires input_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return meta::bind_back(*this, std::move(rng2), std::move(proj1), std::move(proj2));
        }
    };

    export inline constexpr not_in_fn not_in{};
    export inline constexpr in_fn in{};
}
