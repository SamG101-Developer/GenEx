#pragma once
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/erase.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        erasable_range<Rng&, iterator_t<Rng>, iterator_t<Rng>> and
        std::weakly_incrementable<Int>;
}


namespace genex::actions {
    struct drop_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::droppable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const -> decltype(auto) {
            GENEX_ASSERT(std::out_of_range, n > 0);
            actions::erase(rng, iterators::begin(rng), iterators::next(iterators::begin(rng), n, iterators::end(rng)));
            return std::forward<Rng>(rng);
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            GENEX_ASSERT(std::out_of_range, n > 0);
            return std::bind_back(drop_fn{}, n);
        }
    };

    inline constexpr drop_fn drop{};
}
