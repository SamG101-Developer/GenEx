#pragma once
#include <utility>
#include <genex/actions/_action_base.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/concepts.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/meta.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename E, typename Proj>
    concept can_action_remove_range =
        forward_range<Rng> and
        std::permutable<iterator_t<Rng>> and
        std::invocable<Proj, range_reference_t<Rng>> and
        std::indirectly_comparable<iterator_t<Rng>, E*, operations::eq, Proj, meta::identity> and
        has_member_erase<Rng>;

    template <typename Rng, typename Pred, typename Proj>
    concept can_action_remove_if_range =
        forward_range<Rng> and
        std::permutable<iterator_t<Rng>> and
        std::indirect_unary_predicate<Pred, std::projected<iterator_t<Rng>, Proj>> and
        has_member_erase<Rng>;
}


namespace genex::actions {
    DEFINE_ACTION(remove) {
        template <typename Rng, typename E, typename Proj = meta::identity> requires concepts::can_action_remove_range<Rng, E, Proj>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> Rng& {
            auto first = iterators::begin(std::forward<Rng>(rng));
            auto last = iterators::end(std::forward<Rng>(rng));

            while (first != last) {
                auto it = algorithms::find(first, last, std::forward<E>(elem), std::forward<Proj>(proj));
                if (it == last) { break; }
                first = rng.erase(std::move(it));
            }

            return rng;
        }

        template <typename E> requires (not input_range<std::remove_cvref_t<E>>)
        auto operator()(E &&elem) const -> auto {
            return
                [FWD_CAPTURES(elem)]<typename Rng> requires concepts::can_action_remove_range<Rng, E, meta::identity>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(elem));
            };
        }
    };

    DEFINE_ACTION(remove_if) {
        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_action_remove_if_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> Rng& {
            auto first = iterators::begin(std::forward<Rng>(rng));
            auto last = iterators::end(std::forward<Rng>(rng));

            while (first != last) {
                auto it = algorithms::find_if(first, last, std::forward<Pred>(pred), std::forward<Proj>(proj));
                if (it == last) { break; }
                first = rng.erase(std::move(it));
            }

            return rng;
        }

        template <typename Pred, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(pred, proj)]<typename Rng> requires concepts::can_action_remove_if_range<Rng, Pred, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(proj));
            };
        }
    };

    EXPORT_GENEX_ACTION(remove);
    EXPORT_GENEX_ACTION(remove_if);
};
