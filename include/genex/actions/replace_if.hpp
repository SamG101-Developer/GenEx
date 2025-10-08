#pragma once
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/algorithms/find_if.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Pred, typename New, typename Proj>
    concept replaceable_if_range =
        forward_range<Rng> and
        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> and
        std::permutable<iterator_t<Rng>> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>> and
        std::convertible_to<New, range_value_t<Rng>>;
}


namespace genex::actions {
    struct replace_if_fn {
        template <typename Rng, typename Pred, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_if_range<Rng, Pred, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred pred, New new_val, Proj proj = {}) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            for (;first != last; ++first) {
                if (std::invoke(pred, std::invoke(proj, *first))) {
                    *first = new_val;
                }
            }
            return std::forward<Rng>(rng);
        }

        template <typename Pred, typename New, typename Proj = meta::identity>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred pred, New new_val, Proj proj = {}) const {
            return std::bind_back(replace_if_fn{}, std::move(pred), std::move(new_val), std::move(proj));
        }
    };

    inline constexpr replace_if_fn replace_if{};
}
