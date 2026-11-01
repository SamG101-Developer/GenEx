#include "view_benchmarks.hpp"

#include <range/v3/to_container.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/take.hpp>

import genex.to_container;
import genex.views.empty;
import genex.views.generate;
import genex.views.iota;
import genex.views.repeat;
import genex.views.single;
import genex.views.take;

GENEX_BENCH_EXPR(
  Iota_StartEnd, bench::no_data,
  ranges::views::iota(0, 1000) | ranges::to<std::vector>(),
  genex::views::iota(0, 1000) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  RepeatN, bench::no_data,
  ranges::views::repeat_n(42, 1000) | ranges::to<std::vector>(),
  genex::views::repeat(42, 1000) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  RepeatUnbounded, bench::no_data,
  ranges::views::repeat(42) | ranges::views::take(1000) | ranges::to<std::vector>(),
  genex::views::repeat(42) | genex::views::take(1000) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  GenerateN, bench::no_data,
  ranges::views::generate_n([next = 0]() mutable { return next++; }, 1000) | ranges::to<std::vector>(),
  genex::views::generate_n([next = 0]() mutable { return next++; }, 1000) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  GenerateUnbounded, bench::no_data,
  ranges::views::generate([next = 0]() mutable { return next++; }) | ranges::views::take(1000) | ranges::to<std::vector>(),
  genex::views::generate([next = 0]() mutable { return next++; }) | genex::views::take(1000) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  Single, bench::no_data,
  ranges::views::single(42) | ranges::to<std::vector>(),
  genex::views::single(42) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  Empty, bench::no_data,
  ranges::views::empty<int> | ranges::to<std::vector>(),
  genex::views::empty<int> | genex::to<std::vector>());
