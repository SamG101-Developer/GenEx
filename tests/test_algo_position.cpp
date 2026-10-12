#include <coroutine>
#include <gtest/gtest.h>

import genex.algorithms.position;
import genex.algorithms.position_last;
import std;

TEST(GenexAlgosPosition, ElementExists) {
  auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

  const auto res = genex::position(vec, [](const int v) { return v == 1; });
  EXPECT_EQ(res, 3uz);
}

TEST(GenexAlgosPosition, ElementNotExists) {
  auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

  const auto res = genex::position(vec, [](const int v) { return v == 10; });
  EXPECT_EQ(res, std::nullopt);
}

TEST(GenexAlgosPosition, ElementExistsWithDrop) {
  auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

  const auto res = genex::position(vec, [](const int v) { return v == 1; }, {}, 100);
  EXPECT_EQ(res, 103uz);
}

TEST(GenexAlgosPositionLast, ElementExists) {
  auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6, 1};

  const auto res = genex::position_last(vec, [](const int v) { return v == 1; });
  EXPECT_EQ(res, 8uz);
}

TEST(GenexAlgosPositionLast, ElementExistsForward) {
  auto lst = std::forward_list{5, 3, 8, 1, 4, 7, 2, 6, 1};

  const auto res = genex::position_last(lst, [](const int v) { return v == 1; });
  EXPECT_EQ(res, 8uz);
}

TEST(GenexAlgosPositionLast, ElementNotExists) {
  auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

  const auto res = genex::position_last(vec, [](const int v) { return v == 10; });
  EXPECT_EQ(res, std::nullopt);
}
