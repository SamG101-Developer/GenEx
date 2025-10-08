#pragma once
#include <functional>
#include <utility>

#include <genex/macros.hpp>


namespace genex::meta::detail::concepts {
    template <typename T>
    concept dereferenceable = requires(T &&t) {
        { *std::forward<T>(t) };
    };
}


namespace genex::meta::detail {
    template <typename T>
    requires concepts::dereferenceable<T>
    GENEX_INLINE auto dereference(T &&) -> decltype(*std::declval<T&>()) {
        return *std::declval<T&>();
    }

    template <typename T>
    requires (not concepts::dereferenceable<T>)
    GENEX_INLINE auto dereference(T &&t) -> T&& {
        return std::forward<T>(t);
    }
}


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
         * obj.*mp(args...) or obj->*mp(args...).
         */
        template <typename T, typename C, typename Obj>
        GENEX_INLINE constexpr auto operator()(T C::*f, Obj &&obj) const noexcept(
            noexcept(detail::dereference(std::forward<Obj>(obj)).*f))
            -> decltype(detail::dereference(std::forward<Obj>(obj)).*f) {
            return detail::dereference(std::forward<Obj>(obj)).*f;
        }

        /**
         * Member function pointer invoke function that invokes a member function pointer on an object with arguments:
         * (obj.*mp)(args...) or (mp->*obj)(args...).
         */
        template <typename R, typename C, typename Obj, typename... Args>
        GENEX_INLINE constexpr auto operator()(R C::*f, Obj &&obj, Args &&... args) const noexcept(
            noexcept((detail::dereference(std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...)))
            -> decltype((detail::dereference(std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...)) {
            return (detail::dereference(std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...);
        }
    };

    inline constexpr invoke_fn invoke{};
}
