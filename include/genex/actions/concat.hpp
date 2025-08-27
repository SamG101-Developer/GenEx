#pragma once
#include <genex/concepts.hpp>
#include <genex/actions/push.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>


namespace genex::actions {
    template <typename... Rngs>
    concept can_concat_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...) and
        requires { typename std::common_type_t<range_value_t<Rngs>...>; };

    DEFINE_ACTION(concat) {
        template <typename Rng1, typename Rng2> requires can_concat_range<Rng1, Rng2>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const -> Rng1& {
            for (auto &&x : rng2) {
                rng1 |= push(iterators::end(rng1), std::forward<decltype(x)>(x));
            }
            return rng1;
        }

        template <typename Rng1, typename Rng2, typename... Rngs> requires can_concat_range<Rng1, Rng2, Rngs...>
        constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Rngs... rngs) const -> Rng1& {
            (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            return (*this)(std::forward<Rng1>(rng1), std::forward<Rngs>(rngs)...);
        }

        template <typename Rng2>
        constexpr auto operator()(Rng2 &&rng2) const -> auto {
            return
                [FWD_CAPTURES(rng2)]<typename Rng1> requires can_concat_range<Rng1, Rng2>
                (Rng1 &&rng1) mutable -> auto {
                return (*this)(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
            };
        }
    };

    EXPORT_GENEX_ACTION(concat);
}
