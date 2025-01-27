module;
#include <coroutine>
#include <functional>

export module genex.views.fold;
import genex.concepts;
import genex.generator;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;

namespace genex::views {
    struct fold_left_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
        auto operator()(Rng &&rng, range_value_t<Rng> &&init, F &&f) const -> range_value_t<Rng> {
            auto acc = std::forward<range_value_t<Rng>>(init);
            for (auto &&x : rng) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), std::forward<decltype(x)>(x));
            }
            return acc;
        }
    };

    struct fold_right_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
        auto operator()(Rng &&rng, range_value_t<Rng> &&init, F &&f) const -> range_value_t<Rng> {
            auto acc = std::forward<range_value_t<Rng>>(init);
            for (auto it = iterators::rbegin(rng); it != iterators::rend(rng); ++it) {
                acc = std::invoke(std::forward<F>(f), *it, std::move(acc));
            }
            return acc;
        }
    };

    struct fold_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>, range_value_t<Rng>> F>
        auto operator()(Rng &&rng, range_value_t<Rng> &&init, F &&f) const -> range_value_t<Rng> {
            auto acc = std::forward<range_value_t<Rng>>(init);
            for (auto &&x : rng) {
                acc = std::invoke(std::forward<F>(f), std::move(acc), std::forward<decltype(x)>(x));
            }
            return acc;
        }
    };

    struct fold_left_fn final : fold_left_base_fn {
        using fold_left_base_fn::operator();

        template <typename E, std::invocable<E, E> F>
        auto operator()(E &&init, F &&f) const -> decltype(auto) {
            return [init = std::forward<E>(init), f = std::forward<F>(f), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(init), std::forward<F>(f));
            };
        }
    };

    struct fold_right_fn final : fold_right_base_fn {
        using fold_right_base_fn::operator();

        template <typename E, std::invocable<E, E> F>
        auto operator()(E &&init, F &&f) const -> decltype(auto) {
            return [init = std::forward<E>(init), f = std::forward<F>(f), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(init), std::forward<F>(f));
            };
        }
    };

    struct fold_fn final : fold_base_fn {
        using fold_base_fn::operator();

        template <typename E, std::invocable<E, E> F>
        auto operator()(E &&init, F &&f) const -> decltype(auto) {
            return [init = std::forward<E>(init), f = std::forward<F>(f), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(init), std::forward<F>(f));
            };
        }
    };

    export inline constexpr fold_left_fn fold_left;
    export inline constexpr fold_right_fn fold_right;
    export inline constexpr fold_fn fold;
}
