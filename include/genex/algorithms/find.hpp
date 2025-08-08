#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, typename E, std::invocable<E> Proj = genex::meta::identity> requires std::equality_comparable_with<iter_value_t<I>, E>
auto do_find(I &&first, S &&last, E &&elem, Proj &&proj = {}) -> I {
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Proj>(proj), *first) == elem) { return first; }
    }
    return last;
}


template <range Rng, typename E, std::invocable<E> Proj = genex::meta::identity> requires std::equality_comparable_with<range_value_t<Rng>, E>
auto do_find(Rng &&rng, E &&elem, Proj &&proj = {}) -> decltype(auto) {
    return do_find(genex::iterators::begin(rng), genex::iterators::end(rng), std::forward<E>(elem), std::forward<Proj>(proj));
}


template <typename I, typename S, typename E, std::invocable<E> Proj = genex::meta::identity> requires std::equality_comparable_with<iter_value_t<I>, E>
auto do_find_last(I &&first, S &&last, E &&elem, Proj &&proj = {}) -> I {
    auto found_last = last;
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Proj>(proj), *first) == elem) { found_last = first; }
    }
    return found_last;
}


template <range Rng, typename E, std::invocable<E> Proj = genex::meta::identity> requires std::equality_comparable_with<range_value_t<Rng>, E>
auto do_find_last(Rng &&rng, E &&elem, Proj &&proj = {}) -> decltype(auto) {
    return do_find_last(genex::iterators::begin(rng), genex::iterators::end(rng), std::forward<E>(elem), std::forward<Proj>(proj));
}


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_find_if(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> I {
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { return first; }
    }
    return last;
}


template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_find_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> decltype(auto) {
    return do_find_if(genex::iterators::begin(rng), genex::iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
}


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_find_last_if(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> I {
    auto found_last = last;
    for (; first != last; ++first) {
        if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { found_last = first; }
    }
    return found_last;
}


template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_find_last_if(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> decltype(auto) {
    return do_find_last_if(genex::iterators::begin(rng), genex::iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
}


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_find_if_not(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> I {
    for (; first != last; ++first) {
        if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { return first; }
    }
    return last;
}


template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_find_if_not(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> decltype(auto) {
    return do_find_if_not(genex::iterators::begin(rng), genex::iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
}


template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
auto do_find_last_if_not(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) -> I {
    auto found_last = last;
    for (; first != last; ++first) {
        if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { found_last = first; }
    }
    return found_last;
}


template <range Rng, std::invocable<range_value_t<Rng>> Proj = genex::meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
auto do_find_last_if_not(Rng &&rng, Pred &&pred, Proj &&proj = {}) -> decltype(auto) {
    return do_find_last_if_not(genex::iterators::begin(rng), genex::iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
}


namespace genex::algorithms {
    struct find_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity> requires std::equality_comparable_with<iter_value_t<I>, E>
        constexpr auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> I {
            MAP_TO_IMPL(do_find, first, last, elem, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> const &elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_find, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, proj);
        }
    };

    struct find_last_fn final : detail::algorithm_base {
        template <typename I, typename S, typename E, std::invocable<E> Proj = meta::identity> requires std::equality_comparable_with<iter_value_t<I>, E>
        constexpr auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> I {
            MAP_TO_IMPL(do_find_last, first, last, elem, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> const &elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_find_last, rng, elem, proj);
        }

        template <typename E, std::invocable<E> Proj = meta::identity>
        constexpr auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(elem, proj);
        }
    };

    struct find_if_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            MAP_TO_IMPL(do_find_if, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_find_if, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };

    struct find_last_if_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            MAP_TO_IMPL(do_find_last_if, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_find_last_if, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };


    struct find_if_not_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            MAP_TO_IMPL(do_find_if_not, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_find_if_not, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };

    struct find_last_if_not_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            MAP_TO_IMPL(do_find_last_if_not, first, last, pred, proj);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_IMPL(do_find_last_if_not, rng, pred, proj);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj);
        }
    };

    EXPORT_GENEX_STRUCT(find);
    EXPORT_GENEX_STRUCT(find_last);
    EXPORT_GENEX_STRUCT(find_if);
    EXPORT_GENEX_STRUCT(find_last_if);
    EXPORT_GENEX_STRUCT(find_if_not);
    EXPORT_GENEX_STRUCT(find_last_if_not);
}
