#pragma once
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng>
    concept reversible_range =
        bidirectional_range<Rng> and
        std::permutable<iterator_t<Rng>>;
}


namespace genex::actions {
    struct reverse_fn {
        template <typename Rng>
        requires detail::concepts::reversible_range<Rng> and random_access_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            while (first < --last) {
                std::iter_swap(first++, last);
            }
            return std::forward<Rng>(rng);
        }

        template <typename Rng>
        requires detail::concepts::reversible_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            while (first != last and first != --last) {
                std::iter_swap(first++, last);
            }
            return std::forward<Rng>(rng);
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(reverse_fn{});
        }
    };

    inline constexpr reverse_fn reverse{};
}
