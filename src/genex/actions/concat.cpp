module;
#include <genex/macros.hpp>

export module genex.actions.concat;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.actions.insert;
import genex.iterators.access;
import std;


namespace genex::actions::detail::concepts {
    template <typename... Rngs>
    concept concatenatable_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...) and
        requires { typename std::common_type_t<range_value_t<Rngs>...>; };
}


namespace genex::actions {
    struct concat_fn {
        template <typename Rng1, typename Rng2>
        requires detail::concepts::concatenatable_range<Rng1, Rng2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> decltype(auto) {
            for (auto &&x : rng2) {
                rng1 |= actions::insert(iterators::end(rng1), std::forward<decltype(x)>(x));
            }
            return std::forward<Rng1>(rng1);
        }

        template <typename Rng1, typename Rng2, typename... Rngs>
        requires detail::concepts::concatenatable_range<Rng1, Rng2, Rngs...>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Rngs... rngs) const -> decltype(auto) {
            (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            return (*this)(std::forward<Rngs>(rngs)...);
        }

        template <typename Rng2>
        requires detail::concepts::concatenatable_range<Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
            return meta::bind_back(concat_fn{}, std::forward<Rng2>(rng2));
        }
    };

    export inline constexpr concat_fn concat{};
}
