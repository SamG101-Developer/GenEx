#pragma once
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
    auto do_pop(Rng &&rng, const std::size_t n) -> void {
        iterators::erase(std::forward<Rng>(rng), iterators::next(iterators::begin(std::forward<Rng>(rng)), n));
    }

    template <typename Rng>
    auto do_pop_front(Rng &&rng) -> void {
        return do_pop(std::forward<Rng>(rng), 0);
    }

    template <typename Rng>
    auto do_pop_back(Rng &&rng) -> void {
        return do_pop(std::forward<Rng>(rng), operations::size(rng) - 1);
    }
}


namespace genex::actions {
    DEFINE_ACTION(pop) {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng, const std::size_t n) const -> void {
            FWD_TO_IMPL(detail::do_pop, rng, n);
        }

        constexpr auto operator()(const std::size_t n) const -> auto {
            MAKE_CLOSURE(n);
        }
    };

    DEFINE_ACTION(pop_front) {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const -> void {
            FWD_TO_IMPL(detail::do_pop_front, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    DEFINE_ACTION(pop_back) {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const -> void {
            FWD_TO_IMPL(detail::do_pop_back, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_ACTION(pop);
    EXPORT_GENEX_ACTION(pop_front);
    EXPORT_GENEX_ACTION(pop_back);
}
