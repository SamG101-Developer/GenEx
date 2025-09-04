#pragma once
#include <utility>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
    template <typename Rng>
    concept reversible_range =
        bidirectional_range<Rng> and
        std::permutable<iterator_t<Rng>>;

    template <typename Rng>
    concept reversible_range_optimized =
        random_access_range<Rng> and
        reversible_range<Rng>;

    template <typename Rng>
    concept reversible_range_unoptimized =
        input_range<Rng> and
        not random_access_range<Rng> and
        reversible_range<Rng>;
}


namespace genex::actions {
    struct reverse_fn {
        template <typename Rng>
            requires concepts::reversible_range_optimized<Rng>
        constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            while (first < --last) {
                std::iter_swap(first++, last);
            }
            return std::forward<Rng>(rng);
        }

        template <typename Rng>
            requires concepts::reversible_range_unoptimized<Rng>
        constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            while (first != last and first != --last) {
                std::iter_swap(first++, last);
            }
            return std::forward<Rng>(rng);
        }

        constexpr auto operator()() const -> auto {
            return std::bind_back(reverse_fn{});
        }
    };

    EXPORT_GENEX_STRUCT(reverse);
}
