#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept duplicate_checkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_comparable<I, I, Comp, Proj, meta::identity>;

    template <typename Rng, typename Comp, typename Proj>
    concept duplicate_checkable_range =
        forward_range<Rng> and
        duplicate_checkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::views::detail::coros {
    template <typename I, typename S, typename Comp, typename Proj>
    requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
    auto do_duplicates(I first, S last, Comp comp, Proj proj) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;

        auto seen = std::vector<iter_value_t<I>>{};
        auto dupe_element = std::optional<iter_value_t<I>>{};

        for (; first != last; ++first) {
            auto cur = meta::invoke(proj, *first);
            if (dupe_element.has_value()) {
                if (meta::invoke(comp, cur, meta::invoke(proj, *dupe_element))) {
                    co_yield static_cast<iter_value_t<I>>(*first);
                }
                continue;
            }

            for (const auto &s : seen) {
                if (meta::invoke(comp, cur, meta::invoke(proj, s))) {
                    dupe_element = cur;
                    co_yield static_cast<iter_value_t<I>>(*dupe_element);
                    co_yield static_cast<iter_value_t<I>>(*first);
                    break;
                }
            }
            seen.emplace_back(*first);
        }
    }
}


namespace genex::views {
    struct duplicates_fn {
        template <typename I, typename S, typename Comp = operations::eq, typename Proj = meta::identity>
        requires detail::concepts::duplicate_checkable_iters<I, S, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
            return detail::coros::do_duplicates(std::move(first), std::move(last), std::move(comp), std::move(proj));
        }

        template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity>
        requires detail::concepts::duplicate_checkable_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::coros::do_duplicates(std::move(first), std::move(last), std::move(comp), std::move(proj));
        }

        template <typename Comp = operations::eq, typename Proj = meta::identity>
        requires (not range<Comp>)
        GENEX_INLINE constexpr auto operator()(Comp comp = {}, Proj proj = {}) const {
            return std::bind_back(duplicates_fn{}, std::move(comp), std::move(proj));
        }
    };

    inline constexpr duplicates_fn duplicates{};
}
