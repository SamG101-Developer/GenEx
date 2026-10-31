#include <benchmark/benchmark.h>

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/binary_search.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/algorithm/fold_left.hpp>
#include <range/v3/algorithm/fold_right.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/reverse.hpp>

import genex.algorithms.all_of;
import genex.algorithms.any_of;
import genex.algorithms.binary_search;
import genex.algorithms.contains;
import genex.algorithms.contains_if;
import genex.algorithms.count;
import genex.algorithms.count_if;
import genex.algorithms.equals;
import genex.algorithms.find;
import genex.algorithms.find_if;
import genex.algorithms.find_if_not;
import genex.algorithms.find_last;
import genex.algorithms.find_last_if;
import genex.algorithms.find_last_if_not;
import genex.algorithms.fold_left;
import genex.algorithms.fold_left_first;
import genex.algorithms.fold_right;
import genex.algorithms.fold_right_first;
import genex.algorithms.for_each;
import genex.algorithms.max_element;
import genex.algorithms.min_element;
import genex.algorithms.none_of;
import genex.algorithms.position;
import genex.algorithms.position_last;
import genex.algorithms.sorted;
import std;

namespace {
  constexpr auto size = 1000;

  // Nothing here matches until the very end, so every search looks at every element.
  constexpr auto is_last = [](const int x) { return x == size - 1; };
  constexpr auto is_first = [](const int x) { return x == 0; };
  constexpr auto not_last = [](const int x) { return x != size - 1; };
  constexpr auto not_first = [](const int x) { return x != 0; };
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto never = [](const int x) { return x < 0; };
  constexpr auto always = [](const int x) { return x >= 0; };
  constexpr auto plus = [](const int a, const int b) { return a + b; };

  struct Point {
    int x;
    int y;
  };

  // Vectorized loops can run several times slower or faster depending on how
  // their data is aligned. malloc only promises 16 bytes, so landing on a
  // 32-byte boundary would otherwise be an accident of what was allocated
  // before, which can differ between the sides of a pair and change with any
  // unrelated edit. This puts the data GENEX_BENCH_OFFSET bytes past a 64-byte
  // boundary instead, and check_ratios.py runs each offset that malloc could
  // give. Only the contiguous data is moved, as spacing out the nodes of a list
  // would measure the spacing.
  const auto data_offset = [] {
    const auto *const text = std::getenv("GENEX_BENCH_OFFSET");
    return text != nullptr ? static_cast<std::size_t>(std::atoi(text)) : 0uz;
  }();

  template <typename T>
  struct offset_allocator {
    using value_type = T;

    offset_allocator() = default;

    template <typename U>
    offset_allocator(const offset_allocator<U> &) {
    }

    auto allocate(const std::size_t n) -> T* {
      auto *const base = static_cast<std::byte*>(
        ::operator new(n * sizeof(T) + data_offset, std::align_val_t{64}));
      return reinterpret_cast<T*>(base + data_offset);
    }

    auto deallocate(T *const ptr, std::size_t) -> void {
      ::operator delete(
        reinterpret_cast<std::byte*>(ptr) - data_offset, std::align_val_t{64});
    }

    auto operator==(const offset_allocator &) const -> bool = default;
  };

  template <typename T>
  using offset_vector = std::vector<T, offset_allocator<T>>;

  auto vec() -> offset_vector<int> {
    auto data = offset_vector<int>(size);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  template <int N>
  auto vec_n() -> offset_vector<int> {
    auto data = offset_vector<int>(N);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  template <int N>
  auto vec_pair_n() -> std::pair<offset_vector<int>, offset_vector<int>> {
    return {vec_n<N>(), vec_n<N>()};
  }

  // Only the last byte is the one that's looked for.
  template <int N>
  auto bytes_n() -> offset_vector<char> {
    auto data = offset_vector<char>(N, 'a');
    data.back() = 'b';
    return data;
  }

  auto lst() -> std::list<int> {
    auto data = std::list<int>(size);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  // Two separate ranges, as comparing one to itself lets the compiler skip the comparison altogether.
  auto vec_pair() -> std::pair<offset_vector<int>, offset_vector<int>> {
    return {vec(), vec()};
  }

  auto lst_pair() -> std::pair<std::list<int>, std::list<int>> {
    return {lst(), lst()};
  }

  // Shuffled with a fixed seed, so a sort has real work to do and each run sees the same data.
  auto shuffled() -> offset_vector<int> {
    auto data = vec();
    auto engine = std::mt19937(42);
    std::shuffle(data.begin(), data.end(), engine);
    return data;
  }

  auto shuffled_lst() -> std::list<int> {
    const auto data = shuffled();
    return {data.begin(), data.end()};
  }

  auto points() -> offset_vector<Point> {
    auto data = offset_vector<Point>(size);
    for (auto i = 0; i < size; ++i) { data[static_cast<std::size_t>(i)] = Point{i, -i}; }
    return data;
  }
}

#define GENEX_BENCH_ALGO(Name, Data, RangesExpr, GenexExpr)      \
  static void BM_StdRanges_Algo##Name(benchmark::State &state) { \
    auto data = Data();                                          \
    for (auto _ : state) {                                       \
      benchmark::DoNotOptimize(data);                            \
      auto result = RangesExpr;                                  \
      benchmark::DoNotOptimize(result);                          \
    }                                                            \
  }                                                              \
  BENCHMARK(BM_StdRanges_Algo##Name);                            \
  static void BM_Genex_V2_Algo##Name(benchmark::State &state) {  \
    auto data = Data();                                          \
    for (auto _ : state) {                                       \
      benchmark::DoNotOptimize(data);                            \
      auto result = GenexExpr;                                   \
      benchmark::DoNotOptimize(result);                          \
    }                                                            \
  }                                                              \
  BENCHMARK(BM_Genex_V2_Algo##Name)

GENEX_BENCH_ALGO(AllOf, vec, ranges::all_of(data, always), genex::all_of(data, always));
GENEX_BENCH_ALGO(AnyOf, vec, ranges::any_of(data, never), genex::any_of(data, never));
GENEX_BENCH_ALGO(NoneOf, vec, ranges::none_of(data, never), genex::none_of(data, never));
GENEX_BENCH_ALGO(AllOf_List, lst, ranges::all_of(data, always), genex::all_of(data, always));

GENEX_BENCH_ALGO(BinarySearch, vec, ranges::binary_search(data, size - 1), genex::binary_search(data, size - 1));
GENEX_BENCH_ALGO(Contains, vec, ranges::contains(data, size - 1), genex::contains(data, size - 1));
GENEX_BENCH_ALGO(ContainsIf, vec, ranges::any_of(data, is_last), genex::contains_if(data, is_last));

GENEX_BENCH_ALGO(Count, vec, ranges::count(data, 5), genex::count(data, 5));
GENEX_BENCH_ALGO(CountIf, vec, ranges::count_if(data, is_even), genex::count_if(data, is_even));
GENEX_BENCH_ALGO(Count_List, lst, ranges::count(data, 5), genex::count(data, 5));
GENEX_BENCH_ALGO(CountIf_Proj, points, ranges::count_if(data, is_even, &Point::x), genex::count_if(data, is_even, &Point::x));

GENEX_BENCH_ALGO(Equals, vec_pair, ranges::equal(data.first, data.second), genex::equals(data.first, data.second));
GENEX_BENCH_ALGO(Equals_List, lst_pair, ranges::equal(data.first, data.second), genex::equals(data.first, data.second));

GENEX_BENCH_ALGO(Find, vec, ranges::find(data, size - 1), genex::find(data, size - 1));
GENEX_BENCH_ALGO(FindIf, vec, ranges::find_if(data, is_last), genex::find_if(data, is_last));
GENEX_BENCH_ALGO(FindIfNot, vec, ranges::find_if_not(data, not_last), genex::find_if_not(data, not_last));
GENEX_BENCH_ALGO(Find_List, lst, ranges::find(data, size - 1), genex::find(data, size - 1));
GENEX_BENCH_ALGO(FindIf_Proj, points, ranges::find_if(data, is_last, &Point::x), genex::find_if(data, is_last, &Point::x));

// range-v3 has no find_last, so these search it in reverse, as anyone using it would.
GENEX_BENCH_ALGO(FindLast, vec, ranges::find(data | ranges::views::reverse, 0), genex::find_last(data, 0));
GENEX_BENCH_ALGO(FindLastIf, vec, ranges::find_if(data | ranges::views::reverse, is_first), genex::find_last_if(data, is_first));
GENEX_BENCH_ALGO(FindLastIfNot, vec, ranges::find_if_not(data | ranges::views::reverse, not_first), genex::find_last_if_not(data, not_first));
GENEX_BENCH_ALGO(FindLast_List, lst, ranges::find(data | ranges::views::reverse, 0), genex::find_last(data, 0));

// range-v3 has no position either, so these find and then measure.
GENEX_BENCH_ALGO(Position, vec, ranges::find_if(data, is_last) - data.begin(), genex::position(data, is_last));
GENEX_BENCH_ALGO(PositionLast, vec, ranges::find_if(data | ranges::views::reverse, is_first).base() - data.begin(), genex::position_last(data, is_first));

// Short ranges and bytes, for where calling into libc costs more or gains more than a loop does.
GENEX_BENCH_ALGO(Find_Short4, vec_n<4>, ranges::find(data, 3), genex::find(data, 3));
GENEX_BENCH_ALGO(Find_Short16, vec_n<16>, ranges::find(data, 15), genex::find(data, 15));
GENEX_BENCH_ALGO(Find_Short64, vec_n<64>, ranges::find(data, 63), genex::find(data, 63));
GENEX_BENCH_ALGO(FindByte, bytes_n<1000>, ranges::find(data, 'b'), genex::find(data, 'b'));
GENEX_BENCH_ALGO(FindByte_Short4, bytes_n<4>, ranges::find(data, 'b'), genex::find(data, 'b'));
GENEX_BENCH_ALGO(FindByte_Short16, bytes_n<16>, ranges::find(data, 'b'), genex::find(data, 'b'));
GENEX_BENCH_ALGO(FindByte_Short64, bytes_n<64>, ranges::find(data, 'b'), genex::find(data, 'b'));
GENEX_BENCH_ALGO(Equals_Short4, vec_pair_n<4>, ranges::equal(data.first, data.second), genex::equals(data.first, data.second));
GENEX_BENCH_ALGO(Equals_Short16, vec_pair_n<16>, ranges::equal(data.first, data.second), genex::equals(data.first, data.second));
GENEX_BENCH_ALGO(Equals_Short64, vec_pair_n<64>, ranges::equal(data.first, data.second), genex::equals(data.first, data.second));

GENEX_BENCH_ALGO(FoldLeft, vec, ranges::fold_left(data, 0, plus), genex::fold_left(data, 0, plus));
GENEX_BENCH_ALGO(FoldLeftFirst, vec, ranges::fold_left_first(data, plus), genex::fold_left_first(data, plus));
GENEX_BENCH_ALGO(FoldRight, vec, ranges::fold_right(data, 0, plus), genex::fold_right(data, 0, plus));
GENEX_BENCH_ALGO(FoldRightFirst, vec, ranges::fold_right_last(data, plus), genex::fold_right_first(data, plus));
GENEX_BENCH_ALGO(FoldLeft_List, lst, ranges::fold_left(data, 0, plus), genex::fold_left(data, 0, plus));
GENEX_BENCH_ALGO(FoldRight_List, lst, ranges::fold_right(data, 0, plus), genex::fold_right(data, 0, plus));

GENEX_BENCH_ALGO(MaxElement, vec, *ranges::max_element(data), genex::max_element(data));
GENEX_BENCH_ALGO(MinElement, vec, *ranges::min_element(data), genex::min_element(data));
GENEX_BENCH_ALGO(MaxElement_List, lst, *ranges::max_element(data), genex::max_element(data));
GENEX_BENCH_ALGO(MaxElement_Proj, points, ranges::max_element(data, {}, &Point::x)->x, genex::max_element(data, {}, &Point::x).x);

// genex::sorted returns a sorted copy, so these are the same thing with range-v3: copy, then sort.
GENEX_BENCH_ALGO(Sorted, shuffled, [&] { auto copy = data; ranges::sort(copy); return copy; }(), genex::sorted(data));
GENEX_BENCH_ALGO(
  Sorted_List, shuffled_lst,
  [&] { auto copy = std::vector<int>(data.begin(), data.end()); ranges::sort(copy); return copy; }(),
  genex::sorted(data));

GENEX_BENCH_ALGO(
  ForEach, vec,
  [&] {
    auto sum = 0;
    ranges::for_each(data, [&sum](const int x) { sum += x; });
    return sum;
  }(),
  [&] {
    auto sum = 0;
    data | genex::for_each([&sum](const int x) { sum += x; });
    return sum;
  }());
