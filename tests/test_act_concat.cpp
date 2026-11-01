#include <gtest/gtest.h>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

import genex.actions.concat;
import genex.views.filter;

TEST(GenexActionsConcat, VecTwo) {
  auto vec1 = std::vector{0, 1, 2, 3, 4};
  auto vec2 = std::vector{5, 6, 7, 8, 9};
  vec1 |= genex::actions::concat(vec2);
  const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(vec1, exp);
}

TEST(GenexActionsConcat, VecThree) {
  auto vec1 = std::vector{0, 1, 2};
  auto vec2 = std::vector{3, 4, 5};
  auto vec3 = std::vector{6, 7, 8, 9};
  vec1 |= genex::actions::concat(vec2);
  vec1 |= genex::actions::concat(vec3);
  const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(vec1, exp);
}

TEST(GenexActionsConcat, IntoASet) {
  auto set = std::set{1, 2};
  auto vec = std::vector{3, 2, 4};
  set |= genex::actions::concat(vec);
  EXPECT_EQ(set, (std::set{1, 2, 3, 4}));
  EXPECT_EQ(vec, (std::vector{3, 2, 4}));
}

TEST(GenexActionsConcat, IntoASetFromAnRvalue) {
  auto set = std::set<std::string>{"a"};
  set |= genex::actions::concat(std::vector<std::string>{"b", "a"});
  EXPECT_EQ(set, (std::set<std::string>{"a", "b"}));
}

TEST(GenexActionsConcat, IntoASetFromALazyRange) {
  auto set = std::set{1};
  auto vec = std::vector{2, 3, 4};
  set |= genex::actions::concat(vec | genex::views::filter([](const int x) { return x % 2 == 0; }));
  EXPECT_EQ(set, (std::set{1, 2, 4}));
}

TEST(GenexActionsConcat, IntoOtherAssociativeContainers) {
  auto vec = std::vector{1, 1, 2};

  auto multiset = std::multiset{1};
  multiset |= genex::actions::concat(vec);
  EXPECT_EQ(multiset, (std::multiset{1, 1, 1, 2}));

  auto unordered = std::unordered_set{1};
  unordered |= genex::actions::concat(vec);
  EXPECT_EQ(unordered, (std::unordered_set{1, 2}));

  auto map = std::map<int, std::string>{{1, "a"}};
  auto pairs = std::vector<std::pair<int, std::string>>{{1, "ignored"}, {2, "b"}};
  map |= genex::actions::concat(pairs);
  EXPECT_EQ(map, (std::map<int, std::string>{{1, "a"}, {2, "b"}}));
}

TEST(GenexActionsConcat, FromASetIntoAVector) {
  auto vec = std::vector{9};
  const auto set = std::set{1, 2};
  vec |= genex::actions::concat(set);
  EXPECT_EQ(vec, (std::vector{9, 1, 2}));
}

TEST(GenexActionsConcat, SeveralAtOnceIntoASet) {
  auto set = std::set{1};
  auto vec = std::vector{2};
  auto list = std::list{3};
  static_cast<void>(genex::actions::concat(set, vec, list));
  EXPECT_EQ(set, (std::set{1, 2, 3}));
}

TEST(GenexActionsConcat, MovesElementsOutOfAnRvalue) {
  auto list = std::list<std::unique_ptr<int>>();
  auto source = std::vector<std::unique_ptr<int>>();
  source.push_back(std::make_unique<int>(1));
  static_cast<void>(genex::actions::concat(list, std::move(source)));
  ASSERT_EQ(list.size(), 1uz);
  EXPECT_EQ(*list.front(), 1);
}

namespace {
  template <typename Rng1, typename Rng2>
  concept has_range_insert = requires(Rng1 &rng1, Rng2 &rng2) {
    rng1.insert(rng1.end(), rng2.begin(), rng2.end());
  };
}

// The associative containers have no way to insert a range at a position, which is what sends them one at a time.
static_assert(has_range_insert<std::vector<int>, std::vector<int>>);
static_assert(has_range_insert<std::list<int>, std::vector<int>>);
static_assert(not has_range_insert<std::set<int>, std::vector<int>>);
static_assert(not has_range_insert<std::map<int, int>, std::vector<std::pair<int, int>>>);
