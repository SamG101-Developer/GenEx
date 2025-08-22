#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/iterators/_iterator_base.hpp>


namespace genex::iterators {
    template <typename Rng>
    concept has_member_begin = requires(Rng &&r) { { r.begin() } -> std::input_or_output_iterator; };

    template <typename Rng>
    concept has_member_rbegin = requires(Rng &&r) { { r.rbegin() } -> std::input_or_output_iterator; };

    template <typename Rng>
    concept has_std_begin = requires(Rng &&r) { { std::begin(std::forward<Rng>(r)) } -> std::input_or_output_iterator; };

    template <typename Rng>
    concept has_std_rbegin = requires(Rng &&r) { { std::rbegin(std::forward<Rng>(r)) } -> std::input_or_output_iterator; };

    template <typename Rng>
    concept has_begin = has_member_begin<Rng> || has_std_begin<Rng>;

    template <typename Rng>
    concept has_rbegin = has_member_rbegin<Rng> || has_std_rbegin<Rng>;

    DEFINE_ITERATOR(begin) {
        template <typename Rng> requires (has_member_begin<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.begin()) {
            return r.begin();
        }

        template <typename Rng> requires (not has_member_begin<Rng> and has_std_begin<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::begin(std::forward<Rng>(r))) {
            return std::begin(std::forward<Rng>(r));
        }
    };

    DEFINE_ITERATOR(rbegin) {
        template <typename Rng> requires (has_member_rbegin<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.rbegin()) {
            return r.rbegin();
        }

        template <typename Rng> requires (not has_member_rbegin<Rng> and has_std_rbegin<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::rbegin(std::forward<Rng>(r))) {
            return std::rbegin(std::forward<Rng>(r));
        }
    };

    template <typename Rng>
    using begin_t = decltype(std::declval<begin_fn>()(std::declval<Rng>()));

    template <typename Rng>
    using rbegin_t = decltype(std::declval<rbegin_fn>()(std::declval<Rng>()));

    EXPORT_GENEX_ITERATOR(begin);
    EXPORT_GENEX_ITERATOR(rbegin);
}
