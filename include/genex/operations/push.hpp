#pragma once
#include <utility>
#include <genex/macros.hpp>
#include <genex/operations/insert.hpp>
#include <genex/operations/_operations_base.hpp>
#include <genex/operations/size.hpp>


namespace genex::operations {
    template <typename Rng>
    concept has_member_push_back = requires(Rng &&r) { r.push_back(std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_push_front = requires(Rng &&r) { r.push_front(std::declval<range_value_t<Rng>>()); };

    struct push_back_fn final : detail::operations_base {
        template <typename Rng> requires (has_member_push_back<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> &&elem) const noexcept -> void {
            r.push_back(std::forward<range_value_t<Rng>>(elem));
        }

        template <typename Rng> requires (not has_member_push_back<Rng> and has_member_insert<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> &&elem) const noexcept -> void {
            r.insert(std::forward<range_value_t<Rng>>(elem), operations::size(r));
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const noexcept -> decltype(auto) {
            MAP_TO_BASE(elem);
        }
    };

    struct push_front_fn final : detail::operations_base {
        template <typename Rng> requires (has_member_push_front<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> &&elem) const noexcept -> void {
            r.push_front(std::forward<range_value_t<Rng>>(elem));
        }

        template <typename Rng> requires (not has_member_push_front<Rng> and has_member_insert<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> &&elem) const noexcept -> void {
            r.insert(std::forward<range_value_t<Rng>>(elem), 0);
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const noexcept -> decltype(auto) {
            MAP_TO_BASE(elem);
        }
    };

    EXPORT_GENEX_STRUCT(push_front);
    EXPORT_GENEX_STRUCT(push_back);
}
