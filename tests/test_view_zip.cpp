#include <gtest/gtest.h>

#include <genex/views/zip.hpp>
#include <genex/to_container.hpp>


TEST(GenexViewsZip, VecInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4};
    auto vec2 = std::vector{5, 6, 7, 8, 9};

    const auto rng = vec1
        | genex::views::zip(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::tuple<int, int>>{{0, 5}, {1, 6}, {2, 7}, {3, 8}, {4, 9}};

    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(std::get<0>(rng[i]), std::get<0>(exp[i]));
        EXPECT_EQ(std::get<1>(rng[i]), std::get<1>(exp[i]));
    }
}


TEST(GenexViewsZip, VecInputDiffTypes) {
    auto vec1 = std::vector{0, 1, 2};
    auto vec2 = std::vector<std::string>{"a", "b", "c"};

    const auto rng = vec1
        | genex::views::zip(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::tuple<int, std::string>>{{0, "a"}, {1, "b"}, {2, "c"}};

    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(std::get<0>(rng[i]), std::get<0>(exp[i]));
        EXPECT_EQ(std::get<1>(rng[i]), std::get<1>(exp[i]));
    }
}


TEST(GenexViewsZip, VecNoPipe) {
    auto vec1 = std::vector{0, 1, 2};
    auto vec2 = std::vector{3, 4, 5};

    const auto rng = genex::views::zip(vec1, vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::tuple<int, int>>{{0, 3}, {1, 4}, {2, 5}};

    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(std::get<0>(rng[i]), std::get<0>(exp[i]));
        EXPECT_EQ(std::get<1>(rng[i]), std::get<1>(exp[i]));
    }
}


TEST(GenexViewsZip, VecInputMulti) {
    auto vec1 = std::vector{0, 1, 2};
    auto vec2 = std::vector{3, 4, 5};
    auto vec3 = std::vector{6, 7, 8};

    const auto rng = genex::views::zip(vec1, vec2, vec3)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::tuple<int, int, int>>{{0, 3, 6}, {1, 4, 7}, {2, 5, 8}};

    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(std::get<0>(rng[i]), std::get<0>(exp[i]));
        EXPECT_EQ(std::get<1>(rng[i]), std::get<1>(exp[i]));
        EXPECT_EQ(std::get<2>(rng[i]), std::get<2>(exp[i]));
    }
}
