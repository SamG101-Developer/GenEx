module;
#include <genex/macros.hpp>

export module genex.actions.slice;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.erase;
import genex.iterators.access;
import genex.iterators.next;
import std;


// Todo: random_access_iterator versions with optimizations.


namespace genex::actions::detail::concepts {
    template <typename Rng, typename Int>
    concept sliceable_range =
        forward_range<Rng> and
        std::weakly_incrementable<Int>;
}


namespace genex::actions {
    struct slice_fn {
        template <typename Rng, typename Int>
        requires detail::concepts::sliceable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int from, Int to) const -> decltype(auto) {
            actions::erase(rng, iterators::begin(rng), iterators::next(iterators::begin(rng), from, iterators::end(rng)));
            actions::erase(rng, iterators::next(iterators::begin(rng), to - from), iterators::end(rng));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename Int>
        requires detail::concepts::sliceable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Int from) const -> decltype(auto) {
            actions::erase(rng, iterators::begin(rng), iterators::next(iterators::begin(rng), from, iterators::end(rng)));
            return std::forward<Rng>(rng);
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int from, const Int to) const {
            return meta::bind_back(slice_fn{}, from, to);
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int from) const {
            return meta::bind_back(slice_fn{}, from);
        }
    };

    export inline constexpr slice_fn slice{};
}
