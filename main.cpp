#include <cassert>
#include <coroutine>
#include <iostream>
#include <string>
#include <vector>


import genex.generator;
import genex.algorithms.all_of;
import genex.algorithms.any_of;
import genex.algorithms.binary_search;
import genex.algorithms.contains;
import genex.algorithms.count;
import genex.algorithms.find;
import genex.iterators.begin;
import genex.views.chunk;
import genex.views.concat;
import genex.views.copied;
import genex.views.cycle;
import genex.views.drop;
import genex.views.enumerate;
import genex.views.filter;
import genex.views.flat;
import genex.views.fold;
import genex.views.for_each;
import genex.views.intersperse;
import genex.views.iota;
import genex.views.map;
import genex.views.remove;
import genex.views.reverse;
import genex.views.set_algorithms;
import genex.views.take;
import genex.views.to;


// Custom "<<" for "std::vector<int>"
template <typename T>
auto operator<<(std::ostream &os, const std::vector<T> &v) -> std::ostream& {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i + 1 < v.size()) { os << ", "; }
    }
    os << "]";
    return os;
}


int main() {
    {
        const auto v = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        const auto f = v
            | genex::views::filter([](const int x) { return x % 2 == 0; })
            | genex::views::filter([](const int x) { return x % 3 == 0; })
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 6};
        assert(f == expected1);
    }

    {
        const auto w = std::vector{std::string("hello"), std::string("world")};
        const auto g = w
            | genex::views::filter([](const std::string &s) { return s.starts_with("h"); })
            | genex::views::to<std::vector>();
        const auto expected2 = std::vector{std::string("hello")};
        assert(g == expected2);
    }

    {
        const auto x = std::vector{std::string("hello"), std::string("world")};
        const auto h = x
            | genex::views::map([](const std::string &s) { return s.size(); })
            | genex::views::to<std::vector>();
        const auto expected3 = std::vector{5ull, 5ull};
        assert(h == expected3);
    }

    {
        const auto y = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto z = std::vector{10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
        const auto i = y
            | genex::views::concat(z)
            | genex::views::to<std::vector>();
        const auto expected4 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
        assert(i == expected4);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::drop(5)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{5, 6, 7, 8, 9};
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::views::drop_last(5)
            | genex::views::to<std::vector>();
        const auto expected2 = std::vector{0, 1, 2, 3, 4};
        assert(d == expected2);

        const auto e = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto f = e
            | genex::views::drop_while([](const int x) { return x < 3; })
            | genex::views::to<std::vector>();
        const auto expected3 = std::vector{3, 4, 5, 6, 7, 8, 9};
        assert(f == expected3);

        const auto g = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto h = g
            | genex::views::drop_until([](const int x) { return x == 7; })
            | genex::views::to<std::vector>();
        const auto expected4 = std::vector{7, 8, 9};
        assert(h == expected4);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::take(5)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4};
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::views::take_last(5)
            | genex::views::to<std::vector>();
        const auto expected2 = std::vector{5, 6, 7, 8, 9};
        assert(d == expected2);

        const auto e = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto f = e
            | genex::views::take_while([](const int x) { return x < 3; })
            | genex::views::to<std::vector>();
        const auto expected3 = std::vector{0, 1, 2};
        assert(f == expected3);

        const auto g = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto h = g
            | genex::views::take_until([](const int x) { return x == 7; })
            | genex::views::to<std::vector>();
        const auto expected4 = std::vector{0, 1, 2, 3, 4, 5, 6};
        assert(h == expected4);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::reverse
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::cycle
            | genex::views::take(15)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::enumerate
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector<std::pair<size_t, int>>{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        const auto c = a
            | genex::views::interleave(b)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 9, 1, 8, 2, 7, 3, 6, 4, 5, 5, 4, 6, 3, 7, 2, 8, 1, 9, 0};
        assert(c == expected1);

        const auto d = std::vector{0, 1, 2, 3, 4};
        const auto e = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        const auto f = d
            | genex::views::interleave(e)
            | genex::views::to<std::vector>();
        const auto expected2 = std::vector{0, 9, 1, 8, 2, 7, 3, 6, 4, 5, 4, 3, 2, 1, 0};
        assert(f == expected2);

        const auto g = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto h = std::vector{9, 8, 7, 6, 5};
        const auto i = g
            | genex::views::interleave(h)
            | genex::views::to<std::vector>();
        const auto expected3 = std::vector{0, 9, 1, 8, 2, 7, 3, 6, 4, 5, 5, 6, 7, 8, 9};
        assert(i == expected3);
    }

    {
        const auto a = genex::views::iota(0, 10)
            | genex::views::to<std::vector>();
        auto expected1 = std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(a == expected1);

        const auto b = genex::views::iota(10)
            | genex::views::to<std::vector>();
        auto expected2 = std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(b == expected2);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::remove(5)
            | genex::views::to<std::vector>();
        auto expected1 = std::vector{0, 1, 2, 3, 4, 6, 7, 8, 9};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::remove_if([](const int x) { return x % 2 == 0; })
            | genex::views::to<std::vector>();
        auto expected1 = std::vector{1, 3, 5, 7, 9};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::all_of([](const int x) { return x < 10; });
        auto expected1 = true;
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::algorithms::all_of([](const int x) { return x < 5; });
        auto expected2 = false;
        assert(d == expected2);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::any_of([](const int x) { return x == 5; });
        auto expected1 = true;
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::algorithms::any_of([](const int x) { return x == 10; });
        auto expected2 = false;
        assert(d == expected2);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::contains(5);
        auto expected1 = true;
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::algorithms::contains(10);
        auto expected2 = false;
        assert(d == expected2);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::binary_search(5);
        auto expected1 = true;
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::algorithms::binary_search(10);
        auto expected2 = false;
        assert(d == expected2);
    }

    // {
    //     const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    //     const auto b = a
    //         | genex::views::chunk(3)
    //         | genex::views::to<std::vector>();
    //     const auto expected1 = std::vector<std::vector<int>>{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9}};
    //     assert(b == expected1);
    // }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        const auto c = a
            | genex::views::set_difference(b)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4};
        assert(c == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        const auto c = a
            | genex::views::set_intersection(b)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{5, 6, 7, 8, 9};
        assert(c == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        const auto c = a
            | genex::views::set_symmetric_difference(b)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 10, 11, 12, 13, 14};
        assert(c == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        const auto c = a
            | genex::views::set_union(b)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        assert(c == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::find(5);
        const auto expected1 = genex::iterators::begin(a) + 5;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 5};
        const auto b = a
            | genex::algorithms::find_last(5);
        const auto expected1 = genex::iterators::begin(a) + 9;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::find_if([](const int x) { return x > 5; });
        const auto expected1 = genex::iterators::begin(a) + 6;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 5};
        const auto b = a
            | genex::algorithms::find_last_if([](const int x) { return x < 5; });
        const auto expected1 = genex::iterators::begin(a) + 4;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 5};
        const auto b = a
            | genex::algorithms::find_if_not([](const int x) { return x < 5; });
        const auto expected1 = genex::iterators::begin(a) + 5;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 5};
        const auto b = a
            | genex::algorithms::find_last_if_not([](const int x) { return x > 5; });
        const auto expected1 = genex::iterators::begin(a) + 9;
        assert(b == expected1);
    }

    {
        // const auto a = std::vector<std::string>{"0"};
        // const auto b = a
        //     | genex::views::copied
        //     | genex::views::to<std::vector>();
        // const auto expected1 = std::vector<std::string>{"0"};
        // const auto expected2 = a[0] == a[1] and &a[0] != &a[1];
        // assert(b == expected1 and expected2);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 5};
        const auto b = a
            | genex::algorithms::count(5);
        const auto expected1 = 2;
        assert(b == expected1);
    }

    {
        const auto a = std::vector<std::vector<int>>{{0, 1, 2, 3}, {4, 5}, {6, 7, 8}, {9}};
        const auto b = a
            | genex::views::flat
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::fold_left(0, std::minus<int>{});
        const auto expected1 = -45;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::fold_right(0, std::minus<int>{});
        const auto expected1 = -5;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::fold(0, std::minus<int>{});
        const auto expected1 = -45;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto b = std::vector<int>{};
        a |
            genex::views::for_each([&b](const int x) { b.push_back(x); });
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(b == expected1);
    }
}
