#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Comp, typename Proj>
    concept can_sorted_range =
        random_access_range<Rng> and
        std::sortable<iterator_t<Rng>, Comp, Proj>;
}


namespace genex::actions {
    struct sort_fn {
        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity>
            requires concepts::can_sorted_range<Rng, Comp, Proj>
        constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            auto sorter = [&]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
                return std::invoke(
                    comp, std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
            };
            std::sort(std::move(first), std::move(last), std::move(sorter));
            return std::forward<Rng>(rng);
        }

        template <typename Comp = operations::lt, typename Proj = meta::identity>
            requires (not input_range<std::remove_cvref_t<Comp>>)
        constexpr auto operator()(Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return std::bind_back(sort_fn{}, std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(sort);
}
