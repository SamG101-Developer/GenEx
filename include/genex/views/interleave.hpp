#pragma once
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I1, typename S1, typename I2, typename S2>
    auto do_interleave(I1 first1, S1 last1, I2 first2, S2 last2, const bool extend) -> generator<std::common_type_t<iter_value_t<I1>, iter_value_t<I2>>> {
        while (first1 != last1 && first2 != last2) {
            co_yield *first1;
            ++first1;
            co_yield *first2;
            ++first2;
        }

        if (extend) {
            while (first1 != last1) {
                co_yield *first1;
                ++first1;
            }
            while (first2 != last2) {
                co_yield *first2;
                ++first2;
            }
        }
    }
}


namespace genex::views {
    template <typename I1, typename S1, typename I2, typename S2>
    concept can_interleave_iters =
        input_iterator<I1> and
        sentinel_for<S1, I1> and
        input_iterator<I2> and
        sentinel_for<S2, I2> and
        requires { typename std::common_type_t<iter_value_t<I1>, iter_value_t<I2>>; };

    template <typename Rng1, typename Rng2>
    concept can_interleave_range =
        input_range<Rng1> and
        input_range<Rng2> and
        requires { typename std::common_type_t<range_value_t<Rng1>, range_value_t<Rng2>>; };

    DEFINE_VIEW(interleave) {
        template <typename I1, typename S1, typename I2, typename S2> requires can_interleave_iters<I1, S1, I2, S2>
        constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, const bool extend = true) const -> auto {
            // Call the interleave inner function.
            auto gen = detail::do_interleave(std::move(first1), std::move(last1), std::move(first2), std::move(last2), extend);
            return interleave_view(std::move(gen));
        }

        template <typename Rng1, typename Rng2> requires can_interleave_range<Rng1, Rng2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, const bool extend = true) const -> auto {
            // Call the interleave inner function.
            return (*this)(iterators::begin(rng1), iterators::end(rng1), iterators::begin(rng2), iterators::end(rng2), extend);
        }

        template <typename Rng2>
        constexpr auto operator()(Rng2 &&rng2, const bool extend = true) const -> auto {
            // Create a closure that takes a range and applies the interleave.
            return
                [FWD_CAPTURES(rng2, extend)]<typename Rng1> requires can_interleave_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::move(rng2), extend);
            };
        }
    };

    EXPORT_GENEX_VIEW(interleave);
}
