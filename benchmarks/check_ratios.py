#!/usr/bin/env python3
"""
Checks GenEx's performance against range-v3, using the ratio between them instead of raw times.

Each benchmark is a pair run by the same process, so a ratio cancels out most of how fast or how busy the machine is.
That is what makes this usable on shared CI runners, where comparing raw times from one run to the next isn't. Two rules
apply to every pair:

 - Parity: GenEx must not be more than --max-ratio times slower than range-v3.
 - Regression: a ratio must not get more than --max-regression times worse than the baseline. Without this, something
   far faster than range-v3 could lose most of its lead and still pass.

A baseline only means something for the toolchain that made it, so the regression rule is skipped with a warning if the
compiler or range-v3 differ, or if there is no baseline yet.

The algorithms are run at each of --offsets, which set how their data is aligned (see benchmark_algorithms.cpp), and each
pair is judged by its worst one. Otherwise a loop that is only fast for aligned data would pass or fail depending on the
luck of the allocator. Nothing else uses the offset, so nothing else is repeated: taking the worst of two identical runs
would only make noise more likely to fail.

Run with --update-baseline to write a new baseline from this run's results, which then needs committing.
"""

import argparse
import json
import os
import statistics
import subprocess
import sys
import tempfile
from pathlib import Path

RANGES_PREFIX = "BM_StdRanges_"
GENEX_PREFIX = "BM_Genex_V2_"
BENCHMARK_FILTER = f"^({RANGES_PREFIX}|{GENEX_PREFIX})[A-Z]"
OFFSET_FILTER = f"^({RANGES_PREFIX}|{GENEX_PREFIX})Algo"


def run_benchmarks(binary: str, rounds: int, min_time: float, benchmark_filter: str, offset: int) -> dict[str, float]:
    """
    Median time of each benchmark over a number of whole runs. Repeating the whole run rather than each benchmark means
    a benchmark that was unlucky with what ran before it, or a burst of load on the runner, only affects one sample.
    """
    samples: dict[str, list[float]] = {}
    with tempfile.TemporaryDirectory() as directory:
        output = Path(directory) / "results.json"
        for _ in range(rounds):
            subprocess.run(
                [binary, f"--benchmark_filter={benchmark_filter}", f"--benchmark_min_time={min_time}s",
                 f"--benchmark_out={output}", "--benchmark_out_format=json", "--benchmark_format=json"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                env={**os.environ, "GENEX_BENCH_OFFSET": str(offset)})
            for benchmark in json.loads(output.read_text())["benchmarks"]:
                samples.setdefault(benchmark["run_name"], []).append(benchmark["cpu_time"])
    return {name: statistics.median(times) for name, times in samples.items()}


def ratios_of(times: dict[str, float], min_time_ns: float) -> dict[str, dict[str, float]]:
    """Pairs each benchmark up. Anything too quick to time reliably is left out, as its ratio would mostly be error."""
    pairs = {}
    for name, ranges_time in times.items():
        if not name.startswith(RANGES_PREFIX):
            continue
        pair = name.removeprefix(RANGES_PREFIX)
        genex_time = times.get(GENEX_PREFIX + pair)
        if genex_time is not None and min(ranges_time, genex_time) >= min_time_ns:
            pairs[pair] = {"ranges_ns": ranges_time, "genex_ns": genex_time, "ratio": genex_time / ranges_time}
    return pairs


def worst_of(runs: dict[int, dict[str, dict[str, float]]]) -> dict[str, dict[str, float]]:
    """Each pair at the offset where it compares worst."""
    worst = {}
    for offset, pairs in runs.items():
        for name, pair in pairs.items():
            if name not in worst or pair["ratio"] > worst[name]["ratio"]:
                worst[name] = {**pair, "offset": offset}
    return worst


def cpu_model() -> str:
    """Goes in with the results, as shared runners vary and ratios can move with the CPU."""
    try:
        for line in Path("/proc/cpuinfo").read_text().splitlines():
            if line.startswith("model name"):
                return line.split(":", 1)[1].strip()
    except OSError:
        pass
    return "unknown"


def annotate(level: str, message: str) -> None:
    """Prints a message that GitHub also shows on the run itself."""
    print(f"::{level}::{message}" if os.environ.get("GITHUB_ACTIONS") else f"{level}: {message}")


def summarize(lines: list[str]) -> None:
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as file:
            file.write("\n".join(lines) + "\n")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--binary", required=True, help="the GenExBenchmark executable")
    parser.add_argument("--baseline", type=Path, default=Path(__file__).with_name("baseline.json"))
    parser.add_argument("--output", type=Path, help="also write this run's results here, in the baseline's format")
    parser.add_argument("--update-baseline", action="store_true", help="write the results to --baseline and pass")
    parser.add_argument("--compiler", default="unknown", help="identifies the compiler the baseline is for")
    parser.add_argument("--range-v3", default="unknown", help="identifies the range-v3 the baseline is for")
    parser.add_argument("--rounds", type=int, default=5)
    parser.add_argument("--min-time", type=float, default=0.05, help="seconds to run each benchmark for, per round")
    parser.add_argument("--min-time-ns", type=float, default=5.0, help="ignore pairs quicker than this")
    parser.add_argument("--offsets", type=int, nargs="+", default=[0, 16], help="data alignments to run each pair at")
    parser.add_argument("--max-ratio", type=float, default=1.25)
    parser.add_argument("--max-regression", type=float, default=1.30)
    args = parser.parse_args()

    pairs = worst_of({
        offset: ratios_of(
            run_benchmarks(
                args.binary, args.rounds, args.min_time,
                BENCHMARK_FILTER if index == 0 else OFFSET_FILTER, offset),
            args.min_time_ns)
        for index, offset in enumerate(args.offsets)})
    if not pairs:
        annotate("error", "No benchmark pairs were found, so nothing has been checked.")
        return 1

    results = {"compiler": args.compiler, "range_v3": args.range_v3, "cpu": cpu_model(), "pairs": pairs}
    print(f"Ran on: {results['cpu']}")
    if args.output:
        args.output.write_text(json.dumps(results, indent=2, sort_keys=True) + "\n")
    if args.update_baseline:
        args.baseline.write_text(json.dumps(results, indent=2, sort_keys=True) + "\n")
        print(f"Wrote a baseline of {len(pairs)} pairs to {args.baseline}")
        return 0

    baseline = None
    if not args.baseline.exists():
        annotate("warning", f"There's no baseline at {args.baseline}, so only parity is being checked.")
    else:
        candidate = json.loads(args.baseline.read_text())
        if (candidate["compiler"], candidate["range_v3"]) != (args.compiler, args.range_v3):
            annotate(
                "warning",
                f"The baseline is for {candidate['compiler']} and range-v3 {candidate['range_v3']}, but "
                f"this run used {args.compiler} and range-v3 {args.range_v3}, so only parity is being "
                f"checked. Regenerate it with --update-baseline.")
        else:
            baseline = candidate["pairs"]

    failures = []
    rows = []
    for name, pair in sorted(pairs.items(), key=lambda item: -item[1]["ratio"]):
        previous = baseline.get(name) if baseline else None
        change = pair["ratio"] / previous["ratio"] if previous else None
        problems = []
        if pair["ratio"] > args.max_ratio:
            problems.append(f"{pair['ratio']:.2f}x range-v3, over the limit of {args.max_ratio:.2f}x")
        if change is not None and change > args.max_regression:
            problems.append(f"{change:.2f}x worse than the baseline's {previous['ratio']:.2f}x, "
                            f"over the limit of {args.max_regression:.2f}x")
        if problems:
            failures.append((name, problems))
        rows.append((name, pair, change, bool(problems)))

    missing = sorted(set(baseline) - set(pairs)) if baseline else []
    for name in missing:
        annotate("warning", f"{name} is in the baseline but wasn't in this run.")

    for name, problems in failures:
        for problem in problems:
            annotate("error", f"{name} is {problem}.")

    print(f"{'benchmark':36}{'range-v3':>11}{'genex':>11}{'ratio':>8}{'vs baseline':>13}")
    for name, pair, change, _ in rows:
        versus = f"{change:.2f}x" if change is not None else "-"
        print(f"{name:36}{pair['ranges_ns']:11.0f}{pair['genex_ns']:11.0f}{pair['ratio']:8.2f}{versus:>13}")

    summarize([
        f"### {'Failed' if failures else 'Passed'}: {len(pairs)} pairs against range-v3",
        "",
        "| Benchmark | range-v3 (ns) | GenEx (ns) | Ratio | vs baseline | |",
        "|---|---:|---:|---:|---:|---|",
        *(f"| {name} | {pair['ranges_ns']:.0f} | {pair['genex_ns']:.0f} | {pair['ratio']:.2f} | "
          f"{f'{change:.2f}x' if change is not None else '-'} | {'**failed**' if failed else ''} |"
          for name, pair, change, failed in rows),
    ])

    print(f"\n{len(pairs)} pairs, {len(failures)} failed.")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
