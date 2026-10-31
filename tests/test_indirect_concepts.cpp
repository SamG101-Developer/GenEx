#include <gtest/gtest.h>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

import genex.concepts;
import genex.meta;
import genex.operations.cmp;
import genex.views.filter;
import genex.views.transform;

// These replace the standard's concepts, so they ought to give the same answers.
namespace {
  using id = genex::meta::identity;
  using vec_it = std::vector<int>::iterator;
  using list_it = std::list<int>::iterator;
  using set_it = std::set<int>::iterator;
  using bool_it = std::vector<bool>::iterator;
  using unique_it = std::vector<std::unique_ptr<int>>::iterator;
  using str_it = std::vector<std::string>::iterator;
  using pair_it = std::vector<std::pair<int, int>>::iterator;

  auto is_even(int x) -> bool;

  struct holder {
    int field;
    auto get() const -> int;
  };

  using holder_it = std::vector<holder>::iterator;

  constexpr auto pred = [](const int) { return true; };
  constexpr auto mutable_pred = [count = 0](const int) mutable { return ++count > 0; };
  constexpr auto wants_string = [](const std::string &) { return true; };
  constexpr auto returns_void = [](const int) {};
  constexpr auto returns_struct = [](const int) { return holder{}; };
  constexpr auto two_ints = [](const int, const int) { return true; };
  constexpr auto twice = [](const int x) { return x * 2; };
  constexpr auto rvalue_only = [](int &&x) { return x; };
  constexpr auto to_string_view = [](const std::string &s) { return s.size(); };

  struct uncopyable_pred {
    uncopyable_pred(const uncopyable_pred&) = delete;
    uncopyable_pred(uncopyable_pred&&) = default;
    auto operator()(int) const -> bool;
  };

  struct no_const_call {
    auto operator()(int) -> bool;
  };

  struct cannot_assign {
    const int x;
  };

  using cannot_assign_it = std::vector<cannot_assign>::iterator;

  using filter_it = genex::iterator_t<decltype(genex::views::filter(std::declval<std::vector<int>&>(), pred))>;
  using transform_it = genex::iterator_t<decltype(genex::views::transform(std::declval<std::vector<int>&>(), twice))>;
}

#define SAME_PREDICATE(F, I, Proj)                                                       \
  static_assert(                                                                         \
    genex::indirect_unary_predicate<F, genex::projected<I, Proj>> ==                     \
    std::indirect_unary_predicate<F, std::projected<I, Proj>>)

SAME_PREDICATE(decltype(pred), vec_it, id);
SAME_PREDICATE(decltype(pred), list_it, id);
SAME_PREDICATE(decltype(pred), set_it, id);
SAME_PREDICATE(decltype(pred), bool_it, id);
SAME_PREDICATE(decltype(pred), filter_it, id);
SAME_PREDICATE(decltype(pred), transform_it, id);
SAME_PREDICATE(decltype(pred), holder_it, decltype(&holder::field));
SAME_PREDICATE(decltype(pred), holder_it, decltype(&holder::get));
SAME_PREDICATE(decltype(pred), vec_it, decltype(twice));
SAME_PREDICATE(decltype(&is_even), vec_it, id);
SAME_PREDICATE(decltype(mutable_pred), vec_it, id);
SAME_PREDICATE(decltype(wants_string), vec_it, id);
SAME_PREDICATE(decltype(returns_void), vec_it, id);
SAME_PREDICATE(decltype(returns_struct), vec_it, id);
SAME_PREDICATE(decltype(rvalue_only), vec_it, id);
SAME_PREDICATE(decltype(two_ints), vec_it, id);
SAME_PREDICATE(uncopyable_pred, vec_it, id);
SAME_PREDICATE(no_const_call, vec_it, id);
SAME_PREDICATE(decltype(to_string_view), str_it, id);

// Spot checks on what those comparisons should have come out as.
static_assert(genex::indirect_unary_predicate<decltype(pred), genex::projected<vec_it, id>>);
static_assert(genex::indirect_unary_predicate<decltype(pred), genex::projected<holder_it, decltype(&holder::field)>>);
static_assert(genex::indirect_unary_predicate<no_const_call, vec_it>);
static_assert(not genex::indirect_unary_predicate<uncopyable_pred, vec_it>);
static_assert(not genex::indirect_unary_predicate<decltype(returns_void), vec_it>);
static_assert(not genex::indirect_unary_predicate<decltype(wants_string), vec_it>);

#define SAME_INVOCABLE(F, I)                                                             \
  static_assert(genex::indirectly_unary_invocable<F, I> == std::indirectly_unary_invocable<F, I>)

SAME_INVOCABLE(decltype(twice), vec_it);
SAME_INVOCABLE(decltype(twice), bool_it);
SAME_INVOCABLE(decltype(twice), filter_it);
SAME_INVOCABLE(decltype(twice), transform_it);
SAME_INVOCABLE(decltype(rvalue_only), vec_it);
SAME_INVOCABLE(decltype(two_ints), vec_it);
SAME_INVOCABLE(decltype(wants_string), vec_it);
SAME_INVOCABLE(decltype(&holder::get), holder_it);
SAME_INVOCABLE(decltype(&holder::field), holder_it);
SAME_INVOCABLE(uncopyable_pred, vec_it);
static_assert(
  genex::indirectly_regular_unary_invocable<decltype(twice), vec_it> ==
  std::indirectly_regular_unary_invocable<decltype(twice), vec_it>);

#define SAME_COMPARABLE(I1, I2, Comp, Proj1, Proj2)                                      \
  static_assert(                                                                         \
    genex::indirectly_comparable<I1, I2, Comp, Proj1, Proj2> ==                          \
    std::indirectly_comparable<I1, I2, Comp, Proj1, Proj2>)

SAME_COMPARABLE(vec_it, vec_it, genex::operations::eq, id, id);
SAME_COMPARABLE(vec_it, list_it, genex::operations::eq, id, id);
SAME_COMPARABLE(vec_it, const int*, genex::operations::eq, id, id);
SAME_COMPARABLE(vec_it, vec_it, std::less<>, id, id);
SAME_COMPARABLE(vec_it, vec_it, decltype(two_ints), id, id);
SAME_COMPARABLE(vec_it, vec_it, decltype(pred), id, id);
SAME_COMPARABLE(vec_it, str_it, genex::operations::eq, id, id);
SAME_COMPARABLE(holder_it, vec_it, genex::operations::eq, decltype(&holder::field), id);
SAME_COMPARABLE(unique_it, unique_it, genex::operations::eq, id, id);

#define SAME_RELATION(Concept, F, I1, I2)                                                \
  static_assert(genex::Concept<F, I1, I2> == std::Concept<F, I1, I2>)

SAME_RELATION(indirect_binary_predicate, decltype(two_ints), vec_it, list_it);
SAME_RELATION(indirect_binary_predicate, decltype(pred), vec_it, vec_it);
SAME_RELATION(indirect_equivalence_relation, genex::operations::eq, vec_it, const int*);
SAME_RELATION(indirect_equivalence_relation, genex::operations::eq, str_it, const char* const*);
SAME_RELATION(indirect_equivalence_relation, genex::operations::eq, vec_it, const std::string*);
SAME_RELATION(indirect_strict_weak_order, std::less<>, vec_it, vec_it);
SAME_RELATION(indirect_strict_weak_order, std::less<>, vec_it, const int*);
SAME_RELATION(indirect_strict_weak_order, std::less<>, vec_it, str_it);
SAME_RELATION(indirect_strict_weak_order, genex::operations::lt, vec_it, vec_it);

#define SAME_PERMUTABLE(I) static_assert(genex::permutable<I> == std::permutable<I>)

SAME_PERMUTABLE(vec_it);
SAME_PERMUTABLE(list_it);
SAME_PERMUTABLE(set_it);
SAME_PERMUTABLE(bool_it);
SAME_PERMUTABLE(unique_it);
SAME_PERMUTABLE(pair_it);
SAME_PERMUTABLE(cannot_assign_it);
SAME_PERMUTABLE(filter_it);
SAME_PERMUTABLE(transform_it);
SAME_PERMUTABLE(const int*);
SAME_PERMUTABLE(int*);
static_assert(genex::permutable<vec_it>);
static_assert(not genex::permutable<set_it>);
static_assert(not genex::permutable<cannot_assign_it>);

#define SAME_SORTABLE(I, Comp, Proj) \
  static_assert(genex::sortable<I, Comp, Proj> == std::sortable<I, Comp, Proj>)

SAME_SORTABLE(vec_it, std::less<>, id);
SAME_SORTABLE(list_it, std::less<>, id);
SAME_SORTABLE(set_it, std::less<>, id);
SAME_SORTABLE(unique_it, std::less<>, id);
SAME_SORTABLE(holder_it, std::less<>, decltype(&holder::field));
SAME_SORTABLE(holder_it, std::less<>, id);
SAME_SORTABLE(vec_it, decltype(pred), id);
SAME_SORTABLE(vec_it, decltype(two_ints), id);
static_assert(genex::sortable<vec_it, std::less<>, id>);
static_assert(not genex::sortable<holder_it, std::less<>, id>);

// A projection that can't take what the iterator reads isn't one, rather than being a hard error.
template <typename I, typename Proj>
concept can_project = requires { typename genex::projected<I, Proj>; };

static_assert(can_project<vec_it, id>);
static_assert(can_project<holder_it, decltype(&holder::field)>);
static_assert(not can_project<vec_it, decltype(wants_string)>);
static_assert(not can_project<vec_it, decltype(two_ints)>);

using projected_twice = genex::projected<vec_it, decltype(twice)>;
static_assert(std::same_as<genex::iter_value_t<projected_twice>, int>);
static_assert(std::same_as<genex::iter_reference_t<projected_twice>, int>);
using projected_field = genex::projected<holder_it, decltype(&holder::field)>;
static_assert(std::same_as<genex::iter_value_t<projected_field>, int>);
static_assert(std::same_as<genex::iter_reference_t<projected_field>, int&>);

// Lookup through a projected iterator mustn't need to complete what it was made from.
namespace {
  struct incomplete;

  template <typename T>
  struct would_not_compile {
    T value;
  };

  struct takes_anything {
    template <typename T>
    auto operator()(T &&t) const -> T&&;
  };
}

static_assert(requires(genex::projected<would_not_compile<incomplete>**, takes_anything> p) { *p; });
