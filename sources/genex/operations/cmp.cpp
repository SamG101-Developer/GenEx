module;
#include <genex/detail/macros.hpp>

export module genex.operations.cmp;
import genex.meta;
import std;

namespace genex::operations::detail::concepts {
  template <typename T, typename U, typename Proj = meta::identity>
  concept comparable_with =
    std::invocable<Proj, T> and
    std::invocable<Proj, U> and
    std::equality_comparable_with<
      std::invoke_result_t<Proj, T>, std::invoke_result_t<Proj, U>>;

  template <typename T, typename U, typename Proj = meta::identity>
  concept orderable_with =
    std::invocable<Proj, T> and
    std::invocable<Proj, U> and
    std::totally_ordered_with<
      std::invoke_result_t<Proj, T>, std::invoke_result_t<Proj, U>>;
}

namespace genex::operations {
  export struct eq {
    template <typename T, typename U>
    requires detail::concepts::comparable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b) const noexcept(
      SAFE_NOEX(static_cast<bool>(std::forward<T>(a) == std::forward<U>(b)))) -> bool {
      return std::forward<T>(a) == std::forward<U>(b);
    }

    template <typename T, typename U, typename Proj>
    requires detail::concepts::comparable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b, Proj proj) const noexcept(
      SAFE_NOEX(static_cast<bool>(
        meta::invoke(proj, std::forward<T>(a)) == meta::invoke(proj, std::forward<U>(b))))) -> bool {
      return meta::invoke(
        proj, std::forward<T>(a)) == meta::invoke(proj, std::forward<U>(b));
    }
  };

  export struct ne {
    template <typename T, typename U>
    requires detail::concepts::comparable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b) const noexcept(
      SAFE_NOEX(static_cast<bool>(std::forward<T>(a) != std::forward<U>(b)))) -> bool {
      return std::forward<T>(a) != std::forward<U>(b);
    }

    template <typename T, typename U, typename Proj>
    requires detail::concepts::comparable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b, Proj proj) const noexcept(
      SAFE_NOEX(static_cast<bool>(
        meta::invoke(proj, std::forward<T>(a)) != meta::invoke(proj, std::forward<U>(b))))) -> bool {
      return meta::invoke(
        proj, std::forward<T>(a)) != meta::invoke(proj, std::forward<U>(b));
    }
  };

  export struct lt {
    template <typename T, typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b) const noexcept(
      SAFE_NOEX(static_cast<bool>(std::forward<T>(a) < std::forward<U>(b)))) -> bool {
      return std::forward<T>(a) < std::forward<U>(b);
    }

    template <typename T, typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b, Proj proj) const noexcept(
      SAFE_NOEX(static_cast<bool>(
        meta::invoke(proj, std::forward<T>(a)) < meta::invoke(proj, std::forward<U>(b))))) -> bool {
      return meta::invoke(
        proj, std::forward<T>(a)) < meta::invoke(proj, std::forward<U>(b));
    }
  };

  export struct le {
    template <typename T, typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b) const noexcept(
      SAFE_NOEX(static_cast<bool>(std::forward<T>(a) <= std::forward<U>(b)))) -> bool {
      return std::forward<T>(a) <= std::forward<U>(b);
    }

    template <typename T, typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b, Proj proj) const noexcept(
      SAFE_NOEX(static_cast<bool>(meta::invoke(proj, std::forward<T>(a)) <= meta::invoke(proj, std::forward<U>(b))))) -> bool {
      return meta::invoke(
        proj, std::forward<T>(a)) <= meta::invoke(proj, std::forward<U>(b));
    }
  };

  export struct gt {
    template <typename T, typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b) const noexcept(
      SAFE_NOEX(static_cast<bool>(std::forward<T>(a) > std::forward<U>(b)))) -> bool {
      return std::forward<T>(a) > std::forward<U>(b);
    }

    template <typename T, typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b, Proj proj) const noexcept(
      SAFE_NOEX(static_cast<bool>(
        meta::invoke(proj, std::forward<T>(a)) > meta::invoke(proj, std::forward<U>(b))))) -> bool {
      return meta::invoke(
        proj, std::forward<T>(a)) > meta::invoke(proj, std::forward<U>(b));
    }
  };

  export struct ge {
    template <typename T, typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b) const noexcept(
      SAFE_NOEX(static_cast<bool>(std::forward<T>(a) >= std::forward<U>(b)))) -> bool {
      return std::forward<T>(a) >= std::forward<U>(b);
    }

    template <typename T, typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      T &&a, U &&b, Proj proj) const noexcept(
      SAFE_NOEX(static_cast<bool>(
        meta::invoke(proj, std::forward<T>(a)) >= meta::invoke(proj, std::forward<U>(b))))) -> bool {
      return meta::invoke(
        proj, std::forward<T>(a)) >= meta::invoke(proj, std::forward<U>(b));
    }
  };

  export template <typename T>
  struct eq_fixed {
    T lhs;

    template <typename U>
    requires detail::concepts::comparable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs) const noexcept(
      SAFE_CALL(eq, const T&, U)) -> bool {
      return meta::invoke(
        eq{}, lhs, std::forward<U>(rhs));
    }

    template <typename U, typename Proj>
    requires detail::concepts::comparable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs, Proj proj) const noexcept(
      SAFE_CALL(eq, const T&, U, Proj)) -> bool {
      return meta::invoke(
        eq{}, lhs, std::forward<U>(rhs), std::move(proj));
    }
  };

  export template <typename T>
  struct ne_fixed {
    T lhs;

    template <typename U>
    requires detail::concepts::comparable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs) const noexcept(
      SAFE_CALL(ne, const T&, U)) -> bool {
      return meta::invoke(
        ne{}, lhs, std::forward<U>(rhs));
    }

    template <typename U, typename Proj>
    requires detail::concepts::comparable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs, Proj proj) const noexcept(
      SAFE_CALL(ne, const T&, U, Proj)) -> bool {
      return meta::invoke(
        ne{}, lhs, std::forward<U>(rhs), std::move(proj));
    }
  };

  export template <typename T>
  struct gt_fixed {
    T lhs;

    template <typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs) const noexcept(
      SAFE_CALL(gt, const T&, U)) -> bool {
      return meta::invoke(
        gt{}, lhs, std::forward<U>(rhs));
    }

    template <typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs, Proj proj) const noexcept(
      SAFE_CALL(gt, const T&, U, Proj)) -> bool {
      return meta::invoke(
        gt{}, lhs, std::forward<U>(rhs), std::move(proj));
    }
  };

  export template <typename T>
  struct lt_fixed {
    T lhs;

    template <typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs) const noexcept(
      SAFE_CALL(lt, const T&, U)) -> bool {
      return meta::invoke(
        lt{}, lhs, std::forward<U>(rhs));
    }

    template <typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs, Proj proj) const noexcept(
      SAFE_CALL(lt, const T&, U, Proj)) -> bool {
      return meta::invoke(
        lt{}, lhs, std::forward<U>(rhs), std::move(proj));
    }
  };

  export template <typename T>
  struct ge_fixed {
    T lhs;

    template <typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs) const noexcept(
      SAFE_CALL(ge, const T&, U)) -> bool {
      return meta::invoke(
        ge{}, lhs, std::forward<U>(rhs));
    }

    template <typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs, Proj proj) const noexcept(
      SAFE_CALL(ge, const T&, U, Proj)) -> bool {
      return meta::invoke(
        ge{}, lhs, std::forward<U>(rhs), std::move(proj));
    }
  };

  export template <typename T>
  struct le_fixed {
    T lhs;

    template <typename U>
    requires detail::concepts::orderable_with<T, U>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs) const noexcept(
      SAFE_CALL(le, const T&, U)) -> bool {
      return meta::invoke(
        le{}, lhs, std::forward<U>(rhs));
    }

    template <typename U, typename Proj>
    requires detail::concepts::orderable_with<T, U, Proj>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      U &&rhs, Proj proj) const noexcept(
      SAFE_CALL(le, const T&, U, Proj)) -> bool {
      return meta::invoke(
        le{}, lhs, std::forward<U>(rhs), std::move(proj));
    }
  };
}
