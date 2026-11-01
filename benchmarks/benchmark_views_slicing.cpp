#include "view_benchmarks.hpp"

#include <iterator>
#include <span>

#include <range/v3/to_container.hpp>
#include <range/v3/view/any_view.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/chunk_by.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/drop_last.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/take_last.hpp>
#include <range/v3/view/take_while.hpp>

import genex.to_container;
import genex.views.chunk;
import genex.views.chunk_by;
import genex.views.divide;
import genex.views.drop;
import genex.views.drop_last;
import genex.views.drop_while;
import genex.views.pairwise;
import genex.views.slice;
import genex.views.slide;
import genex.views.split;
import genex.views.stride;
import genex.views.take;
import genex.views.take_last;
import genex.views.take_while;
import genex.views.view;

namespace {
  constexpr auto below_half = [](const int x) { return x < 500; };

  auto with_separators() -> std::vector<int> {
    auto data = bench::ascending();
    for (auto i = 0uz; i < data.size() / 100; i += 10) { data[i] = 1; }
    return data;
  }

  // ranges-v3 has no divide view, so the baselines are what it does written out by hand.
  auto divide_by_hand(std::vector<int> &data) -> std::array<std::span<int>, 4> {
    const auto n = data.size() / 4;
    const auto rem = data.size() % 4;
    auto result = std::array<std::span<int>, 4>();
    auto offset = 0uz;
    for (auto i = 0uz; i < 4; ++i) {
      const auto len = n + (i < rem ? 1 : 0);
      result[i] = std::span<int>(data.data() + offset, len);
      offset += len;
    }
    return result;
  }

  auto divide_by_hand(std::list<int> &data) -> std::array<std::pair<std::list<int>::iterator, std::list<int>::iterator>, 4> {
    const auto n = data.size() / 4;
    const auto rem = data.size() % 4;
    auto result = std::array<std::pair<std::list<int>::iterator, std::list<int>::iterator>, 4>();
    auto it = data.begin();
    for (auto i = 0uz; i < 4; ++i) {
      const auto len = n + (i < rem ? 1 : 0);
      auto lo = it;
      std::advance(it, len);
      result[i] = {lo, it};
    }
    return result;
  }
}

GENEX_BENCH_VIEW(Take_RandomAccess, bench::ascending, ranges::views::take(500), genex::views::take(500));
GENEX_BENCH_VIEW(Take_Forward, bench::ascending_list, ranges::views::take(500), genex::views::take(500));
GENEX_BENCH_VIEW(TakeLast_RandomAccess, bench::ascending, ranges::views::take_last(500), genex::views::take_last(500));
GENEX_BENCH_VIEW(TakeWhile, bench::ascending, ranges::views::take_while(below_half), genex::views::take_while(below_half));

GENEX_BENCH_VIEW(Drop_RandomAccess, bench::ascending, ranges::views::drop(500), genex::views::drop(500));
GENEX_BENCH_VIEW(Drop_Forward, bench::ascending_list, ranges::views::drop(500), genex::views::drop(500));
GENEX_BENCH_VIEW(DropLast_RandomAccess, bench::ascending, ranges::views::drop_last(500), genex::views::drop_last(500));
GENEX_BENCH_VIEW(DropLast_Bidirectional, bench::ascending_list, ranges::views::drop_last(500), genex::views::drop_last(500));
GENEX_BENCH_VIEW(DropLast_Forward, bench::ascending_forward_list, ranges::views::drop_last(500), genex::views::drop_last(500));
GENEX_BENCH_VIEW(DropWhile, bench::ascending, ranges::views::drop_while(below_half), genex::views::drop_while(below_half));

GENEX_BENCH_VIEW(Slice_RandomAccess, bench::ascending, ranges::views::slice(200, 800), genex::views::slice(200, 800));
GENEX_BENCH_VIEW(Slice_Forward, bench::ascending_list, ranges::views::slice(200, 800), genex::views::slice(200, 800));

GENEX_BENCH_VIEW(Stride, bench::ascending, ranges::views::stride(3), genex::views::stride(3));
GENEX_BENCH_VIEW(Chunk, bench::ascending, ranges::views::chunk(10), genex::views::chunk(10));

GENEX_BENCH_VIEW(
  ChunkBy, bench::runs_of_ten,
  ranges::views::chunk_by([](const int a, const int b) { return a == b; }),
  genex::views::chunk_by([](const int a, const int b) { return a == b; }));

GENEX_BENCH_VIEW(Slide, bench::ascending, ranges::views::sliding(10), genex::views::slide(10));

// ranges-v3 has no pairwise view, so the closest equivalent -- a sliding window of two -- is the
// baseline. It yields subranges rather than pairs, which is the one shape difference here.
GENEX_BENCH_VIEW(Pairwise, bench::ascending, ranges::views::sliding(2), genex::views::pairwise);

GENEX_BENCH_VIEW(Split, with_separators, ranges::views::split(1), genex::views::split(1));

GENEX_BENCH_EXPR(
  View, bench::ascending,
  ranges::any_view<int>(data) | ranges::to<std::vector>(),
  data | genex::views::view | genex::to<std::vector>());

GENEX_BENCH_EXPR(Divide_RandomAccess, bench::ascending, divide_by_hand(data), data | genex::views::divide<4>);
GENEX_BENCH_EXPR(Divide_Forward, bench::ascending_list, divide_by_hand(data), data | genex::views::divide<4>);
