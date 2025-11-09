#pragma once
#include <random>
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Shuffler>
    concept shufflable_range =
        random_access_range<Rng> and
        std::permutable<iterator_t<Rng>> and
        std::uniform_random_bit_generator<std::remove_reference_t<Shuffler>>;
}


namespace genex::actions::detail {
    static thread_local std::mt19937_64 default_random{std::random_device{}()};
}


namespace genex::actions {
    struct shuffle_fn {
        template <typename Rng, typename Shuffler = decltype(detail::default_random)>
        requires detail::concepts::shufflable_range<Rng, Shuffler>
        GENEX_INLINE auto operator()(Rng &&rng, Shuffler shuffler = detail::default_random) const -> decltype(auto) {
            auto [first, last] = iterators::iter_pair(rng);
            std::shuffle(std::move(first), std::move(last), std::move(shuffler));
            return std::forward<Rng>(rng);
        }

        template <typename Shuffler = decltype(detail::default_random)>
        requires std::uniform_random_bit_generator<std::remove_reference_t<Shuffler>>
        GENEX_INLINE auto operator()(Shuffler shuffler = detail::default_random) const -> auto {
            return meta::bind_back(shuffle_fn{}, std::move(shuffler));
        }
    };

    inline constexpr shuffle_fn shuffle{};
}
