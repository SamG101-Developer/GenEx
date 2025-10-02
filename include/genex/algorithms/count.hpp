#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
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


namespace genex::algorithms {
    struct count_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
            requires concepts::can_count_iters<I, S, E, Proj>
        constexpr auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            auto count = 0uz;
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == elem) { ++count; }
            }
            return count;
        }

        template <typename Rng, typename E, std::invocable<E> Proj = meta::identity>
            requires concepts::can_count_range<Rng, E, Proj>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(count);
}
