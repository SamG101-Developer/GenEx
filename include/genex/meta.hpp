#pragma once
#include <functional>
#include <utility>

#include <genex/macros.hpp>


namespace genex::meta {
    struct identity {
        template <typename T>
        GENEX_INLINE constexpr auto operator()(T &&x) const -> T&& {
            return std::forward<T>(x);
        }
    };

    struct invoke_fn {
        /**
         * Regular invoke function that perfectly forwards its arguments to the callable:
         * f(args...).
         */
        template <typename F, typename... Args>
        GENEX_INLINE constexpr auto operator()(F &&f, Args &&... args) const noexcept(
            noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
            -> decltype(std::forward<F>(f)(std::forward<Args>(args)...)) {
            return std::forward<F>(f)(std::forward<Args>(args)...);
        }

        /**
         * Member pointer invoke function that invokes a member function pointer on an object:
         * obj.*mp(args...) or mp->*obj(args...).
         */
        template <typename T, typename C, typename Obj>
        GENEX_INLINE constexpr auto operator()(T C::*mp, Obj &&obj) const noexcept(
            noexcept(obj.*mp))
            -> decltype(obj.*mp) {
            return obj.*mp;
        }

        /**
         * Member function pointer invoke function that invokes a member function pointer on an object with arguments:
         * (obj.*mp)(args...) or (mp->*obj)(args...).
         */
        template <typename R, typename C, typename Obj, typename... Args>
        GENEX_INLINE constexpr auto operator()(R C::*mp, Obj &&obj, Args &&... args) const noexcept(
            noexcept((obj.*mp)(std::forward<Args>(args)...)))
            -> decltype((obj.*mp)(std::forward<Args>(args)...)) {
            return (obj.*mp)(std::forward<Args>(args)...);
        }
    };

    inline constexpr invoke_fn invoke{};
}
