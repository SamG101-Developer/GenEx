module;
#include <coroutine>
#include <functional>

export module genex.views.drop;
export import genex.generator;
import genex.concepts;
import genex.iterators.distance;
import genex.meta;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct drop_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, const size_t n) const -> generator<range_value_t<Rng>> {
            auto i = 0;
            for (auto &&x : rng) {
                if (i++ < n) { continue; }
                co_yield std::forward<decltype(x)>(x);
            }
        }
    };

    struct drop_last_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, size_t n) const -> generator<range_value_t<Rng>> {
            const auto length = iterators::distance(std::forward<Rng>(rng));
            size_t i = 0;
            for (auto &&x : rng) {
                if (i++ < length - n) { co_yield std::forward<decltype(x)>(x); }
                else { break; }
            }
        }
    };

    struct drop_while_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            auto dropping = true;
            for (auto &&x : rng) {
                if (dropping && std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
                dropping = false;
                co_yield std::forward<decltype(x)>(x);
            }
        }
    };

    struct drop_until_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            auto dropping = true;
            for (auto &&x : rng) {
                if (dropping && !std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), std::forward<decltype(x)>(x)))) { continue; }
                dropping = false;
                co_yield std::forward<decltype(x)>(x);
            }
        }
    };

    struct drop_fn final : drop_base_fn {
        using drop_base_fn::operator();

        auto operator()(size_t n) const -> decltype(auto) {
            return [n = n, this]<range Rng>(Rng &&rng) {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    struct drop_last_fn final : drop_last_base_fn {
        using drop_last_base_fn::operator();

        auto operator()(size_t n) const -> decltype(auto) {
            return [n = n, this]<range Rng>(Rng &&rng) {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    struct drop_while_fn final : drop_while_base_fn {
        using drop_while_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    struct drop_until_fn final : drop_until_base_fn {
        using drop_until_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr drop_fn drop;
    export inline constexpr drop_last_fn drop_last;
    export inline constexpr drop_while_fn drop_while;
    export inline constexpr drop_until_fn drop_until;
}
