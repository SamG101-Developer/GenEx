#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <typename T, iterator I, sentinel_for<I> S>
    auto do_cast(I &&first, S &&last) -> generator<T> {
        for (; first != last; ++first) {
            co_yield static_cast<T>(std::forward<decltype(*first)>(*first));
        }
    }

    template <typename T, iterator I, sentinel_for<I> S> requires (std::is_pointer_v<iter_value_t<I>>)
    auto do_cast(I &&first, S &&last) -> generator<T> {
        for (; first != last; ++first) {
            co_yield dynamic_cast<T>(std::forward<decltype(*first)>(*first));
        }
    }

    template <typename T, iterator I, sentinel_for<I> S> requires (unique_ptr<iter_value_t<I>>)
    auto do_cast(I &&first, S &&last) -> generator<std::unique_ptr<T>> {
        for (; first != last; ++first) {
            co_yield std::unique_ptr<T>(dynamic_cast<T*>(first->release()));
        }
    }

    template <typename T, iterator I, sentinel_for<I> S> requires (shared_ptr<iter_value_t<I>>)
    auto do_cast(I &&first, S &&last) -> generator<std::shared_ptr<T>> {
        for (; first != last; ++first) {
            co_yield std::shared_ptr<T>(dynamic_cast<T*>(first->release()));
        }
    }

    template <typename T, iterator I, sentinel_for<I> S> requires (weak_ptr<iter_value_t<I>>)
    auto do_cast(I &&first, S &&last) -> generator<std::weak_ptr<T>> {
        for (; first != last; ++first) {
            co_yield std::weak_ptr<T>(dynamic_cast<T*>(first->release()));
        }
    }

    template <typename T, range Rng>
    auto do_cast(Rng &&rng) -> generator<T> {
        for (auto &&x : rng) {
            co_yield static_cast<T>(std::forward<decltype(x)>(x));
        }
    }

    template <typename T, range Rng> requires (std::is_pointer_v<range_value_t<Rng>>)
    auto do_cast(Rng &&rng) -> generator<T> {
        for (auto &&x : rng) {
            co_yield dynamic_cast<T>(std::forward<decltype(x)>(x));
        }
    }

    template <typename T, range Rng> requires (unique_ptr<range_value_t<Rng>>)
    auto do_cast(Rng &&rng) -> generator<std::unique_ptr<T>> {
        for (auto &&x : rng) {
            co_yield std::unique_ptr<T>(dynamic_cast<T*>(x.release()));
        }
    }

    template <typename T, range Rng> requires (shared_ptr<range_value_t<Rng>>)
    auto do_cast(Rng &&rng) -> generator<std::shared_ptr<T>> {
        for (auto &&x : rng) {
            co_yield std::shared_ptr<T>(dynamic_cast<T*>(x.release()));
        }
    }

    template <typename T, range Rng> requires (weak_ptr<range_value_t<Rng>>)
    auto do_cast(Rng &&rng) -> generator<std::weak_ptr<T>> {
        for (auto &&x : rng) {
            co_yield std::weak_ptr<T>(dynamic_cast<T*>(x.release()));
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(cast) {
        DEFINE_OUTPUT_TYPE(cast);

        template <iterator I, sentinel_for<I> S, typename T>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_cast<T>, first, last);
        }

        template <iterator I, sentinel_for<I> S, typename T> requires (unique_ptr<iter_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_cast<T>, first, last);
        }

        template <range Rng, typename T>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_cast<T>, rng);
        }

        template <range Rng, typename T> requires (unique_ptr<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_cast<T>, rng);
        }

        template <typename T>
        constexpr auto operator()() const -> auto {
            return [this]<range Rng>(Rng &&rng) mutable -> auto { return this->operator()<Rng, T>(std::forward<Rng>(rng)); };
        }
    };

    EXPORT_GENEX_VIEW(cast);
}
