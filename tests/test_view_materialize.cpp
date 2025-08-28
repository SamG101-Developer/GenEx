#include <gtest/gtest.h>

#include <deque>
#include <list>
#include <vector>

#include <genex/views/filter.hpp>
#include <genex/views/map.hpp>
#include <genex/views/materialize.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsMaterialize, AsVec) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::filter([](const int x) { return x % 2 == 0; })
        | genex::views::materialize
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 2, 4, 6, 8};

    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsMaterialize, AsList) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::filter([](const int x) { return x % 2 == 0; })
        | genex::views::materialize.operator()<std::list>()
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 2, 4, 6, 8};

    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}


TEST(GenexViewsMaterialize, AsDeque) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::filter([](const int x) { return x % 2 == 0; })
        | genex::views::materialize.operator()<std::list>()
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 2, 4, 6, 8};

    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}
