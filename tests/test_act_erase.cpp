#include <gtest/gtest.h>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

import genex.actions.erase;

TEST(GenexActionsErase, OneElement) {
  auto vec = std::vector{1, 2, 3};
  const auto next = genex::actions::erase(vec, vec.begin() + 1);
  EXPECT_EQ(vec, (std::vector{1, 3}));
  EXPECT_EQ(*next, 3);
}

TEST(GenexActionsErase, TheLastElementGivesTheEnd) {
  auto vec = std::vector{1, 2};
  const auto next = genex::actions::erase(vec, vec.begin() + 1);
  EXPECT_EQ(next, vec.end());
}

TEST(GenexActionsErase, ARange) {
  auto vec = std::vector{1, 2, 3, 4};
  const auto next = genex::actions::erase(vec, vec.begin() + 1, vec.begin() + 3);
  EXPECT_EQ(vec, (std::vector{1, 4}));
  EXPECT_EQ(*next, 4);
}

TEST(GenexActionsErase, AnEmptyRangeChangesNothing) {
  auto vec = std::vector{1, 2};
  static_cast<void>(genex::actions::erase(vec, vec.begin(), vec.begin()));
  EXPECT_EQ(vec, (std::vector{1, 2}));
}

TEST(GenexActionsErase, Everything) {
  auto vec = std::vector{1, 2};
  static_cast<void>(genex::actions::erase(vec, vec.begin(), vec.end()));
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsErase, OtherContainers) {
  auto list = std::list{1, 2, 3};
  static_cast<void>(genex::actions::erase(list, std::next(list.begin())));
  EXPECT_EQ(list, (std::list{1, 3}));

  auto set = std::set{1, 2, 3};
  static_cast<void>(genex::actions::erase(set, set.begin(), std::next(set.begin(), 2)));
  EXPECT_EQ(set, (std::set{3}));

  auto map = std::map<int, std::string>{{1, "a"}, {2, "b"}};
  static_cast<void>(genex::actions::erase(map, map.begin()));
  EXPECT_EQ(map, (std::map<int, std::string>{{2, "b"}}));
}

template <typename Rng, typename I>
concept erasable = requires(Rng &rng, I it) { genex::actions::erase(rng, it); };

// The position has to belong to that kind of container.
static_assert(erasable<std::vector<int>, std::vector<int>::iterator>);
static_assert(not erasable<std::vector<int>, std::list<int>::iterator>);
