#pragma once
#include <functional>
#include <genex/concepts.hpp>
#include <genex/actions/erase.hpp>
#include <genex/iterators/access.hpp>
#include <genex/iterators/next.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        erasable_range<Rng&, iterator_t<Rng>, iterator_t<Rng>> and
        integer_like<Int>;
}


namespace genex::actions {
    struct drop_fn {
        template <typename Rng, typename Int>
            requires concepts::droppable_range<Rng, Int>
        constexpr auto operator()(Rng &&rng, const Int n) const -> decltype(auto) {
            GENEX_ASSERT(std::out_of_range, n > 0);
            actions::erase(
                rng, iterators::begin(rng), iterators::next(iterators::begin(rng), n, iterators::end(rng)));
            return std::forward<Rng>(rng);
        }

        template <typename Int>
            requires integer_like<Int>
        constexpr auto operator()(const Int n) const -> auto {
            GENEX_ASSERT(std::out_of_range, n > 0);
            return std::bind_back(drop_fn{}, n);
        }
    };

    EXPORT_GENEX_STRUCT(drop);
}
