module;
#include <coroutine>
#include <functional>

export module genex.views.intersperse;
import genex.concepts;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.next;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct intersperse_base_fn : detail::view_base {
        template <iterator I, sentinel S, typename E> requires std::same_as<iter_value_t<I>, E>
        auto operator()(I &&first, S &&last, E&& separator) const -> generator<iter_value_t<I>> {
            if (first == last) { co_return; }
            co_yield *first;
            for (auto it = iterators::next(first); it != last; ++it) {
                co_yield *it;
                co_yield std::forward<E>(separator);
            }
        }

        template <range Rng>
        auto operator()(Rng &&rng, range_value_t<Rng> &&separator) const -> generator<range_value_t<Rng>> {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<range_value_t<Rng>>(separator));
        }
    };

    struct intersperse_with_base_fn : intersperse_base_fn {
        template <iterator I, sentinel S, std::invocable F> requires std::same_as<std::invoke_result_t<F>, iter_value_t<I>>
        auto operator()(I &&first, S &&last, F &&separator) const -> generator<iter_value_t<I>> {
            if (first == last) { co_return; }
            co_yield *first;
            for (auto it = iterators::next(first); it != last; ++it) {
                co_yield *it;
                co_yield std::invoke(std::forward<F>(separator));
            }
        }

        template <range Rng, std::invocable F> requires std::same_as<std::invoke_result_t<F>, range_value_t<Rng>>
        auto operator()(Rng &&rng, F &&separator) const -> generator<range_value_t<Rng>> {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<F>(separator));
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
