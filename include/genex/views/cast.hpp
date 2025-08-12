#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


template <iterator I, sentinel S, typename T>
auto do_cast(I &&first, S &&last) -> genex::generator<T> {
    for (; first != last; ++first) {
        co_yield static_cast<T>(std::forward<decltype(*first)>(*first));
    }
}


template <iterator I, sentinel S, typename T> requires (unique_ptr<iter_value_t<I>>)
auto do_cast(I &&first, S &&last) -> genex::generator<T> {
    for (; first != last; ++first) {
        co_yield std::unique_ptr<T>(dynamic_cast<T*>(first->release()));
    }
}


template <range Rng, typename T>
auto do_cast(Rng &&rng) -> genex::generator<T> {
    for (auto &&x : rng) {
        co_yield static_cast<T>(std::forward<decltype(x)>(x));
    }
}


template <range Rng, typename T> requires (unique_ptr<range_value_t<Rng>>)
auto do_cast(Rng &&rng) -> genex::generator<T> {
    for (auto &&x : rng) {
        co_yield std::unique_ptr<T>(dynamic_cast<T*>(x.release()));
    }
}


namespace genex::views {
    struct cast_fn final : detail::view_base {
        template <iterator I, sentinel S, typename T>
        constexpr auto operator()(I &&first, S &&last) -> generator<T> {
            MAP_TO_IMPL(do_cast<T>, first, last);
        }

        template <range Rng, typename T>
        constexpr auto operator()(Rng &&rng) -> generator<T> {
            MAP_TO_IMPL(do_cast<T>, rng);
        }

        template <typename T>
        constexpr auto operator()() -> decltype(auto) {
            return [this]<range Rng>(Rng &&rng) mutable -> decltype(auto) { return this->operator()<Rng, T>(std::forward<Rng>(rng)); };
        }
    };

    EXPORT_GENEX_STRUCT(cast);
}
