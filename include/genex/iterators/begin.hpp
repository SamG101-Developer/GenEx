#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/_iterator_base.hpp>


namespace genex::iterators::concepts {
    template <typename Rng>
    concept can_begin_range =
        input_range<Rng>;

    template <typename Rng>
    concept can_rbegin_range =
        bidirectional_range<Rng>;
}


namespace genex::iterators {
    DEFINE_ITERATOR(begin) {
        template <typename Rng> requires (concepts::can_begin_range<Rng> and has_std_begin<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::begin(std::forward<Rng>(rng)))) -> decltype(std::begin(std::forward<Rng>(rng))) {
            return std::begin(std::forward<Rng>(rng));
        }

        template <typename Rng> requires (concepts::can_begin_range<Rng> and not has_std_begin<Rng> and has_member_begin<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.begin())) -> decltype(rng.begin()) {
            return rng.begin();
        }
    };

    DEFINE_ITERATOR(rbegin) {
        template <typename Rng> requires (concepts::can_rbegin_range<Rng> and has_std_rbegin<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::rbegin(std::forward<Rng>(rng)))) -> decltype(std::rbegin(std::forward<Rng>(rng))) {
            return std::rbegin(std::forward<Rng>(rng));
        }

        template <typename Rng> requires (concepts::can_rbegin_range<Rng> and not has_std_rbegin<Rng> and has_member_rbegin<Rng>)
        constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.end())) -> decltype(rng.rbegin()) {
            return rng.rbegin();
        }
    };

    EXPORT_GENEX_ITERATOR(begin);
    EXPORT_GENEX_ITERATOR(rbegin);
}
