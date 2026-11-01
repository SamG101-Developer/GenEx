#include "view_benchmarks.hpp"

#include <algorithm>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/remove.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/unique.hpp>

import genex.to_container;
import genex.views.duplicates;
import genex.views.filter;
import genex.views.in;
import genex.views.remove;
import genex.views.remove_if;
import genex.views.unique;

namespace {
  auto data_and_other() -> std::pair<std::vector<int>, std::vector<int>> {
    return {bench::ascending(), bench::ascending_n(100)};
  }

  // ranges-v3 has no duplicates view, so the baseline is the hand-rolled scan-against-seen loop that
  // the view replaces.
  auto duplicates_by_hand(const std::vector<int> &data) -> std::vector<int> {
    auto seen = std::vector<int>();
    auto result = std::vector<int>();
    for (const auto x : data) {
      if (std::find(seen.begin(), seen.end(), x) != seen.end()) { result.push_back(x); }
      else { seen.push_back(x); }
    }
    return result;
  }
}

GENEX_BENCH_VIEW(
  Filter, bench::ascending,
  ranges::views::filter([](const int x) { return x % 2 == 0; }),
  genex::views::filter([](const int x) { return x % 2 == 0; }));

GENEX_BENCH_VIEW(Remove, bench::alternating, ranges::views::remove(0), genex::views::remove(0));

GENEX_BENCH_VIEW(
  RemoveIf, bench::ascending,
  ranges::views::remove_if([](const int x) { return x % 2 == 0; }),
  genex::views::remove_if([](const int x) { return x % 2 == 0; }));

GENEX_BENCH_VIEW(Unique, bench::runs_of_ten, ranges::views::unique, genex::views::unique);

GENEX_BENCH_EXPR(
  Duplicates, bench::repeats_every_hundred,
  duplicates_by_hand(data),
  data | genex::views::duplicates | genex::to<std::vector>());

// ranges-v3 has no membership view, so the baseline is the filter-over-contains it expands to.
GENEX_BENCH_EXPR(
  In, data_and_other,
  data.first
    | ranges::views::filter([&other = data.second](const int x) { return ranges::contains(other, x); })
    | ranges::to<std::vector>(),
  data.first | genex::views::in(data.second) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  NotIn, data_and_other,
  data.first
    | ranges::views::filter([&other = data.second](const int x) { return not ranges::contains(other, x); })
    | ranges::to<std::vector>(),
  data.first | genex::views::not_in(data.second) | genex::to<std::vector>());
