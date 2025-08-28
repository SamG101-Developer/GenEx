#pragma once
#include <genex/concepts.hpp>
#include <genex/actions/push.hpp>


namespace genex::actions::concepts {
    template <typename... Rngs>
    concept can_concat_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...) and
        requires { typename std::common_type_t<range_value_t<Rngs>...>; };
}


namespace genex::actions {
    DEFINE_ACTION(concat) {
        template <typename Rng1, typename Rng2> requires concepts::can_concat_range<Rng1, Rng2>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> Rng1& {
            for (auto &&x : rng2) {
                rng1 |= push(iterators::end(std::forward<Rng1>(rng1)), std::forward<decltype(x)>(x));
            }
            return rng1;
        }

        template <typename Rng1, typename Rng2, typename... Rngs> requires concepts::can_concat_range<Rng1, Rng2, Rngs...>
        auto operator()(Rng1 &&rng1, Rng2 &&rng2, Rngs... rngs) const -> Rng1& {
            (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            return (*this)(std::forward<Rng1>(rng1), std::forward<Rngs>(rngs)...);
        }

        template <typename Rng2>
        auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires concepts::can_concat_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_ACTION(concat);
}
