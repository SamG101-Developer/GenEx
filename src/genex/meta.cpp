module;
#include <genex/macros.hpp>

export module genex.meta;
import std;


export namespace genex::meta::detail::concepts {
    template <typename T>
    concept dereferenceable = requires(T &&t) {
        { *std::forward<T>(t) };
    };
}


namespace genex::meta {
    struct deref_fn {
        template <typename T>
        requires detail::concepts::dereferenceable<T>
        GENEX_INLINE constexpr auto operator()(T &&t) const -> decltype(*std::declval<T&>()) {
            return *std::forward<T>(t);
        }

        template <typename T>
        requires (not detail::concepts::dereferenceable<T>)
        GENEX_INLINE constexpr auto operator()(T &&t) const -> T&& {
            return std::forward<T>(t);
        }
    };

    export inline constexpr deref_fn deref{};

    export struct identity {
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
        GENEX_INLINE constexpr auto operator()(F &&f, Args &&... args) const
            -> decltype(std::forward<F>(f)(std::forward<Args>(args)...)) {
            return std::forward<F>(f)(std::forward<Args>(args)...);
        }

        /**
         * Member pointer invoke function that invokes a member function pointer on an object:
         * obj.*mp(args...) or obj->*mp(args...).
         */
        template <typename T, typename C, typename Obj>
        GENEX_INLINE constexpr auto operator()(T C::*f, Obj &&obj) const
            -> decltype(deref(std::forward<Obj>(obj)).*f) {
            return deref(std::forward<Obj>(obj)).*f;
        }

        /**
         * Member function pointer invoke function that invokes a member function pointer on an object with arguments:
         * (obj.*mp)(args...) or (mp->*obj)(args...).
         */
        template <typename R, typename C, typename Obj, typename... Args>
        GENEX_INLINE constexpr auto operator()(R C::*f, Obj &&obj, Args &&... args) const
            -> decltype((deref(std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...)) {
            return (deref(std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...);
        }
    };

    export inline constexpr invoke_fn invoke{};
}


namespace genex::meta::detail {
    // N-arg general implementation
    template <typename F, typename... BoundArgs>
    struct bind_back_impl_fn {
        std::tuple<F, BoundArgs...> func_and_args;

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && -> decltype(auto) {
            return std::apply([&](F &&f, BoundArgs &&... bound_args) -> decltype(auto) {
                return meta::invoke(std::forward<F>(f), std::forward<CallArgs>(call_args)..., std::forward<BoundArgs>(bound_args)...);
            }, std::move(func_and_args));
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & -> decltype(auto) {
            return std::apply([&](F &f, BoundArgs &... bound_args) -> decltype(auto) {
                return meta::invoke(f, std::forward<CallArgs>(call_args)..., bound_args...);
            }, func_and_args);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & -> decltype(auto) {
            return std::apply([&](F const &f, BoundArgs const &... bound_args) -> decltype(auto) {
                return meta::invoke(f, std::forward<CallArgs>(call_args)..., bound_args...);
            }, func_and_args);
        }
    };

    // 0-arg optimization
    template <typename F>
    struct bind_back_impl_fn<F> {
        F func;

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && -> decltype(auto) {
            return meta::invoke(std::move(func), std::forward<CallArgs>(call_args)...);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & -> decltype(auto) {
            return meta::invoke(func, std::forward<CallArgs>(call_args)...);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & -> decltype(auto) {
            return meta::invoke(func, std::forward<CallArgs>(call_args)...);
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
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && -> decltype(auto) {
            return meta::invoke(
                std::move(func_and_arg.func), std::forward<CallArgs>(call_args)..., std::move(func_and_arg.arg));
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & -> decltype(auto) {
            return meta::invoke(
                func_and_arg.func, std::forward<CallArgs>(call_args)..., func_and_arg.arg);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & -> decltype(auto) {
            return meta::invoke(
                func_and_arg.func, std::forward<CallArgs>(call_args)..., func_and_arg.arg);
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
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) && -> decltype(auto) {
            return meta::invoke(
                std::move(func_and_args.func), std::forward<CallArgs>(call_args)..., std::move(func_and_args.arg1), std::move(func_and_args.arg2));
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) & -> decltype(auto) {
            return meta::invoke(
                func_and_args.func, std::forward<CallArgs>(call_args)..., func_and_args.arg1, func_and_args.arg2);
        }

        template <typename... CallArgs>
        GENEX_INLINE constexpr auto operator()(CallArgs &&... call_args) const & -> decltype(auto) {
            return meta::invoke(
                func_and_args.func, std::forward<CallArgs>(call_args)..., func_and_args.arg1, func_and_args.arg2);
        }
    };
}


namespace genex::meta {
    struct bind_back_fn {
        template <typename F, typename... BoundArgs>
        GENEX_INLINE constexpr auto operator()(F &&f, BoundArgs &&... bound_args) const
            -> detail::bind_back_impl_fn<std::decay_t<F>, std::decay_t<BoundArgs>...> {
            return {{std::forward<F>(f), std::forward<BoundArgs>(bound_args)...}};
        }
    };

    export inline constexpr bind_back_fn bind_back{};
}


namespace genex::meta {
    export template <typename T>
    struct semiregular_box {
        std::optional<T> val;

        GENEX_INLINE constexpr explicit semiregular_box() requires std::default_initializable<T> : val(std::in_place) {}
        GENEX_INLINE constexpr explicit semiregular_box() = default;
        GENEX_INLINE constexpr explicit semiregular_box(T v) : val(std::move(v)) {}

        GENEX_INLINE constexpr T& get() & { return *val; }
        GENEX_INLINE constexpr T const& get() const & { return *val; }
        GENEX_INLINE constexpr T&& get() && { return std::move(*val); }

        explicit constexpr operator bool() const noexcept { return val.has_value(); }
    };

    export template <typename T>
    using semiregular_box_t = std::conditional_t<std::semiregular<T>, T, semiregular_box<T>>;

    export template <typename T>
    GENEX_INLINE constexpr auto semi_get(semiregular_box<T> &b) noexcept -> decltype(auto) {
        return b.get();
    }

    export template <typename T>
    GENEX_INLINE constexpr auto semi_get(semiregular_box<T> const &b) noexcept -> decltype(auto) {
        return b.get();
    }

    export template <typename T>
    GENEX_INLINE constexpr auto semi_get(semiregular_box<T> &&b) noexcept -> decltype(auto) {
        return std::move(b).get();
    }

    export template <typename T>
    requires (not std::same_as<std::remove_cvref_t<T>, semiregular_box<std::remove_cvref_t<T>>>)
    GENEX_INLINE constexpr auto semi_get(T &&v) noexcept -> decltype(auto) {
        return std::forward<T>(v);
    }
}
