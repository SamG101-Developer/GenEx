#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms::detail {
    template <iterator I, sentinel S, typename E, std::invocable<E> Proj = genex::meta::identity>
    auto do_count(I &&first, S &&last, E &&elem, Proj &&proj = {}) -> size_t {
        auto count = static_cast<std::size_t>(0);
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Proj>(proj), *first) == elem) { ++count; }
        }
        return count;
    }

    template <range Rng, typename E, std::invocable<E> Proj = genex::meta::identity>
    auto do_count(Rng &&rng, E &&elem, Proj &&proj = {}) -> size_t {
        auto count = static_cast<std::size_t>(0);
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)) == elem) { ++count; }
        }
        return count;
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_count_if(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> size_t {
        auto count = static_cast<std::size_t>(0);
        for (; first != last; ++first) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { ++count; }
        }
        return count;
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_count_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> size_t {
        auto count = static_cast<std::size_t>(0);
        for (auto &&x : rng) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { ++count; }
        }
        return count;
    }
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(count) {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> size_t {
            FWD_TO_IMPL(detail::do_count, first, last, elem, proj);
        }

        template <range Rng, typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_count, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(elem, proj);
        }
    };

    DEFINE_ALGORITHM(count_if) {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> size_t {
            FWD_TO_IMPL(detail::do_count_if, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            FWD_TO_IMPL(detail::do_count_if, rng, pred, proj);
        }

        template <typename Proj = meta::identity>
        constexpr auto operator()(Proj &&proj = {}) const -> auto {
            MAKE_CLOSURE(proj);
        }
    };

    EXPORT_GENEX_ALGORITHM(count);
    EXPORT_GENEX_ALGORITHM(count_if);
}
