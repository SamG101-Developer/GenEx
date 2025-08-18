#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions::detail {
    template <range Rng>
    auto do_remove(Rng *rng, range_value_t<Rng> &&elem) -> void {
        rng->erase(std::remove(rng->begin(), rng->end(), std::forward<decltype(elem)>(elem)), rng->end());
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_remove_if(Rng *rng, Pred &&pred, Proj &&proj = {}) -> void {
        auto it = std::remove_if(rng->begin(), rng->end(), [&]<typename E>(E &&elem) {
            return std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<E>(elem)));
        });
        rng->erase(it, rng->end());
    }
}


namespace genex::actions {
    DEFINE_ACTION(remove) {
        template <range Rng>
        auto operator()(Rng &&rng, range_value_t<Rng> &&elem) const -> void {
            FWD_TO_IMPL(detail::do_remove, &rng, elem);
        }

        template <typename E>
        auto operator()(E &&elem) const -> auto {
            MAKE_CLOSURE(elem);
        }
    };

    DEFINE_ACTION(remove_if) {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> void {
            FWD_TO_IMPL(detail::do_remove_if, &rng, pred, proj);
        }

        template <typename E>
        auto operator()(E &&elem) const -> auto {
            MAKE_CLOSURE(elem);
        }
    };

    EXPORT_GENEX_ACTION(remove);
    EXPORT_GENEX_ACTION(remove_if);
};
