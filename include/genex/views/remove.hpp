#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept removable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Proj>
    concept removable_range =
        input_range<Rng> and
        removable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S, typename E, typename Proj>
    requires concepts::removable_iters<I, S, E, Proj>
    auto do_remove(I first, S last, E elem, Proj proj) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            if (meta::invoke(proj, *first) == elem) { continue; }
            co_yield static_cast<iter_value_t<I>>(*first);
        }
    }
}


namespace genex::views {
    struct remove_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E elem, Proj proj = {}) const {
            return detail::coros::do_remove(std::move(first), std::move(last), std::move(elem), std::move(proj));
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_remove(std::move(first), std::move(last), std::move(elem), std::move(proj));
        }

        template <typename E, typename Proj = meta::identity>
        requires (not range<E>)
        GENEX_INLINE constexpr auto operator()(E elem, Proj proj = {}) const {
            return std::bind_back(remove_fn{}, std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    inline constexpr remove_fn remove{};
}
