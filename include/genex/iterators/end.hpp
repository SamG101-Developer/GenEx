#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/_iterator_base.hpp>


namespace genex::iterators {
    template <typename Rng>
    concept can_end_range =
        input_range<Rng>;

    template <typename Rng>
    concept can_rend_range =
        bidirectional_range<Rng>;

    DEFINE_ITERATOR(end) {
        template <typename Rng> requires (can_end_range<Rng> and has_std_end<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::end(std::forward<Rng>(rng)))) -> decltype(std::end(std::forward<Rng>(rng))) {
            return std::end(std::forward<Rng>(rng));
        }

        template <typename Rng> requires (can_end_range<Rng> and not has_std_end<Rng> and has_member_end<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.end())) -> decltype(rng.end()) {
            return rng.end();
        }
    };

    DEFINE_ITERATOR(rend) {
        template <typename Rng> requires (can_rend_range<Rng> and has_std_rend<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::rend(std::forward<Rng>(rng)))) -> decltype(std::rend(std::forward<Rng>(rng))) {
            return std::rend(std::forward<Rng>(rng));
        }

        template <typename Rng> requires (can_rend_range<Rng> and not has_std_rend<Rng> and has_member_rend<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.rend())) -> decltype(rng.rend()) {
            return rng.rend();
        }
    };

    EXPORT_GENEX_ITERATOR(end);
    EXPORT_GENEX_ITERATOR(rend);
}
