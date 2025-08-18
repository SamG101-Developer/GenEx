#pragma once
#include <utility>
#include <genex/macros.hpp>
#include <genex/actions/insert.hpp>
#include <genex/operations/size.hpp>


namespace genex::actions {
    template <typename Rng>
    concept has_member_push_back = requires(Rng &&r) { r.push_back(std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_push_front = requires(Rng &&r) { r.push_front(std::declval<range_value_t<Rng>>()); };
}


namespace genex::actions::detail {
    template <typename Rng> requires (has_member_push_back<Rng>)
    auto do_push_back(Rng &&r, range_value_t<Rng> &&elem) -> void {
        r.push_back(std::forward<range_value_t<Rng>>(elem));
    }

    template <typename Rng> requires (has_member_push_front<Rng>)
    auto do_push_front(Rng &&r, range_value_t<Rng> &&elem) -> void {
        r.push_front(std::forward<range_value_t<Rng>>(elem));
    }
}


namespace genex::actions {
    DEFINE_ACTION(push_back) {
        template <typename Rng> requires (has_member_push_back<Rng>)
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> &&elem) const noexcept -> void {
            FWD_TO_IMPL(detail::do_push_back, rng, elem);
        }

        template <typename Rng> requires (not has_member_push_back<Rng> and has_member_insert<Rng>)
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> &&elem) const noexcept -> void {
            FWD_TO_IMPL(detail::do_insert, rng, elem, operations::size(rng));
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const noexcept -> auto {
            MAKE_CLOSURE(elem);
        }
    };

    DEFINE_ACTION(push_front) {
        template <typename Rng> requires (has_member_push_front<Rng>)
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> &&elem) const noexcept -> void {
            FWD_TO_IMPL(detail::do_push_front, rng, elem);
        }

        template <typename Rng> requires (not has_member_push_front<Rng> and has_member_insert<Rng>)
        constexpr auto operator()(Rng &&rng, range_value_t<Rng> &&elem) const noexcept -> void {
            FWD_TO_IMPL(detail::do_insert, rng, elem, 0);
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const noexcept -> auto {
            MAKE_CLOSURE(elem);
        }
    };

    EXPORT_GENEX_ACTION(push_front);
    EXPORT_GENEX_ACTION(push_back);
}
