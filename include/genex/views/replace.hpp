#pragma once
#include <coroutine>
#include <genex/categories.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;

// Todo: "replace_with" => callable function


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S>
    auto do_replace(I &&first, S &&last, iter_value_t<I> &&old_val, iter_value_t<I> &&new_val) -> generator<iter_value_t<I>> {
        for (; first != last; ++first) {
            if (*first == old_val) { co_yield std::forward<decltype(new_val)>(new_val); }
            else { co_yield *first; }
        }
    }

    template <range Rng>
    auto do_replace(Rng &&rng, range_value_t<Rng> &&old_val, range_value_t<Rng> &&new_val) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            if (x == old_val) { co_yield std::forward<decltype(new_val)>(new_val); }
            else { co_yield std::forward<decltype(x)>(x); }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(replace) {
        DEFINE_OUTPUT_TYPE(replace);

        template <iterator I, sentinel_for<I> S, typename Old, typename New> requires (
            categories::input_iterator<I> and
            std::equality_comparable_with<iter_value_t<I>, Old> and
            std::convertible_to<New, iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last, Old &&old_val, New &&new_val) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_replace, first, last, old_val, new_val);
        }

        template <range Rng, typename Old, typename New> requires (
            categories::input_range<Rng> and
            std::equality_comparable_with<range_value_t<Rng>, Old> and
            std::convertible_to<range_value_t<Rng>, New>)
        constexpr auto operator()(Rng &&rng, Old &&old_val, New &&new_val) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_replace, rng, old_val, new_val);
        }

        template <typename Old, typename New>
        constexpr auto operator()(Old &&old_val, New &&new_val) const -> auto {
            MAKE_CLOSURE(old_val, new_val);
        }
    };

    EXPORT_GENEX_VIEW(replace);
}
