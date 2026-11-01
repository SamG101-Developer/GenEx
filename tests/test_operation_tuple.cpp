#include <gtest/gtest.h>
#include <concepts>
#include <string>
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

TEST(GenexTupleHead, OfArguments) {
  EXPECT_EQ(genex::tuple_head(1, 2, 3), 1);
  EXPECT_EQ(genex::tuple_head(std::string("a")), "a");

  auto first = 1;
  auto second = 2;
  EXPECT_EQ(&genex::tuple_head(first, second), &first);
}

TEST(GenexTupleHead, OfATuple) {
  const auto constant = std::tuple{1, 2L};
  EXPECT_EQ(genex::tuple_head(constant), 1);

  auto mutable_tuple = std::tuple{3, 4L};
  EXPECT_EQ(genex::tuple_head(mutable_tuple), 3);

  EXPECT_EQ(genex::tuple_head(std::tuple{5, 6L}), 5);
}

static_assert(std::same_as<decltype(genex::tuple_head(std::declval<const int_tuple&>())), const int&>);
static_assert(std::same_as<decltype(genex::tuple_head(std::declval<int_tuple&>())), int&>);
static_assert(std::same_as<decltype(genex::tuple_head(std::declval<int_tuple>())), int&&>);

TEST(GenexTupleTail, OfArguments) {
  EXPECT_EQ(genex::tuple_tail(1, 2L, 'c'), (std::tuple{2L, 'c'}));
  EXPECT_EQ(genex::tuple_tail(1, 2L), (std::tuple{2L}));
}

TEST(GenexTupleTail, OfATuple) {
  const auto constant = std::tuple{1, 2L, 'c'};
  EXPECT_EQ(genex::tuple_tail(constant), (std::tuple{2L, 'c'}));

  auto mutable_tuple = std::tuple{1, 2L};
  EXPECT_EQ(genex::tuple_tail(mutable_tuple), (std::tuple{2L}));

  EXPECT_EQ(genex::tuple_tail(std::tuple{1, std::string("a")}), (std::tuple{std::string("a")}));
}

TEST(GenexTupleTail, OfASingleElementTupleIsEmpty) {
  const auto constant = std::tuple{1};
  EXPECT_EQ(genex::tuple_tail(constant), std::tuple<>{});
  EXPECT_EQ(genex::tuple_tail(std::tuple{1}), std::tuple<>{});
}
