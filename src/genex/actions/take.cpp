module;
#include <genex/macros.hpp>

export module genex.actions.take;
export import genex.pipe;
import genex.actions.erase;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.next;
import genex.meta;
import std;


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Int>
    concept takeable_range =
        forward_range<Rng> and
        std::weakly_incrementable<Int>;
}


namespace genex::actions {
    struct take_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::takeable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int n) const -> decltype(auto) {
            GENEX_ASSERT(std::out_of_range, n > 0);
            actions::erase(rng, iterators::next(iterators::begin(rng), n), iterators::end(rng));
            return std::forward<Rng>(rng);
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            GENEX_ASSERT(std::out_of_range, n > 0);
            return meta::bind_back(take_fn{}, n);
        }
    };

    export inline constexpr take_fn take{};
}
