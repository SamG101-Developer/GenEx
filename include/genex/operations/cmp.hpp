#pragma once
#include <utility>
#include <genex/macros.hpp>
#include <genex/meta.hpp>


namespace genex::operations::detail::concepts {
    template <typename T, typename U, typename Proj = meta::identity>
    concept comparable_with =
        std::invocable<Proj, T> and
        std::invocable<Proj, U> and
        std::equality_comparable_with<std::invoke_result_t<Proj, T>, std::invoke_result_t<Proj, U>>;

    template <typename T, typename U, typename Proj = meta::identity>
    concept orderable_with =
        std::invocable<Proj, T> and
        std::invocable<Proj, U> and
        std::totally_ordered_with<std::invoke_result_t<Proj, T>, std::invoke_result_t<Proj, U>>;
}


namespace genex::operations {
    struct eq {
        template <typename T, typename U>
        requires detail::concepts::comparable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const -> bool {
            return std::forward<T>(a) == std::forward<U>(b);
        }

        template <typename T, typename U, typename Proj>
        requires detail::concepts::comparable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b, Proj &&proj) const -> bool {
            return meta::invoke(proj, std::forward<T>(a)) == meta::invoke(proj, std::forward<U>(b));
        }
    };

    struct ne {
        template <typename T, typename U>
        requires detail::concepts::comparable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const -> bool {
            return std::forward<T>(a) != std::forward<U>(b);
        }

        template <typename T, typename U, typename Proj>
        requires detail::concepts::comparable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b, Proj &&proj) const -> bool {
            return meta::invoke(proj, std::forward<T>(a)) != meta::invoke(proj, std::forward<U>(b));
        }
    };

    struct lt {
        template <typename T, typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const -> bool {
            return std::forward<T>(a) < std::forward<U>(b);
        }

        template <typename T, typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b, Proj &&proj) const -> bool {
            return meta::invoke(proj, std::forward<T>(a)) < meta::invoke(proj, std::forward<U>(b));
        }
    };

    struct le {
        template <typename T, typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const -> bool {
            return std::forward<T>(a) <= std::forward<U>(b);
        }

        template <typename T, typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b, Proj &&proj) const -> bool {
            return meta::invoke(proj, std::forward<T>(a)) <= meta::invoke(proj, std::forward<U>(b));
        }
    };

    struct gt {
        template <typename T, typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const -> bool {
            return std::forward<T>(a) > std::forward<U>(b);
        }

        template <typename T, typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b, Proj &&proj) const -> bool {
            return meta::invoke(proj, std::forward<T>(a)) > meta::invoke(proj, std::forward<U>(b));
        }
    };

    struct ge {
        template <typename T, typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b) const -> bool {
            return std::forward<T>(a) >= std::forward<U>(b);
        }

        template <typename T, typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(T &&a, U &&b, Proj &&proj) const -> bool {
            return meta::invoke(proj, std::forward<T>(a)) >= meta::invoke(proj, std::forward<U>(b));
        }
    };

    template <typename T>
    struct eq_fixed {
        T lhs;

        template <typename U>
        requires detail::concepts::comparable_with<T, U>
        GENEX_INLINE constexpr auto operator()(U &&rhs) const -> bool {
            return meta::invoke(eq{}, lhs, std::forward<U>(rhs));
        }

        template <typename U, typename Proj>
        requires detail::concepts::comparable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(U &&rhs, Proj &&proj) const -> bool {
            return meta::invoke(eq{}, lhs, std::forward<U>(rhs), std::forward<Proj>(proj));
        }
    };

    template <typename T>
    struct ne_fixed {
        T lhs;

        template <typename U>
        requires detail::concepts::comparable_with<T, U>
        GENEX_INLINE constexpr auto operator()(U &&rhs) const -> bool {
            return meta::invoke(ne{}, lhs, std::forward<U>(rhs));
        }

        template <typename U, typename Proj>
        requires detail::concepts::comparable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(U &&rhs, Proj &&proj) const -> bool {
            return meta::invoke(ne{}, lhs, std::forward<U>(rhs), std::forward<Proj>(proj));
        }
    };

    template <typename T>
    struct gt_fixed {
        T lhs;

        template <typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(U &&rhs) const -> bool {
            return meta::invoke(gt{}, lhs, std::forward<U>(rhs));
        }

        template <typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(U &&rhs, Proj &&proj) const -> bool {
            return meta::invoke(gt{}, lhs, std::forward<U>(rhs), std::forward<Proj>(proj));
        }
    };

    template <typename T>
    struct lt_fixed {
        T lhs;

        template <typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(U &&rhs) const -> bool {
            return meta::invoke(lt{}, lhs, std::forward<U>(rhs));
        }

        template <typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(U &&rhs, Proj &&proj) const -> bool {
            return meta::invoke(lt{}, lhs, std::forward<U>(rhs), std::forward<Proj>(proj));
        }
    };

    template <typename T>
    struct ge_fixed {
        T lhs;

        template <typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(U &&rhs) const -> bool {
            return meta::invoke(ge{}, lhs, std::forward<U>(rhs));
        }

        template <typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(U &&rhs, Proj &&proj) const -> bool {
            return meta::invoke(ge{}, lhs, std::forward<U>(rhs), std::forward<Proj>(proj));
        }
    };

    template <typename T>
    struct le_fixed {
        T lhs;

        template <typename U>
        requires detail::concepts::orderable_with<T, U>
        GENEX_INLINE constexpr auto operator()(U &&rhs) const -> bool {
            return meta::invoke(le{}, lhs, std::forward<U>(rhs));
        }

        template <typename U, typename Proj>
        requires detail::concepts::orderable_with<T, U, Proj>
        GENEX_INLINE constexpr auto operator()(U &&rhs, Proj &&proj) const -> bool {
            return meta::invoke(le{}, lhs, std::forward<U>(rhs), std::forward<Proj>(proj));
        }
    };
}

