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
}
