#include <gtest/gtest.h>
#include <algorithm>
#include <deque>
#include <array>
#include <forward_list>
#include <functional>
#include <list>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

import genex.actions.partition;
import genex.algorithms.partition;
import genex.algorithms.stable_partition;
import genex.detail.stable_partition_impl;

namespace {
  const auto lengths = std::vector<std::size_t>{0, 1, 2, 3, 4, 5, 8, 16, 33, 100, 1000, 20000};

  auto random_ints(const std::size_t count, const unsigned seed = 1) -> std::vector<int> {
    auto engine = std::mt19937(seed + static_cast<unsigned>(count));
    auto data = std::vector<int>(count);
    for (auto &x : data) { x = static_cast<int>(engine() % 100000); }
    return data;
  }

  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto always = [](const int) { return true; };
  constexpr auto never = [](const int) { return false; };

  template <typename Container, typename Pred>
  auto check_partition(const std::vector<int> &input, Pred pred) -> void {
    auto data = Container(input.begin(), input.end());
    const auto point = genex::partition(data, pred);
    EXPECT_TRUE(std::all_of(data.begin(), point, pred));
    EXPECT_TRUE(std::none_of(point, data.end(), pred));
    auto got = std::vector<int>(data.begin(), data.end());
    auto exp = input;
    std::sort(got.begin(), got.end());
    std::sort(exp.begin(), exp.end());
    EXPECT_EQ(got, exp);
  }

  template <typename Container, typename Pred>
  auto check_stable_partition(const std::vector<int> &input, Pred pred) -> void {
    auto data = Container(input.begin(), input.end());
    const auto point = genex::stable_partition(data, pred);

    auto exp = input;
    const auto exp_point = std::stable_partition(exp.begin(), exp.end(), pred);
    EXPECT_EQ(std::vector<int>(data.begin(), data.end()), exp);
    EXPECT_EQ(std::distance(data.begin(), point), std::distance(exp.begin(), exp_point));
  }
}

TEST(GenexPartition, EachContainer) {
  for (const auto count : lengths) {
    const auto input = random_ints(count);
    for (const auto &run : {std::function<bool(int)>(is_even), std::function<bool(int)>(always), std::function<bool(int)>(never)}) {
      check_partition<std::vector<int>>(input, run);
      check_partition<std::deque<int>>(input, run);
      check_partition<std::list<int>>(input, run);
      check_partition<std::forward_list<int>>(input, run);
    }
  }
}

TEST(GenexPartition, BranchlessPathWithEachPredicate) {
  for (const auto count : lengths) {
    const auto input = random_ints(count);
    check_partition<std::vector<int>>(input, is_even);
    check_partition<std::vector<int>>(input, always);
    check_partition<std::vector<int>>(input, never);
    check_partition<std::vector<int>>(input, [](const int x) { return x > 50000; });
  }
}

TEST(GenexPartition, OtherElementTypes) {
  auto strings = std::vector<std::string>();
  for (const auto x : random_ints(2000)) { strings.push_back(std::to_string(x) + "-padding-to-allocate-it"); }
  const auto starts_even = [](const std::string &s) { return (s[0] - '0') % 2 == 0; };
  const auto point = genex::partition(strings, starts_even);
  EXPECT_TRUE(std::all_of(strings.begin(), point, starts_even));
  EXPECT_TRUE(std::none_of(point, strings.end(), starts_even));

  auto owners = std::vector<std::unique_ptr<int>>();
  for (const auto x : random_ints(2000)) { owners.push_back(std::make_unique<int>(x)); }
  const auto even_owner = [](const std::unique_ptr<int> &p) { return *p % 2 == 0; };
  const auto owner_point = genex::partition(owners, even_owner);
  EXPECT_TRUE(std::all_of(owners.begin(), owner_point, even_owner));
  EXPECT_TRUE(std::none_of(owner_point, owners.end(), even_owner));

  auto bits = std::vector<bool>();
  for (const auto x : random_ints(2000)) { bits.push_back(x % 2 == 0); }
  const auto trues = std::count(bits.begin(), bits.end(), true);
  const auto bit_point = genex::partition(bits, [](const bool b) { return b; });
  EXPECT_EQ(std::distance(bits.begin(), bit_point), trues);
  EXPECT_TRUE(std::all_of(bits.begin(), bit_point, [](const bool b) { return b; }));
}

TEST(GenexPartition, PredicateCalledOncePerElement) {
  for (const auto count : {0uz, 1uz, 2uz, 100uz}) {
    auto data = random_ints(count);
    auto calls = 0uz;
    genex::partition(data, [&calls](const int x) { ++calls; return x % 2 == 0; });
    EXPECT_EQ(calls, count);

    auto stable_data = random_ints(count);
    auto stable_calls = 0uz;
    genex::stable_partition(stable_data, [&stable_calls](const int x) { ++stable_calls; return x % 2 == 0; });
    EXPECT_EQ(stable_calls, count);
  }
}

TEST(GenexPartition, Projection) {
  auto data = std::vector<std::pair<int, int>>{{1, 2}, {2, 3}, {3, 4}, {4, 5}};
  const auto point = genex::partition(data, is_even, &std::pair<int, int>::first);
  EXPECT_EQ(std::distance(data.begin(), point), 2);
  EXPECT_TRUE(std::all_of(data.begin(), point, [](const auto &p) { return p.first % 2 == 0; }));
}

TEST(GenexStablePartition, EachContainer) {
  for (const auto count : lengths) {
    const auto input = random_ints(count);
    check_stable_partition<std::vector<int>>(input, is_even);
    check_stable_partition<std::vector<int>>(input, always);
    check_stable_partition<std::vector<int>>(input, never);
    check_stable_partition<std::deque<int>>(input, is_even);
    check_stable_partition<std::list<int>>(input, is_even);
    check_stable_partition<std::forward_list<int>>(input, is_even);
  }
}

TEST(GenexStablePartition, OtherElementTypes) {
  auto strings = std::vector<std::string>();
  for (const auto x : random_ints(2000)) { strings.push_back(std::to_string(x) + "-padding-to-allocate-it"); }
  const auto starts_even = [](const std::string &s) { return (s[0] - '0') % 2 == 0; };
  auto exp = strings;
  std::stable_partition(exp.begin(), exp.end(), starts_even);
  genex::stable_partition(strings, starts_even);
  EXPECT_EQ(strings, exp);

  auto pairs = std::vector<std::pair<int, int>>();
  for (const auto x : random_ints(2000)) { pairs.emplace_back(x, x + 1); }
  auto expect_pairs = pairs;
  const auto even_first = [](const std::pair<int, int> &p) { return p.first % 2 == 0; };
  std::stable_partition(expect_pairs.begin(), expect_pairs.end(), even_first);
  genex::stable_partition(pairs, even_first);
  EXPECT_EQ(pairs, expect_pairs);

  auto owners = std::vector<std::unique_ptr<int>>();
  for (const auto x : random_ints(2000)) { owners.push_back(std::make_unique<int>(x)); }
  const auto even_owner = [](const std::unique_ptr<int> &p) { return *p % 2 == 0; };
  const auto owner_point = genex::stable_partition(owners, even_owner);
  EXPECT_TRUE(std::all_of(owners.begin(), owner_point, even_owner));
  EXPECT_TRUE(std::none_of(owner_point, owners.end(), even_owner));

  auto bits = std::vector<bool>();
  for (const auto x : random_ints(2000)) { bits.push_back(x % 2 == 0); }
  const auto trues = std::count(bits.begin(), bits.end(), true);
  const auto bit_point = genex::stable_partition(bits, [](const bool b) { return b; });
  EXPECT_EQ(std::distance(bits.begin(), bit_point), trues);
}

TEST(GenexStablePartition, WithoutAnyMemory) {
  for (const auto count : lengths) {
    const auto input = random_ints(count);
    auto data = input;
    auto exp = input;
    auto pred = is_even;
    const auto exp_point = std::stable_partition(exp.begin(), exp.end(), pred);
    const auto point = genex::algorithms::detail::stable_partition_in_place(
      data.begin(), data.end(), pred, static_cast<std::ptrdiff_t>(count));
    EXPECT_EQ(data, exp);
    EXPECT_EQ(std::distance(data.begin(), point), std::distance(exp.begin(), exp_point));

    auto list = std::list<int>(input.begin(), input.end());
    genex::algorithms::detail::stable_partition_in_place(
      list.begin(), list.end(), pred, static_cast<std::ptrdiff_t>(count));
    EXPECT_TRUE(std::equal(list.begin(), list.end(), exp.begin(), exp.end()));
  }
}

TEST(GenexStablePartition, SurvivesThrowingPredicate) {
  for (const auto throw_at : {0, 1, 5, 500, 1999}) {
    auto data = std::vector<std::string>();
    for (const auto x : random_ints(2000)) { data.push_back(std::to_string(x) + "-padding-to-allocate-it"); }
    auto calls = 0;
    EXPECT_THROW(
      genex::stable_partition(data, [&](const std::string &s) {
        if (calls++ == throw_at) { throw std::runtime_error("predicate"); }
        return s.size() % 2 == 0;
      }),
      std::runtime_error);
    EXPECT_EQ(data.size(), 2000uz);
  }
}

static_assert([] {
  auto data = std::array{1, 2, 3, 4, 5, 6, 7};
  const auto point = genex::partition(data, is_even);
  for (auto it = data.begin(); it != point; ++it) { if (*it % 2 != 0) { return false; } }
  for (auto it = point; it != data.end(); ++it) { if (*it % 2 == 0) { return false; } }
  return point - data.begin() == 3;
}());

static_assert([] {
  auto data = std::array{1, 2, 3, 4, 5, 6, 7};
  const auto point = genex::stable_partition(data, is_even);
  return point - data.begin() == 3 and data == std::array{2, 4, 6, 1, 3, 5, 7};
}());

static_assert([] {
  auto data = std::array{std::pair{1, 0}, std::pair{2, 0}, std::pair{3, 0}, std::pair{4, 0}};
  auto strings_like = genex::stable_partition(data, is_even, &std::pair<int, int>::first);
  return strings_like - data.begin() == 2 and data[0].first == 2 and data[1].first == 4 and data[2].first == 1;
}());

TEST(GenexPartition, ThroughTheActions) {
  auto data = random_ints(1000);
  data |= genex::actions::partition(is_even);
  EXPECT_TRUE(std::is_partitioned(data.begin(), data.end(), is_even));

  auto stable = random_ints(1000);
  auto exp = stable;
  std::stable_partition(exp.begin(), exp.end(), is_even);
  stable |= genex::actions::stable_partition(is_even);
  EXPECT_EQ(stable, exp);
}
