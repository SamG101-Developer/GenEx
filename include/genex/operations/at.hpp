#pragma once
#include <utility>
#include <genex/macros.hpp>
#include <genex/concepts.hpp>
#include <genex/operations/_operations_base.hpp>
#include <genex/operations/size.hpp>

using namespace genex::concepts;


namespace genex::operations {
    template <typename Rng>
    concept has_member_at = requires(Rng &&r) { r.at(std::declval<std::size_t>()); };

    template <typename Rng>
    concept has_member_front = requires(Rng &&r) { r.front(); };

    template <typename Rng>
    concept has_member_back = requires(Rng &&r) { r.back(); };

    struct at_fn final : detail::operations_base {
        template <typename Rng> requires has_member_at<Rng>
        constexpr auto operator()(Rng &&r, const std::size_t n) const noexcept -> range_value_t<Rng>& {
            return r.at(n);
        }
    };

    struct front_fn final : detail::operations_base {
        template <typename Rng> requires has_member_front<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.front();
        }

        template <typename Rng> requires has_member_at<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.at(0);
        }
    };

    struct back_fn final : detail::operations_base {
        template <typename Rng> requires has_member_back<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.back();
        }

        template <typename Rng> requires has_member_at<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.at(size(r) - 1);
        }
    };

    EXPORT_GENEX_STRUCT(at);
    EXPORT_GENEX_STRUCT(front);
    EXPORT_GENEX_STRUCT(back);
}
