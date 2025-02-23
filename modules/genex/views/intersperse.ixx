module;
#include <coroutine>
#include <functional>

export module genex.views.intersperse;
export import genex.generator;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.distance;
import genex.iterators.advance;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct intersperse_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, range_value_t<Rng> const &separator) const -> generator<range_value_t<Rng>> {
            if (iterators::distance(std::forward<Rng>(rng)) == 0) { co_return; }
            auto first = iterators::begin(rng);
            co_yield *first;
            for (auto it = iterators::advance(first); it != iterators::end(rng); iterators::advance(it)) {
                co_yield separator;
                co_yield *it;
            }
        }
    };

    struct intersperse_with_base_fn : intersperse_base_fn {
        template <range Rng, std::invocable F> requires std::same_as<std::invoke_result_t<F>, range_value_t<Rng>>
        auto operator()(Rng &&rng, F &&separator) const -> generator<range_value_t<Rng>> {
            if (iterators::distance(std::forward<Rng>(rng)) == 0) { co_return; }
            auto first = iterators::begin(rng);
            co_yield *first;
            for (auto it = iterators::advance(first); it != iterators::end(rng); iterators::advance(it)) {
                co_yield std::invoke(std::forward<F>(separator));
                co_yield *it;
            }
        }
    };

    struct intersperse_fn final : intersperse_base_fn {
        using intersperse_base_fn::operator();

        template <typename E>
        auto operator()(E &&separator) const -> decltype(auto) {
            return [separator = std::forward<E>(separator), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(separator));
            };
        }
    };

    struct intersperse_with_fn final : intersperse_with_base_fn {
        using intersperse_with_base_fn::operator();

        template <std::invocable F>
        auto operator()(F &&separator) const -> decltype(auto) {
            return [separator = std::forward<F>(separator), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<F>(separator));
            };
        }
    };

    export inline constexpr intersperse_fn intersperse;
    export inline constexpr intersperse_with_fn intersperse_with;
}
