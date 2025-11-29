#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.filter;
import genex.views.materialize;
import genex.views.zip;


TEST(GenexViewsMaterialize, ZipAsVec) {
    auto vec1 = std::vector{0, 1, 2, 3, 4};
    auto vec2 = std::vector{5, 6, 7, 8, 9};

    const auto rng = genex::views::zip(vec1, vec2)
        | genex::views::materialize
        | genex::to<std::vector>();
    const auto exp = std::vector<std::tuple<int, int>>{{0, 5}, {1, 6}, {2, 7}, {3, 8}, {4, 9}};

    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsMaterialize, AsVec) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::filter([](const int x) { return x % 2 == 0; })
        | genex::views::materialize
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 2, 4, 6, 8};

    EXPECT_EQ(rng, exp);
}


// TEST(GenexViewsMaterialize, AsList) {
//     auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//
//     const auto rng = vec
//         | genex::views::filter([](const int x) { return x % 2 == 0; })
//         | genex::views::materialize<std::list>()
//         | genex::to<std::vector>();
//     const auto exp = std::vector{0, 2, 4, 6, 8};
//
//     for (auto i = 0; i < rng.size(); ++i) {
//         EXPECT_EQ(rng[i], exp[i]);
//     }
// }
//
//
// TEST(GenexViewsMaterialize, AsDeque) {
//     auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//
//     const auto rng = vec
//         | genex::views::filter([](const int x) { return x % 2 == 0; })
//         | genex::views::materialize<std::list>()
//         | genex::to<std::vector>();
//     const auto exp = std::vector{0, 2, 4, 6, 8};
//
//     for (auto i = 0; i < rng.size(); ++i) {
//         EXPECT_EQ(rng[i], exp[i]);
//     }
// }
