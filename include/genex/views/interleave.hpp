#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename T1, typename T2>
    using interleave_common_t = std::common_type_t<
        std::remove_cvref_t<iter_value_t<T1>>,
        std::remove_cvref_t<iter_value_t<T2>>>;

    template <typename I1, typename S1, typename I2, typename S2>
    concept interleavable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::convertible_to<iter_value_t<I1>, interleave_common_t<I1, I2>> and
        std::convertible_to<iter_value_t<I2>, interleave_common_t<I1, I2>>;

    template <typename Rng1, typename Rng2>
    concept interleavable_range =
        input_range<Rng1> and
        input_range<Rng2> and
        interleavable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>>;
}


namespace genex::views::detail::impl {
    template <typename I1, typename S1, typename I2, typename S2>
    requires concepts::interleavable_iters<I1, S1, I2, S2>
    auto do_interleave(I1 first1, S1 last1, I2 first2, S2 last2, const bool extend) -> generator<concepts::interleave_common_t<I1, I2>> {
        while (first1 != last1 and first2 != last2) {
            co_yield static_cast<iter_value_t<I1>>(*first1);
            ++first1;
            co_yield static_cast<iter_value_t<I2>>(*first2);
            ++first2;
        }

        if (extend) {
            while (first1 != last1) {
                co_yield static_cast<iter_value_t<I1>>(*first1);
                ++first1;
            }
            while (first2 != last2) {
                co_yield static_cast<iter_value_t<I2>>(*first2);
                ++first2;
            }
        }
    }
}


namespace genex::views {
    struct interleave_fn {
        template <typename I1, typename S1, typename I2, typename S2>
        requires detail::concepts::interleavable_iters<I1, S1, I2, S2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, const bool extend = true) const {
            return detail::impl::do_interleave(std::move(first1), std::move(last1), std::move(first2), std::move(last2), extend);
        }

        template <typename Rng1, typename Rng2>
        requires detail::concepts::interleavable_range<Rng1, Rng2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, const bool extend = true) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return detail::impl::do_interleave(std::move(first1), std::move(last1), std::move(first2), std::move(last2), extend);
        }

        template <typename Rng2>
        requires range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, const bool extend = true) const {
            return meta::bind_back(interleave_fn{}, std::forward<Rng2>(rng2), extend);
        }
    };

    inline constexpr interleave_fn interleave{};
}
