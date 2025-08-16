#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <typename T, iterator I, sentinel S>
    auto do_cast(I &&first, S &&last) -> generator<T> {
        for (; first != last; ++first) {
            co_yield static_cast<T>(std::forward<decltype(*first)>(*first));
        }
    }

    template <typename T, iterator I, sentinel S> requires (unique_ptr<iter_value_t<I>>)
    auto do_cast(I &&first, S &&last) -> generator<std::unique_ptr<T>> {
        for (; first != last; ++first) {
            co_yield std::unique_ptr<T>(dynamic_cast<T*>(first->release()));
        }
    }

    template <typename T, range Rng>
    auto do_cast(Rng &&rng) -> generator<T> {
        for (auto &&x : rng) {
            co_yield static_cast<T>(std::forward<decltype(x)>(x));
        }
    }

    template <typename T, range Rng> requires (unique_ptr<range_value_t<Rng>>)
    auto do_cast(Rng &&rng) -> generator<std::unique_ptr<T>> {
        for (auto &&x : rng) {
            co_yield std::unique_ptr<T>(dynamic_cast<T*>(x.release()));
        }
    }
}


namespace genex::views {
    struct cast_fn final : detail::view_base {
        template <iterator I, sentinel S, typename T>
        constexpr auto operator()(I &&first, S &&last) const -> generator<T> {
            MAP_TO_IMPL(detail::do_cast<T>, first, last);
        }

        template <iterator I, sentinel S, typename T> requires (unique_ptr<iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> generator<std::unique_ptr<T>> {
            MAP_TO_IMPL(detail::do_cast<T>, first, last);
        }


        template <range Rng, typename T>
        constexpr auto operator()(Rng &&rng) const -> generator<T> {
            MAP_TO_IMPL(detail::do_cast<T>, rng);
        }

        template <range Rng, typename T> requires (unique_ptr<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> generator<std::unique_ptr<T>> {
            MAP_TO_IMPL(detail::do_cast<T>, rng);
        }

        template <typename T>
        constexpr auto operator()() const -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng) mutable -> decltype(auto) { return this->operator()<Rng, T>(std::forward<Rng>(rng)); };
        }
    };

    inline constexpr cast_fn cast{};
}
