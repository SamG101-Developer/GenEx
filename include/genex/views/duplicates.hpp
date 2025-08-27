#pragma once
#include <complex>
#include <coroutine>
#include <genex/algorithms/count.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/views/_view_base.hpp>

// todo: expand "==" into a custom Comp template parameter


namespace genex::views::detail {
    template <typename I, typename S, typename Proj>
    auto do_duplicates(I first, S last, Proj &&proj = {}) -> generator<iter_value_t<I>> {
        auto orig = first;
        for (; first != last; ++first) {
            auto elem = *first;
            auto count_orig = orig;
            auto count = algorithms::count(std::move(count_orig), std::forward<I>(last), std::forward<decltype(elem)>(elem), std::forward<Proj>(proj));

            if (count > 1) {
                for (; orig != last; ++orig) {
                    if (*orig == elem) { co_yield *orig; }
                }
                break;
            }
        }
    }
}


namespace genex::views {
    template <typename I, typename S, typename Proj>
    concept can_duplicates_iters =
        forward_iterator<I> and
        sentinel_for<S, I> and
        std::indirectly_unary_invocable<Proj, I> and
        std::equality_comparable<std::invoke_result_t<Proj, iter_value_t<I>>>;

    template <typename Rng, typename Proj>
    concept can_duplicates_range =
        forward_range<Rng> and
        std::indirectly_unary_invocable<Proj, iterator_t<Rng>> and
        std::equality_comparable<std::invoke_result_t<Proj, range_value_t<Rng>>>;

    DEFINE_VIEW(duplicates) {
        template <typename I, typename S, typename Proj = meta::identity> requires can_duplicates_iters<I, S, Proj>
        constexpr auto operator()(I first, S last, Proj &&proj = {}) const -> auto {
            auto gen = detail::do_duplicates(std::move(first), std::move(last), std::forward<Proj>(proj));
            return duplicates_view(std::move(gen));
        }

        template <typename Rng, typename Proj = meta::identity> requires can_duplicates_range<Rng, Proj>
        constexpr auto operator()(Rng &&rng, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Proj>(proj));
        }

        template <typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Proj>>)
        constexpr auto operator()(Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(proj)]<typename Rng> requires can_duplicates_range<Rng, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(duplicates);
}
