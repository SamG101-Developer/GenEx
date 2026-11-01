#include <gtest/gtest.h>
#include <concepts>
#include <tuple>
#include <utility>

import genex.operations.tuple;

namespace {
  using int_pair = std::pair<int, long>;
  using int_tuple = std::tuple<int, long>;
  using ref_pair = std::pair<int&, long>;
}

// The value category of the argument carries through, as it does for std::get.
static_assert(std::same_as<decltype(genex::get<0>(std::declval<int_pair&>())), int&>);
static_assert(std::same_as<decltype(genex::get<1>(std::declval<int_pair&>())), long&>);
static_assert(std::same_as<decltype(genex::get<0>(std::declval<const int_pair&>())), const int&>);
static_assert(std::same_as<decltype(genex::get<1>(std::declval<const int_pair&>())), const long&>);
static_assert(std::same_as<decltype(genex::get<0>(std::declval<int_pair>())), int&&>);
static_assert(std::same_as<decltype(genex::get<1>(std::declval<int_pair>())), long&&>);

static_assert(std::same_as<decltype(genex::get<0>(std::declval<int_tuple&>())), int&>);
static_assert(std::same_as<decltype(genex::get<0>(std::declval<const int_tuple&>())), const int&>);
static_assert(std::same_as<decltype(genex::get<0>(std::declval<int_tuple>())), int&&>);

// A const pair of a reference still refers to something that isn't const.
static_assert(std::same_as<decltype(genex::get<0>(std::declval<const ref_pair&>())), int&>);

TEST(GenexOperationsGet, PairLvalueRefersToTheElement) {
  auto pair = int_pair{1, 2};
  EXPECT_EQ(&genex::get<0>(pair), &pair.first);
  EXPECT_EQ(&genex::get<1>(pair), &pair.second);
}

TEST(GenexOperationsGet, ConstPairLvalueRefersToTheElement) {
  const auto pair = int_pair{1, 2};
  EXPECT_EQ(&genex::get<0>(pair), &pair.first);
  EXPECT_EQ(&genex::get<1>(pair), &pair.second);
}

TEST(GenexOperationsGet, WritesThroughAPairLvalue) {
  auto pair = int_pair{1, 2};
  genex::get<0>(pair) = 10;
  genex::get<1>(pair) = 20;
  EXPECT_EQ(pair, (int_pair{10, 20}));
}

TEST(GenexOperationsGet, WritesThroughATupleLvalue) {
  auto tuple = int_tuple{1, 2};
  genex::get<0>(tuple) = 10;
  EXPECT_EQ(tuple, (int_tuple{10, 2}));
}
