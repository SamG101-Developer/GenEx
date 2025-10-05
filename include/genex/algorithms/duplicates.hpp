#pragma once
#include <functional>
#include <vector>

#include <genex/concepts.hpp>
#include <genex/meta.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::details::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept duplicate_checkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_comparable<I, I, Comp, Proj, meta::identity> and
        std::movable<I>;

    template <typename Rng, typename Comp, typename Proj>
    concept duplicate_checkable_range =
        forward_range<Rng> and
        duplicate_checkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::algorithms {
    struct duplicates_fn {
        template <typename I, typename S, typename Comp = operations::eq, typename Proj = meta::identity>
        requires details::concepts::duplicate_checkable_iters<I, S, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Comp comp = {}, Proj proj = {}) const -> std::vector<iter_value_t<I>> {
            auto out = std::vector<iter_value_t<I>>{};
            if (first == last) { return out; }

            auto seen = std::vector<iter_value_t<I>>{};
            auto dupe_element = std::optional<iter_value_t<I>>{};

            for (; first != last; ++first) {
                auto cur = std::invoke(proj, *first);
                if (dupe_element.has_value()) {
                    if (std::invoke(comp, cur, std::invoke(proj, *dupe_element))) {
                        out.emplace_back(static_cast<iter_value_t<I>>(*first));
                    }
                    continue;
                }

                for (const auto &s : seen) {
                    if (std::invoke(comp, cur, std::invoke(proj, s))) {
                        dupe_element = cur;
                        out.emplace_back(static_cast<iter_value_t<I>>(*dupe_element));
                        out.emplace_back(static_cast<iter_value_t<I>>(*first));
                        break;
                    }
                }
                seen.emplace_back(*first);
            }
            return out;
        }

        template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity>
        requires details::concepts::duplicate_checkable_range<Rng, Comp, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Comp comp = {}, Proj proj = {}) const -> std::vector<iter_value_t<iterator_t<Rng>>> {
            auto [begin, end] = iterators::iter_pair(rng);
            return (*this)(begin, end, std::move(comp), std::move(proj));
        }
    };

    inline constexpr duplicates_fn duplicates{};
}