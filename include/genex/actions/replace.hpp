#pragma once
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::actions::detail::concepts {
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
        requires detail::concepts::replaceable_range<Rng, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Old old_val, New new_val, Proj proj = {}) const -> decltype(auto) {
            // todo: optimize to prevent multi-passes.
            while (true) {
                auto it = algorithms::find(rng, old_val, proj);
                if (it == iterators::end(rng)) { break; }
                *it = new_val;
            }
            return std::forward<Rng>(rng);
        }

        template <typename Old, typename New, typename Proj = meta::identity>
        requires (not range<Old>)
        GENEX_INLINE constexpr auto operator()(Old old_val, New new_val, Proj proj = {}) const {
            return meta::bind_back(replace_fn{}, std::move(old_val), std::move(new_val), std::move(proj));
        }
    };

    inline constexpr replace_fn replace{};
}
