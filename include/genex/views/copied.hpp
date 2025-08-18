#pragma once
#include <coroutine>
#include <memory>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S> requires (std::is_copy_constructible_v<iter_value_t<I>>)
    auto do_copied(I &&first, S &&last) -> generator<iter_value_t<I>> {
        for (auto it = first; it != last; ++it) {
            co_yield *it;
        }
    }

    template <range Rng> requires (std::is_copy_constructible_v<range_value_t<Rng>>)
    auto do_copied(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::forward<decltype(x)>(x);
        }
    }

    template <range Rng> requires (unique_ptr<range_value_t<Rng>> && std::is_copy_constructible_v<typename range_value_t<Rng>::element_type>)
    auto do_copied(Rng &&rng) -> generator<range_value_t<Rng>> {
        for (auto &&x : rng) {
            co_yield std::make_unique<typename range_value_t<Rng>::element_type>(*x);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(copied) {
        DEFINE_OUTPUT_TYPE(copied);

        template <iterator I, sentinel S> requires (std::is_copy_constructible_v<iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_copied, first, last);
        }

        template <range Rng> requires (std::is_copy_constructible_v<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_copied, rng);
        }

        template <range Rng> requires (unique_ptr<range_value_t<Rng>> && std::is_copy_constructible_v<typename range_value_t<Rng>::element_type>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_copied, rng);
        }

        constexpr auto operator()() const -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_VIEW(copied);
}
