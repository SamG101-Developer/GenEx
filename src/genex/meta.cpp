module;
#include <genex/macros.hpp>

export module genex.meta;
import std;

export namespace genex::meta::detail::concepts {
    template <typename T>
    concept dereferenceable = requires(T &&t)
    {
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

    /**
     * A minimal movable-box: wraps a value so it can be stored as a member while keeping the enclosing type assignable
     * and default-constructible, even when @c T is neither. Two problems it solves for a capturing lambda (whose
     * assignment operators are deleted and which has no default constructor):
     *
     *  - Assignability: storing @c T directly would delete the enclosing iterator's assignment and stop it modelling
     *    @c std::movable / @c std::input_iterator. Assignment is emulated via destroy-then-reconstruct of the whole
     *    storage object (mirrors @c std::ranges' movable-box), which only needs @c T (copy/move)-constructible.
     *  - Default-constructibility: when @c T is not default-constructible we store it in an @c std::optional so the box
     *    stays default-constructible (defaulting to an empty/singular state). Without this the iterator is not
     *    @c std::semiregular, which cascades into it failing @c forward_iterator / @c sentinel_for and makes views such
     *    as @c intersperse, @c zip and @c enumerate reject any capturing-lambda @c transform. When @c T is already
     *    default-constructible we store it directly, keeping the box trivial and @c operator* always valid.
     *
     * Access the value with @c operator* (undefined on an empty box, exactly as dereferencing a singular iterator is).
     */
    export template <typename T>
    class box {
        static constexpr bool store_direct = std::default_initializable<T>;
        using storage_t = std::conditional_t<store_direct, T, std::optional<T>>;
        GENEX_NO_UNIQUE_ADDRESS storage_t m_value;

    public:
        box() = default;

        GENEX_INLINE constexpr box(T value) noexcept(std::is_nothrow_move_constructible_v<T>) :
            m_value(std::move(value)) {
        }

        box(const box &) = default;
        box(box &&) = default;
        ~box() = default;

        GENEX_INLINE constexpr auto operator=(const box &that) -> box& requires std::copy_constructible<T> {
            if (this != std::addressof(that)) {
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_value), that.m_value);
            }
            return *this;
        }

        GENEX_INLINE constexpr auto operator=(box &&that) noexcept(std::is_nothrow_move_constructible_v<T>) -> box& requires std::move_constructible<T> {
            if (this != std::addressof(that)) {
                std::destroy_at(std::addressof(m_value));
                std::construct_at(std::addressof(m_value), std::move(that.m_value));
            }
            return *this;
        }

        template <typename Self>
        GENEX_NODISCARD GENEX_INLINE constexpr auto operator*(this Self &&self) noexcept -> decltype(auto) {
            if constexpr (store_direct) { return (std::forward<Self>(self).m_value); }
            else { return (*std::forward<Self>(self).m_value); }
        }
    };
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
    template <typename F>
    struct not_impl_fn {
        F func;

        template <typename... Args>
        GENEX_INLINE constexpr auto operator()(Args &&... args) -> bool {
            return not meta::invoke(func, std::forward<Args>(args)...);
        }
    };

    struct not_fn {
        template <typename F>
        GENEX_INLINE constexpr auto operator()(F &&f) const -> not_impl_fn<std::decay_t<F>> {
            return {std::forward<F>(f)};
        }
    };

    export inline constexpr not_fn not_{};
}
