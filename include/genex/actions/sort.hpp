#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/meta.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Comp, typename Proj>
    concept can_sorted_range =
        random_access_range<Rng> and
        std::permutable<iterator_t<Rng>> and
        std::sortable<iterator_t<Rng>, Comp, Proj>;
}


namespace genex::actions {
    DEFINE_ACTION(sort) {
        template <typename Rng, typename Comp = operations::lt, typename Proj = meta::identity> requires concepts::can_sorted_range<Rng, Comp, Proj>
        constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> Rng& {
            std::sort(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), [&]<typename Lhs, typename Rhs>(Lhs &&lhs, Rhs &&rhs) {
                return std::invoke(comp, std::invoke(proj, std::forward<Lhs>(lhs)), std::invoke(proj, std::forward<Rhs>(rhs)));
            });
            return rng;
        }

        template <typename Comp = operations::lt, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Comp>>)
        constexpr auto operator()(Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(comp, proj)]<typename Rng> requires concepts::can_sorted_range<Rng, Comp, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Comp>(comp), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_ACTION(sort);
}
