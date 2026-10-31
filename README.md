# GenEx

GenEx is a C++ ranges library, inspired by `range-v3` and built entirely on C++ modules. It has lazy views, in-place
actions and algorithms, all of which can be composed with pipes. Concepts restrict what each operation accepts; for
example, reversing a range requires the range to be bidirectional.

```cpp
import genex;
import std;

auto main() -> int {
  auto vec = std::vector{1, 2, 3, 4, 5};

  auto result = vec
    | genex::views::transform([](const int i) { return i * 2; })
    | genex::views::filter([](const int i) { return i % 4 == 0; })
    | genex::views::materialize
    | genex::views::reverse
    | genex::to<std::vector>();

  // result is {8, 4}
}
```

A filtered range can't be reversed, so `materialize` first collects it into a container. `to` then builds the final
vector, and it works with other containers too, such as `std::set` and `std::map`.

## Requirements

GenEx needs CMake 4.4 and a compiler with `import std`. CI builds it with GCC 16, Homebrew's LLVM and MSVC.

Set `GENEX_BUILD_TESTS` or `GENEX_BUILD_BENCHMARKS` to build the tests or the benchmarks.

## Views

Views are lazy: they do nothing until the range is iterated over. Closures can be composed into a reusable step:

```cpp
const auto evens_doubled = genex::views::filter([](const int i) { return i % 2 == 0; })
  | genex::views::transform([](const int i) { return i * 2; });

auto result = vec | evens_doubled | genex::to<std::vector>();
```

## Actions

Actions change the range they're given, and run immediately.

```cpp
vec |= genex::actions::sort | genex::actions::unique;
```

## Algorithms

Algorithms are functions over ranges that don't return a view. They work with the views and with most containers.

```cpp
const auto any_even = genex::any_of(vec, [](const int i) { return i % 2 == 0; });
const auto total = genex::fold_left(vec, 0, std::plus{});
```

## Parallel

`genex::parallel` holds threaded versions of the sequential algorithms. It's a separate namespace rather than an
execution policy argument, because parallelism always has a cost and should be chosen explicitly.

```cpp
vec | genex::parallel::for_each([](auto &x) { x = expensive(x); });

// With the defaults overridden.
vec | genex::parallel::for_each(f, genex::parallel::params{.threads = 4, .threshold = 0});
```

## Iterators

`genex::iterators` provides a common way to reach a range's iterators, such as `begin` and `end`, whichever way the
container provides them.

## License

GenEx is licensed under the [Boost Software License 1.0](LICENSE).
