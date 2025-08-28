# GenEx

GenEx (generator extensions) is a C++ library inspired by `ranges-v3` that provides a set of utilities for working with
ranges and iterators. It is designed with a generator interface, which allows for lazy evaluation of operations on
ranges. Concepts are used to restrict the operations that can be performed on which types; for example, reversing a
range will require the range be bidirectional.

## Views

GenEx provides a set of views that can be used to transform and filter ranges. These views are lazily evaluated, meaning
they do not actually perform the operation until the range is iterated over. This allows for efficient chaining of
operations on ranges.

### Examples

```cpp
#include <genex/views/transform.hpp>
#include <genex/views/filter.hpp>
#include <genex/views/reverse.hpp>

auto main() -> int {
    auto vec = std::vector<int>{1, 2, 3, 4, 5};

    auto result = vec
        | genex::views::transform([](int i) { return i * 2; })
        | genex::views::filter([](int i) { return i % 4 == 0; })
        | genex::views::reverse();

    for (auto i : result) {
        std::cout << i << ", ";
    }

    return 0;
}
```

## Actions

Actions are similar to views, but they perform the operation on the inputted range, mutating it in place. Actions are
eagerly evaluated, meaning they perform the operation immediately.

## Algorithms

There are a number of algorithms designed to be used with ranges. Whilst the algorithms are designed to be used with the
views, they can be used with almost any container. These are effectively functions over ranges that don't return a view,
such as `any_of` or `fold_left`.

## Iterators

The iterator abstraction layer provides a common interface to access key iteration members, such as `begin`, `end`,
`data` etc. There are multiple specializations of `begin` for example, allowing for different ways to access the data
depending on the design of the container.

## Todo

More algorithms, views and actions need to be implemented. The library is still in its early stages.
