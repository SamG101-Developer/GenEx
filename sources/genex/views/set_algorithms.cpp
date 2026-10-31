module;
#include <genex/detail/macros.hpp>

export module genex.views.set_algorithms;
export import genex.pipe;
import genex.concepts;
import genex.detail.iterator_ops;
import genex.detail.range_closure;
import genex.iterators.iter_pair;
import genex.meta;
import genex.operations.cmp;
import std;

namespace genex::views::detail::concepts {
  template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
  concept set_algorithmicable_iters =
    std::input_iterator<I1> and
    std::input_iterator<I2> and
    std::sentinel_for<S1, I1> and
    std::sentinel_for<S2, I2> and
    std::indirectly_comparable<I1, I2, Comp, Proj1, Proj2>;

  template <typename Rng1, typename Rng2, typename Comp, typename Proj1, typename Proj2>
  concept set_algorithmicable_range =
    non_dangling_range<Rng1> and
    non_dangling_range<Rng2> and
    input_range<Rng1> and
    input_range<Rng2> and
    set_algorithmicable_iters<
      iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>,
      Comp, Proj1, Proj2>;
}

namespace genex::views::detail::impl {
  enum class set_op { difference, intersection, symmetric_difference, union_ };

  struct set_algorithm_sentinel {};

  template <
    set_op Op, typename I1, typename S1, typename I2, typename S2,
    typename Comp, typename Proj1, typename Proj2>
  requires concepts::set_algorithmicable_iters<
    I1, S1, I2, S2, Comp, Proj1, Proj2>
  struct set_iterator : genex::detail::iterator_ops<set_iterator<Op, I1, S1, I2, S2, Comp, Proj1, Proj2>> {
    I1 it1;
    S1 st1;
    I2 it2;
    S2 st2;
    GENEX_NO_UNIQUE_ADDRESS Comp comp;
    GENEX_NO_UNIQUE_ADDRESS Proj1 proj1;
    GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;

    using value_type = std::common_type_t<
      iter_value_t<I1>, iter_value_t<I2>>;
    using reference = std::common_reference_t<
      iter_reference_t<I1>, iter_reference_t<I2>>;
    using difference_type = std::common_type_t<
      iter_difference_t<I1>, iter_difference_t<I2>>;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = iterator_category;

    std::optional<value_type> cur_elem = std::nullopt;

    GENEX_INLINE constexpr set_iterator() = default;

    GENEX_INLINE constexpr set_iterator(
      I1 first1, S1 last1, I2 first2, S2 last2,
      Comp comp, Proj1 proj1, Proj2 proj2) :
      it1(std::move(first1)), st1(std::move(last1)),
      it2(std::move(first2)), st2(std::move(last2)),
      comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto next() -> void {
      fwd_to_valid();
    }

    GENEX_INLINE constexpr auto deref() const -> decltype(auto) {
      return *cur_elem;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const set_iterator &self, const set_iterator &that) -> bool {
      return self.it1 == that.it1 and self.it2 == that.it2 and self.cur_elem == that.cur_elem;
    }

    GENEX_INLINE friend constexpr auto operator==(
      const set_iterator &self, set_algorithm_sentinel) -> bool {
      return not self.cur_elem.has_value();
    }

  private:
    GENEX_INLINE constexpr auto fwd_to_valid() -> void {
      cur_elem.reset();

      auto less12 = [&](auto const &a, auto const &b) {
        return meta::invoke(comp, meta::invoke(proj1, a), meta::invoke(proj2, b));
      };
      auto less21 = [&](auto const &a, auto const &b) {
        return meta::invoke(comp, meta::invoke(proj2, b), meta::invoke(proj1, a));
      };

      if constexpr (Op == set_op::difference) {
        while (it1 != st1) {
          if (it2 == st2 or less12(*it1, *it2)) {
            cur_elem = *it1;
            ++it1;
            return;
          }
          if (less21(*it1, *it2)) {
            ++it2;
            continue;
          }
          ++it1;
          ++it2;
        }
        return;
      }

      if constexpr (Op == set_op::intersection) {
        while (it1 != st1 and it2 != st2) {
          if (less12(*it1, *it2)) {
            ++it1;
            continue;
          }
          if (less21(*it1, *it2)) {
            ++it2;
            continue;
          }
          cur_elem = *it1;
          ++it1;
          ++it2;
          return;
        }
        return;
      }

      if constexpr (Op == set_op::symmetric_difference) {
        while (it1 != st1 or it2 != st2) {
          if (it1 == st1) {
            cur_elem = *it2;
            ++it2;
            return;
          }
          if (it2 == st2) {
            cur_elem = *it1;
            ++it1;
            return;
          }
          if (less12(*it1, *it2)) {
            cur_elem = *it1;
            ++it1;
            return;
          }
          if (less21(*it1, *it2)) {
            cur_elem = *it2;
            ++it2;
            return;
          }
          ++it1;
          ++it2;
        }
        return;
      }

      if constexpr (Op == set_op::union_) {
        while (it1 != st1 or it2 != st2) {
          if (it1 == st1) {
            cur_elem = *it2;
            ++it2;
            return;
          }
          if (it2 == st2) {
            cur_elem = *it1;
            ++it1;
            return;
          }
          if (less12(*it1, *it2)) {
            cur_elem = *it1;
            ++it1;
            return;
          }
          if (less21(*it1, *it2)) {
            cur_elem = *it2;
            ++it2;
            return;
          }
          cur_elem = *it1;
          ++it1;
          ++it2;
          return;
        }
        return;
      }
    }
  };

  template <
    set_op Op, typename I1, typename S1, typename I2, typename S2,
    typename Comp, typename Proj1, typename Proj2>
  requires concepts::set_algorithmicable_iters<
    I1, S1, I2, S2, Comp, Proj1, Proj2>
  struct set_algorithm_view : view_base {
    I1 first1;
    S1 last1;
    I2 first2;
    S2 last2;
    GENEX_NO_UNIQUE_ADDRESS Comp comp;
    GENEX_NO_UNIQUE_ADDRESS Proj1 proj1;
    GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;

    GENEX_INLINE constexpr set_algorithm_view(
      I1 f1, S1 l1, I2 f2, S2 l2, Comp c, Proj1 p1, Proj2 p2) noexcept(
      SAFE_MOVE(I1) and SAFE_MOVE(S1) and SAFE_MOVE(I2) and SAFE_MOVE(S2) and
      SAFE_MOVE(Comp) and SAFE_MOVE(Proj1) and SAFE_MOVE(Proj2)) :
      first1(std::move(f1)), last1(std::move(l1)),
      first2(std::move(f2)), last2(std::move(l2)),
      comp(std::move(c)), proj1(std::move(p1)), proj2(std::move(p2)) {
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return set_iterator<Op, I1, S1, I2, S2, Comp, Proj1, Proj2>(
        first1, last1, first2, last2, comp, proj1, proj2);
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return set_algorithm_sentinel();
    }
  };
}

namespace genex::views {
  template <detail::impl::set_op Op>
  struct set_algorithms_base_fn : pipe_closure {
    template <
      typename I1, typename S1, typename I2, typename S2,
      typename Comp = operations::lt, typename Proj1 = meta::identity,
      typename Proj2 = meta::identity>
    requires detail::concepts::set_algorithmicable_iters<
      I1, S1, I2, S2, Comp, Proj1, Proj2>
    GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = {}, Proj1 proj1 = {},
      Proj2 proj2 = {}) const noexcept(
      SAFE_CTOR(
        detail::impl::set_algorithm_view<Op, I1, S1, I2, S2, Comp, Proj1, Proj2>,
        I1, S1, I2, S2, Comp, Proj1, Proj2) and
      SAFE_MOVE(I1) and SAFE_MOVE(S1) and SAFE_MOVE(I2) and SAFE_MOVE(S2) and
      SAFE_MOVE(Comp) and SAFE_MOVE(Proj1) and SAFE_MOVE(Proj2)) {
      return detail::impl::set_algorithm_view<Op, I1, S1, I2, S2, Comp, Proj1, Proj2>(
        std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(comp), std::move(proj1),
        std::move(proj2));
    }

    template <
      typename Rng1, typename Rng2, typename Comp = operations::lt,
      typename Proj1 = meta::identity, typename Proj2 = meta::identity>
    requires detail::concepts::set_algorithmicable_range<
      Rng1, Rng2, Comp, Proj1, Proj2>
    GENEX_INLINE constexpr auto operator()(
      Rng1 &&rng1, Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {},
      Proj2 proj2 = {}) const noexcept(
      SAFE_CALL(decltype(iterators::iter_pair), Rng1&) and
      SAFE_CALL(decltype(iterators::iter_pair), Rng2&) and
      SAFE_CTOR(
        detail::impl::set_algorithm_view<
          Op, iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>,
          sentinel_t<Rng2>, Comp, Proj1, Proj2>,
        iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>,
        Comp, Proj1, Proj2) and
      SAFE_MOVE(Comp) and SAFE_MOVE(Proj1) and SAFE_MOVE(Proj2)) {
      auto [first1, last1] = iterators::iter_pair(rng1);
      auto [first2, last2] = iterators::iter_pair(rng2);
      return detail::impl::set_algorithm_view<
        Op, iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>,
        sentinel_t<Rng2>, Comp, Proj1, Proj2>(
        std::move(first1), std::move(last1), std::move(first2),
        std::move(last2), std::move(comp), std::move(proj1), std::move(proj2));
    }

    template <
      typename Rng2, typename Comp = operations::lt,
      typename Proj1 = meta::identity, typename Proj2 =meta::identity>
    requires (range<Rng2> and not range<Comp>)
    GENEX_INLINE constexpr auto operator()(
      Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {},
      Proj2 proj2 = {}) const {
      return genex::detail::bind_range_back(
        set_algorithms_base_fn{}, std::forward<Rng2>(rng2), std::move(comp),
        std::move(proj1), std::move(proj2));
    }
  };

  using set_difference_fn = set_algorithms_base_fn<
    detail::impl::set_op::difference>;
  using set_intersection_fn = set_algorithms_base_fn<
    detail::impl::set_op::intersection>;
  using set_symmetric_difference_fn = set_algorithms_base_fn<
    detail::impl::set_op::symmetric_difference>;
  using set_union_fn = set_algorithms_base_fn<
    detail::impl::set_op::union_>;

  export inline constexpr set_difference_fn set_difference{};
  export inline constexpr set_intersection_fn set_intersection{};
  export inline constexpr set_symmetric_difference_fn set_symmetric_difference{};
  export inline constexpr set_union_fn set_union{};
}
