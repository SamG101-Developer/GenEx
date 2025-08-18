#include <cassert>
#include <coroutine>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <boost/preprocessor/comma.hpp>

#include <genex/actions/concat.hpp>
#include <genex/actions/pop.hpp>
#include <genex/actions/sort.hpp>
#include <genex/actions/remove.hpp>
#include <genex/algorithms/accumulate.hpp>
#include <genex/algorithms/all_of.hpp>
#include <genex/algorithms/any_of.hpp>
#include <genex/algorithms/contains.hpp>
#include <genex/algorithms/count.hpp>
#include <genex/algorithms/binary_search.hpp>
#include <genex/algorithms/find.hpp>
#include <genex/algorithms/none_of.hpp>
#include <genex/algorithms/position.hpp>
#include <genex/operations/at.hpp>
#include <genex/views/cast.hpp>
#include <genex/views/chunk.hpp>
#include <genex/views/copied.hpp>
#include <genex/views/concat.hpp>
#include <genex/views/cycle.hpp>
#include <genex/views/address.hpp>
#include <genex/views/drop.hpp>
#include <genex/views/duplicates.hpp>
#include <genex/views/enumerate.hpp>
#include <genex/views/filter.hpp>
#include <genex/views/flat.hpp>
#include <genex/views/fold.hpp>
#include <genex/views/for_each.hpp>
#include <genex/views/interleave.hpp>
#include <genex/views/intersperse.hpp>
#include <genex/views/iota.hpp>
#include <genex/views/join.hpp>
#include <genex/views/map.hpp>
#include <genex/views/ptr.hpp>
#include <genex/views/remove.hpp>
#include <genex/views/replace.hpp>
#include <genex/views/reverse.hpp>
#include <genex/views/set_algorithms.hpp>
#include <genex/views/slice.hpp>
#include <genex/views/take.hpp>
#include <genex/views/to.hpp>
#include <genex/views/view.hpp>
#include <genex/views/zip.hpp>
#include <genex/strings/cases.hpp>


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
    using namespace std::string_literals;

    {
        const auto v = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        const auto f = v
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(f == expected1);
    }

    // {
    //     const auto v = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    //
    //     const auto f = v
    //         | genex::views::filter([](const int x) { return x % 2 == 0; })
    //         | genex::views::to<std::vector>();
    //     const auto expected1 = std::vector{0, 2, 4, 6, 8};
    //     assert(f == expected1);
    // }

    {
        const auto v = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        const auto fff = v
            | genex::views::filter([](const int x) { return x % 2 == 0; });

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
            | genex::views::map([](const std::string &s) { return static_cast<std::size_t>(s.size()); })
            | genex::views::to<std::vector>();
        const auto expected3 = std::vector{5uz, 5uz};
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

    // {
    //     auto x = "a"s, y = "b"s, z = "c"s;
    //     std::vector<std::string&> A = std::vector<std::string&>{x, y, z};
    //
    //     const auto a = std::vector{new int(1), new int(2), new int(3)};
    //     const auto b = a
    //         | genex::views::deref
    //         | genex::views::to<std::vector>();
    //     const auto expected1 = std::vector{*a[0], *a[1], *a[2]};
    //     assert(b == expected1);
    //
    //     for (auto ptr : a) {
    //         delete ptr; // Clean up dynamically allocated memory
    //     }
    // }

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
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::duplicates()
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{5, 5};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::duplicates()
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector<int>{};
        assert(b == expected1);
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

    // {
    //     const auto a = genex::views::iota(0, 10)
    //         | genex::views::to<std::vector>();
    //     auto expected1 = std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    //     assert(a == expected1);
    //
    //     const auto b = genex::views::iota(10)
    //         | genex::views::to<std::vector>();
    //     auto expected2 = std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    //     assert(b == expected2);
    // }

    {
        const auto a = std::vector{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        const auto b = a
            | genex::views::join
            | genex::views::to<std::string>();
        const auto expected1 = "0123456789"s;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        const auto b = a
            | genex::views::join_with(",")
            | genex::views::to<std::string>();
        const auto expected1 = "0,1,2,3,4,5,6,7,8,9"s;
        assert(b == expected1);
    }

    {
        auto a = std::vector<std::unique_ptr<int>>{};
        a.emplace_back(std::make_unique<int>(1));
        a.emplace_back(std::make_unique<int>(2));
        a.emplace_back(std::make_unique<int>(3));
        const auto b = a
            | genex::views::ptr_unique
            | genex::views::to<std::vector>();
        auto expected1 = std::vector{a[0].get(), a[1].get(), a[2].get()};
        assert(b == expected1);
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

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::none_of([](const int x) { return x == 10; });
        auto expected1 = true;
        assert(b == expected1);

        const auto c = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto d = c
            | genex::algorithms::none_of([](const int x) { return x == 5; });
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
        const auto a = std::vector{4, 5, 6, 1, 2, 3, 5};
        const auto b = a
            | genex::algorithms::position([](auto x) { return x == 5; });
        const auto expected1 = 1;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{4, 5, 6, 1, 2, 3, 5};
        const auto b = a
            | genex::algorithms::position([](auto x) { return x == 7; });
        const auto expected1 = -1;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{4, 5, 6, 1, 2, 3, 5};
        const auto b = a
            | genex::algorithms::position_last([](auto x) { return x == 5; });
        const auto expected1 = 6;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{4, 5, 6, 1, 2, 3, 5};
        const auto b = a
            | genex::algorithms::position_last([](auto x) { return x == 7; });
        const auto expected1 = -1;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::cast.operator()<float>()
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
        assert(b == expected1);
    }

    {
        struct A {
        };
        struct B : A {
        };

        auto a = std::vector<std::unique_ptr<B>>{};
        a.push_back(std::make_unique<B>());
        a.push_back(std::make_unique<B>());
        const auto b = a
            | genex::views::cast.operator()<A>()
            | genex::views::to<std::vector>();
    }

    {
        const auto a = std::vector<std::string>{"0"};
        const auto b = a
            | genex::views::copied
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector<std::string>{"0"};
        assert(b == expected1);
    }

    // {
    //     auto a = std::vector<std::unique_ptr<std::string>>{};
    //     a.push_back(std::make_unique<std::string>("0"));
    //     a.push_back(std::make_unique<std::string>("1"));
    //
    //     const auto b = a
    //         | genex::views::copied
    //         | genex::views::deref
    //         | genex::views::to<std::vector>();
    //     const auto expected1 = std::vector<std::string>{"0", "1"};
    //     assert(b == expected1);
    // }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 5};
        const auto b = a
            | genex::algorithms::count(5);
        const auto expected1 = 2;
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::count_if([](const int x) { return x < 5; });
        const auto expected1 = 5;
        assert(b == expected1);
    }

    {
        const auto a = std::vector<std::vector<int>>{{0, 1, 2, 3}, {4, 5}, {6, 7, 8}, {9}};
        const auto b = a
            | genex::views::flat
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(b == expected1);

        const auto c = std::vector<std::string>{"hello", "world"};
        const auto d = c
            | genex::views::flat
            | genex::views::to<std::string>();
        assert(d == "helloworld");
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
        auto b = std::vector<int>{};
        a |
            genex::views::for_each([&b](const int x) { b.push_back(x); });
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::intersperse(10)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 10, 1, 10, 2, 10, 3, 10, 4, 10, 5, 10, 6, 10, 7, 10, 8, 10, 9};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::intersperse_with([] { return 10; })
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 10, 1, 10, 2, 10, 3, 10, 4, 10, 5, 10, 6, 10, 7, 10, 8, 10, 9};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::algorithms::accumulate(0, std::plus<int>{});
        const auto expected = 45;
        assert(b == expected);
    }

    {
        const auto a = std::vector{'a', 'b', 'c', 'd', 'e'};
        const auto b = a
            | genex::views::slice(1, 4)
            | genex::views::map([](auto &&x) { return std::toupper(x); })
            | genex::views::to<std::string>();
        const auto expected1 = std::string{"BCD"};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = a
            | genex::views::slice(3, 7)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{3, 4, 5, 6};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 0, 0};
        const auto b = a
            | genex::views::replace(0, 1)
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{1, 1, 2, 3, 4, 5, 6, 7, 1, 1};
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 0, 0};
        const auto b = a
            | genex::views::view
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 0, 0};
        assert(b == expected1);
    }

    {
        auto variadic_adder = [](auto &&... args) -> int {
            return (args + ...);
        };

        const auto a1 = std::vector{0, 1, 2, 3};
        const auto a2 = std::vector{1, 2, 3, 4};
        const auto a3 = std::vector{2, 3, 4, 5};
        const auto a4 = std::vector{3, 4, 5, 6};

        const auto b = genex::views::zip(a1, a2, a3, a4)
            | genex::views::map([variadic_adder]<typename... Ts>(Ts &&... t) { return std::apply(variadic_adder, std::forward<Ts>(t)...); })
            | genex::views::to<std::vector>();
        const auto expected1 = std::vector{6, 10, 14, 18};
        assert(b == expected1);
    }

    {
        const auto a = std::string("hello world");
        const auto b = a
            | genex::strings::upper_case
            | genex::views::to<std::string>();
        const auto expected1 = std::string("HELLO WORLD");
        assert(b == expected1);
    }

    {
        const auto a = std::string("HELLO WORLD");
        const auto b = a
            | genex::strings::lower_case
            | genex::views::to<std::string>();
        const auto expected1 = std::string("hello world");
        assert(b == expected1);
    }

    {
        const auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        const auto b = a
            | genex::views::map([](const int x) { return x * 2; })
            | genex::views::zip(a)
            | genex::views::to<std::vector>();

        // const auto c = a
        //     | genex::views::map([](const int x) { return x * 2; })
        //     | genex::views::zip(a)
        //     | genex::views::to<std::map>();
        //
        // assert(std::map<int BOOST_PP_COMMA() int>(b.begin(), b.end()) == c);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::remove(2);
        const auto expected1 = std::vector{0, 1, 3, 4, 5, 6, 7, 8, 9};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::remove_if([](const int x) { return x % 2 == 0; });
        const auto expected1 = std::vector{1, 3, 5, 7, 9};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::pop(1);
        const auto expected1 = std::vector{0, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::pop_front;
        const auto expected1 = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::pop_back;
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::concat(std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto b = genex::operations::at(a, 5);
        const auto expected1 = 5;
        assert(b == expected1);
    }

    {
        auto a = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        a |= genex::actions::sort(std::less<int>{});
        const auto expected1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        assert(a == expected1);
    }

    {
        auto a = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        a |= genex::actions::sort(std::greater<int>{});
        const auto expected1 = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        assert(a == expected1);
    }
}
