module;
#include <coroutine>
#include <functional>

export module genex.views.take;
import genex.concepts;
import genex.generator;
import genex.iterators.distance;
import genex.meta;
import genex.type_traits;
import genex.views._view_base;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    struct take_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, const size_t n) const -> generator<range_value_t<Rng>> {
            auto i = 0;
            for (auto &&x : rng) {
                if (i++ >= n) { break; }
                co_yield x;
            }
        }
    };

    struct take_last_base_fn : detail::view_base {
        template <range Rng>
        auto operator()(Rng &&rng, size_t n) const -> generator<range_value_t<Rng>> {
            const auto length = iterators::distance(rng);
            auto i = 0;
            for (auto &&x : rng) {
                if (i++ < length - n) { continue; }
                co_yield x;
            }
        }
    };

    struct take_while_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            for (auto &&x : rng) {
                if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), x))) { break; }
                co_yield x;
            }
        }
    };

    struct take_until_base_fn : detail::view_base {
        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> generator<range_value_t<Rng>> {
            for (auto &&x : rng) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), x))) { break; }
                co_yield x;
            }
        }
    };

    struct take_fn final : take_base_fn {
        using take_base_fn::operator();

        auto operator()(size_t n) const -> decltype(auto) {
            return [n = n, this]<range Rng>(Rng &&rng) {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    struct take_last_fn final : take_last_base_fn {
        using take_last_base_fn::operator();

        auto operator()(size_t n) const -> decltype(auto) {
            return [n = n, this]<range Rng>(Rng &&rng) {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    struct take_while_fn final : take_while_base_fn {
        using take_while_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    struct take_until_fn final : take_until_base_fn {
        using take_until_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr take_fn take;
    export inline constexpr take_last_fn take_last;
    export inline constexpr take_while_fn take_while;
    export inline constexpr take_until_fn take_until;
}
