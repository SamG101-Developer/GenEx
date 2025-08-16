#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms {
    using pos_t = std::make_signed_t<std::size_t>;
}


namespace genex::algorithms::detail {
    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_position(I &&first, S &&last, Pred &&pred, Proj &&proj = {}, const pos_t def = -1) -> pos_t {
        pos_t index = 0;
        for (; first != last; ++first, ++index) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) {
                return index;
            }
        }
        return def;
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_position(Rng &&rng, Pred &&pred, Proj &&proj = {}, const pos_t def = -1) -> pos_t {
        return do_position(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
    }

    template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
    auto do_position_last(I &&first, S &&last, Pred &&pred, Proj &&proj = {}, const pos_t def = -1) -> pos_t {
        pos_t index = iterators::distance(first, last) - 1;
        last = iterators::next(first, index);
        for (; last != first; --last, --index) {
            if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *last))) {
                return index;
            }
        }
        return def;
    }

    template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
    auto do_position_last(Rng &&rng, Pred &&pred, Proj &&proj = {}, const pos_t def = -1) -> pos_t {
        return do_position_last(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj), def);
    }
}


namespace genex::algorithms {
    struct position_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}, pos_t def = -1) const -> std::size_t {
            MAP_TO_IMPL(detail::do_position, first, last, pred, proj, def);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, pos_t def = -1) const -> std::size_t {
            MAP_TO_IMPL(detail::do_position, rng, pred, proj, def);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}, pos_t def = -1) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj, def);
        }
    };

    struct position_last_fn final : detail::algorithm_base {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        constexpr auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}, pos_t def = -1) const -> std::size_t {
            MAP_TO_IMPL(detail::do_position_last, first, last, pred, proj, def);
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        constexpr auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}, pos_t def = -1) const -> std::size_t {
            MAP_TO_IMPL(detail::do_position_last, rng, pred, proj, def);
        }

        template <typename Pred, typename Proj = meta::identity>
        constexpr auto operator()(Pred &&pred, Proj &&proj = {}, pos_t def = -1) const -> decltype(auto) {
            MAP_TO_BASE(pred, proj, def);
        }
    };

    EXPORT_GENEX_STRUCT(position);
    EXPORT_GENEX_STRUCT(position_last);
}
