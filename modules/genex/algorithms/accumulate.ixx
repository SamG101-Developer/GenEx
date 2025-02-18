module;
#include <functional>

export module genex.algorithms.accumulate;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms {
    struct accumulate_base_fn {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, typename T, std::invocable<T, iter_value_t<I>> BinaryOp>
        auto operator()(I &&first, S &&last, T &&init, BinaryOp &&binary_op, Proj &&proj = {}) const -> T {
            for (auto it = first; it != last; ++it) {
                init = std::invoke(std::forward<BinaryOp>(binary_op), std::forward<T>(init), std::invoke(std::forward<Proj>(proj), *it));
            }
            return init;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, typename T, std::invocable<T, range_value_t<Rng>> BinaryOp>
        auto operator()(Rng &&rng, T &&init, BinaryOp &&binary_op, Proj &&proj = {}) const -> T {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<T>(init), std::forward<BinaryOp>(binary_op), std::forward<Proj>(proj));
        }
    };

    struct accumulate_fn final : accumulate_base_fn {
        using accumulate_base_fn::operator();

        template <typename T, typename BinaryOp, typename Proj = meta::identity>
        auto operator()(T &&init, BinaryOp &&binary_op, Proj &&proj = {}) const -> decltype(auto) {
            return [init = std::forward<T>(init), binary_op = std::forward<BinaryOp>(binary_op), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<T>(init), std::forward<BinaryOp>(binary_op), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr accumulate_fn accumulate;
}
