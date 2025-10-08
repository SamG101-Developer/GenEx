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
    GENEX_INLINE auto dereference(T &&t) -> decltype(*std::declval<T&>()) {
        return *std::forward<T>(t);
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


namespace genex::meta::detail {
    template <typename F, typename... BoundArgs>
    struct bind_back_impl_fn {
        std::tuple<F, BoundArgs...> func_and_args;

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && noexcept(
            noexcept(std::is_nothrow_invocable_v<F, CallArgs..., BoundArgs...>))
            -> decltype(auto) {
            return std::apply([&](F &&f, BoundArgs &&... bound_args) -> decltype(auto) {
                return meta::invoke(std::forward<F>(f), std::forward<CallArgs>(call_args)..., std::forward<BoundArgs>(bound_args)...);
            }, std::move(func_and_args));
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & noexcept(
            noexcept(std::is_nothrow_invocable_v<F&, CallArgs..., BoundArgs&...>))
            -> decltype(auto) {
            return std::apply([&](F &f, BoundArgs &... bound_args) -> decltype(auto) {
                return meta::invoke(f, std::forward<CallArgs>(call_args)..., bound_args...);
            }, func_and_args);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & noexcept(
            noexcept(std::is_nothrow_invocable_v<F const&, CallArgs..., BoundArgs const&...>))
            -> decltype(auto) {
            return std::apply([&](F const &f, BoundArgs const &... bound_args) -> decltype(auto) {
                return meta::invoke(f, std::forward<CallArgs>(call_args)..., bound_args...);
            }, func_and_args);
        }
    };

    // 1-arg optimization
    template <typename F, typename BoundArg>
    struct bind_back_impl_fn<F, BoundArg> {
        struct func_and_arg {
            F func;
            BoundArg arg;
        } func_and_arg;

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && noexcept(
            noexcept(std::is_nothrow_invocable_v<F, CallArgs..., BoundArg>))
            -> decltype(auto) {
            return meta::invoke(std::move(func_and_arg.func), std::forward<CallArgs>(call_args)..., std::move(func_and_arg.arg));
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & noexcept(
            noexcept(std::is_nothrow_invocable_v<F&, CallArgs..., BoundArg&>))
            -> decltype(auto) {
            return meta::invoke(func_and_arg.func, std::forward<CallArgs>(call_args)..., func_and_arg.arg);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & noexcept(
            noexcept(std::is_nothrow_invocable_v<F const&, CallArgs..., BoundArg const&>))
            -> decltype(auto) {
            return meta::invoke(func_and_arg.func, std::forward<CallArgs>(call_args)..., func_and_arg.arg);
        }
    };

    // 2-arg optimization
    template <typename F, typename BoundArg1, typename BoundArg2>
    struct bind_back_impl_fn<F, BoundArg1, BoundArg2> {
        struct func_and_args {
            F func;
            BoundArg1 arg1;
            BoundArg2 arg2;
        } func_and_args;

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && noexcept(
            noexcept(std::is_nothrow_invocable_v<F, CallArgs..., BoundArg1, BoundArg2>))
            -> decltype(auto) {
            return meta::invoke(std::move(func_and_args.func), std::forward<CallArgs>(call_args)..., std::move(func_and_args.arg1), std::move(func_and_args.arg2));
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & noexcept(
            noexcept(std::is_nothrow_invocable_v<F&, CallArgs..., BoundArg1&, BoundArg2&>))
            -> decltype(auto) {
            return meta::invoke(func_and_args.func, std::forward<CallArgs>(call_args)..., func_and_args.arg1, func_and_args.arg2);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & noexcept(
            noexcept(std::is_nothrow_invocable_v<F const&, CallArgs..., BoundArg1 const&, BoundArg2 const&>))
            -> decltype(auto) {
            return meta::invoke(func_and_args.func, std::forward<CallArgs>(call_args)..., func_and_args.arg1, func_and_args.arg2);
        }
    };
}


namespace genex::meta {
    struct bind_back {
        template <typename F, typename... BoundArgs>
        GENEX_INLINE constexpr auto operator()(F &&f, BoundArgs &&... bound_args) const
            -> detail::bind_back_impl_fn<std::decay_t<F>, std::decay_t<BoundArgs>...> {
            return {{std::forward<F>(f), std::forward<BoundArgs>(bound_args)...}};
        }
    };

    inline constexpr bind_back bind_back{};
}
