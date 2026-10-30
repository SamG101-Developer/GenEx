#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <list>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

import genex.parallel.for_each;
import genex.views.filter;
import genex.views.transform;

using genex::parallel::params;

TEST(GenexParallelForEach, EmptyInput) {
  auto vec = std::vector<int>{};

  genex::parallel::for_each(vec, [](int &x) { ++x; }, params{.threads = 4, .threshold = 0});
  EXPECT_TRUE(vec.empty());
}

TEST(GenexParallelForEach, MutatesInPlace) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6};

  genex::parallel::for_each(vec, [](int &x) { x *= 2; }, params{.threads = 3, .threshold = 0});
  EXPECT_EQ(vec, (std::vector{0, 2, 4, 6, 8, 10, 12}));
}

TEST(GenexParallelForEach, VisitsEveryElementExactlyOnce) {
  for (const auto threads : {1uz, 2uz, 3uz, 4uz, 8uz}) {
    for (const auto size : {1uz, 7uz, 64uz, 1'000uz, 1'001uz}) {
      auto vec = std::vector(size, 0);

      genex::parallel::for_each(vec, [](int &x) { ++x; }, params{.threads = threads, .threshold = 0});
      EXPECT_EQ(vec, std::vector(size, 1)) << "threads=" << threads << " size=" << size;
    }
  }
}

TEST(GenexParallelForEach, SumsThroughAnAtomic) {
  auto vec = std::vector<int>(1'001);
  std::iota(vec.begin(), vec.end(), 0);

  auto total = std::atomic<long long>{0};
  genex::parallel::for_each(vec, [&total](const int x) { total += x; }, params{.threads = 7, .threshold = 0});
  EXPECT_EQ(total.load(), 1'000LL * 1'001 / 2);
}

TEST(GenexParallelForEach, SingleThreadRunsSerially) {
  auto vec = std::vector(1'000, 0);

  genex::parallel::for_each(vec, [](int &x) { ++x; }, params{.threads = 1, .threshold = 0});
  EXPECT_EQ(vec, std::vector(1'000, 1));
}

TEST(GenexParallelForEach, BelowThresholdStaysOnTheCallingThread) {
  auto vec = std::vector(100, 0);

  const auto self = std::this_thread::get_id();
  auto off_thread = std::atomic{false};
  genex::parallel::for_each(vec, [&](int &x) {
    ++x;
    if (std::this_thread::get_id() != self) { off_thread = true; }
  }, params{.threads = 4});

  EXPECT_FALSE(off_thread.load());
  EXPECT_EQ(vec, std::vector(100, 1));
}

TEST(GenexParallelForEach, MoreThreadsThanElements) {
  auto vec = std::vector{0, 1, 2};

  genex::parallel::for_each(vec, [](int &x) { ++x; }, params{.threads = 64, .threshold = 0});
  EXPECT_EQ(vec, (std::vector{1, 2, 3}));
}

TEST(GenexParallelForEach, ActuallyUsesMultipleThreads) {
  auto vec = std::vector(1 << 16, 0);

  auto ids = std::vector<std::thread::id>(vec.size());
  genex::parallel::for_each(vec, [&](int const &x) {
    ids[static_cast<std::size_t>(&x - vec.data())] = std::this_thread::get_id();
  }, params{.threads = 4, .threshold = 0});

  auto distinct = std::vector<std::thread::id>{};
  for (const auto id : ids) {
    if (std::find(distinct.begin(), distinct.end(), id) == distinct.end()) { distinct.push_back(id); }
  }
  EXPECT_EQ(distinct.size(), 4uz);
}

TEST(GenexParallelForEach, IterInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5};

  genex::parallel::for_each(vec.begin(), vec.end(), [](int &x) { ++x; }, params{.threads = 2, .threshold = 0});
  EXPECT_EQ(vec, (std::vector{1, 2, 3, 4, 5, 6}));
}

TEST(GenexParallelForEach, PipeInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5};

  vec | genex::parallel::for_each([](int &x) { ++x; }, params{.threads = 2, .threshold = 0});
  EXPECT_EQ(vec, (std::vector{1, 2, 3, 4, 5, 6}));
}

TEST(GenexParallelForEach, PipeInputDefaultParams) {
  auto vec = std::vector{0, 1, 2, 3};

  vec | genex::parallel::for_each([](int &x) { ++x; });
  EXPECT_EQ(vec, (std::vector{1, 2, 3, 4}));
}

TEST(GenexParallelForEach, TransformViewInput) {
  auto vec = std::vector<int>(1'000);
  std::iota(vec.begin(), vec.end(), 0);

  auto total = std::atomic<long long>{0};
  vec
    | genex::views::transform([](const int x) { return x * 2; })
    | genex::parallel::for_each([&total](const int x) { total += x; }, params{.threads = 4, .threshold = 0});

  EXPECT_EQ(total.load(), 2 * (999LL * 1'000 / 2));
}

TEST(GenexParallelForEach, ExceptionPropagates) {
  auto vec = std::vector(1'000, 0);

  EXPECT_THROW(
    genex::parallel::for_each(vec, [](int &x) {
      ++x;
      throw std::runtime_error("boom");
      }, params{.threads = 4, .threshold = 0}),
    std::runtime_error);
}

TEST(GenexParallelForEach, ExceptionStillJoinsEveryChunk) {
  auto vec = std::vector(1'000, 0);

  // Every chunk is attempted, so each of the 4 chunks marks exactly its own first element before throwing.
  auto visited = std::atomic{0};
  try {
    genex::parallel::for_each(vec, [&visited](int &) {
      ++visited;
      throw std::runtime_error("boom");
    }, params{.threads = 4, .threshold = 0});
  }
  catch (const std::runtime_error &) {
  }
  EXPECT_EQ(visited.load(), 4);
}

// Ranges that cannot be split in O(1) are rejected outright rather than silently falling back to a serial walk.
// The check goes through a concept rather than a bare `static_assert(not requires ...)`: a requires-expression
// outside a template is evaluated eagerly, so the failed overload resolution would be a hard error instead of an
// unsatisfied requirement.
namespace {
  struct noop_ref {
    auto operator()(int &) const -> void {
    }
  };

  struct noop_val {
    auto operator()(int) const -> void {
    }
  };

  struct positive {
    auto operator()(const int x) const -> bool { return x > 0; }
  };

  template <typename Rng, typename F>
  concept par_for_eachable = requires(Rng &rng, F f)
  {
    genex::parallel::for_each(rng, f);
  };

  using filtered = decltype(std::declval<std::vector<int>&>() | genex::views::filter(positive{}));
  using transformed = decltype(std::declval<std::vector<int>&>() | genex::views::transform(positive{}));
}

static_assert(par_for_eachable<std::vector<int>, noop_ref>);
static_assert(par_for_eachable<transformed, noop_val>);
static_assert(not par_for_eachable<std::list<int>, noop_ref>);
static_assert(not par_for_eachable<filtered, noop_val>);
