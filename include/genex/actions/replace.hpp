#pragma once
#include <utility>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Old, typename New, typename Proj>
    concept replaceable_range =
        forward_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> and
        std::permutable<iterator_t<Rng>> and
        std::indirect_equivalence_relation<operations::eq, std::projected<iterator_t<Rng>, Proj>, std::remove_cvref_t<Old> const*> and
        std::convertible_to<New, range_value_t<Rng>>;
}


namespace genex::actions {
    struct replace_fn {
        template <typename Rng, typename Old, typename New, typename Proj = meta::identity>
            requires concepts::replaceable_range<Rng, Old, New, Proj>
        constexpr auto operator()(Rng &&rng, Old &&old_val, New &&new_val, Proj &&proj = {}) const -> decltype(auto) {
            // todo: optimize to prevent multi-passes.
            while (true) {
                auto it = algorithms::find(rng, std::forward<Old>(old_val), std::forward<Proj>(proj));
                if (it == iterators::end(rng)) { break; }
                *it = new_val;
            }
            return std::forward<Rng>(rng);
        }

        template <typename Old, typename New, typename Proj = meta::identity>
            requires(not range<Old>)
        constexpr auto operator()(Old &&old_val, New &&new_val, Proj &&proj = {}) const -> auto {
            return std::bind_back(
                replace_fn{}, std::forward<Old>(old_val), std::forward<New>(new_val), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(replace);
}
