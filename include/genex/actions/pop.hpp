#pragma once

#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/erase.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/size.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions::detail {
    template <typename Rng>
    auto do_pop(Rng *rng, const std::size_t n) -> void {
        iterators::erase(*rng, iterators::next(iterators::begin(*rng), n));
    }


    template <typename Rng>
    auto do_pop_front(Rng *rng) -> void {
        return do_pop(rng, 0);
    }


    template <typename Rng>
    auto do_pop_back(Rng *rng) -> void {
        return do_pop(rng, operations::size(*rng) - 1);
    }
}


namespace genex::actions {
    struct pop_fn final : detail::action_base {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng, const std::size_t n) const -> void {
            MAP_TO_IMPL(detail::do_pop, &rng, n);
        }

        constexpr auto operator()(const std::size_t n) const -> decltype(auto) {
            MAP_TO_BASE(n);
        }
    };

    struct pop_front_fn final : detail::action_base {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const -> void {
            MAP_TO_IMPL(detail::do_pop_front, &rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    struct pop_back_fn final : detail::action_base {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const -> void {
            MAP_TO_IMPL(detail::do_pop_back, &rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(pop);
    EXPORT_GENEX_STRUCT(pop_front);
    EXPORT_GENEX_STRUCT(pop_back);
}
