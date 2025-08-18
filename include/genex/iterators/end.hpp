#pragma once
#include <iterator>
#include <genex/macros.hpp>
#include <genex/iterators/_iterator_base.hpp>


namespace genex::iterators {
    template <typename Rng>
    concept has_member_end = requires(Rng &&r) { r.end(); };

    template <typename Rng>
    concept has_member_rend = requires(Rng &&r) { r.rend(); };

    template <typename Rng>
    concept has_std_end = requires(Rng &&r) { std::end(std::forward<Rng>(r)); };

    template <typename Rng>
    concept has_std_rend = requires(Rng &&r) { std::rend(std::forward<Rng>(r)); };

    template <typename Rng>
    concept has_end = has_member_end<Rng> || has_std_end<Rng>;

    template <typename Rng>
    concept has_rend = has_member_rend<Rng> || has_std_rend<Rng>;

    DEFINE_ITERATOR(end) {
        template <typename Rng> requires (has_member_end<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.end()) {
            return r.end();
        }

        template <typename Rng> requires (not has_member_end<Rng> and has_std_end<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::end(std::forward<Rng>(r))) {
            return std::end(std::forward<Rng>(r));
        }
    };

    DEFINE_ITERATOR(rend) {
        template <typename Rng> requires (has_member_rend<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(r.rend()) {
            return r.rend();
        }

        template <typename Rng> requires (not has_member_rend<Rng> and has_std_rend<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> decltype(std::rend(std::forward<Rng>(r))) {
            return std::rend(std::forward<Rng>(r));
        }
    };

    template <typename Rng>
    using end_t = decltype(std::declval<end_fn>()(std::declval<Rng>()));

    template <typename Rng>
    using rend_t = decltype(std::declval<rend_fn>()(std::declval<Rng>()));

    EXPORT_GENEX_ITERATOR(end);
    EXPORT_GENEX_ITERATOR(rend);
}
