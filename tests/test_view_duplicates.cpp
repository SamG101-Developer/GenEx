#include <coroutine>
#include <gtest/gtest.h>

import genex.meta;
import genex.to_container;
import genex.views.duplicates;


TEST(GenexViewsDuplicates, Duplicates) {
    auto vec = std::vector{1, 2, 3, 4, 1, 2};

    const auto rng = vec
        | genex::views::duplicates
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 1};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDuplicates, NoDuplicates) {
    auto vec = std::vector{1, 2, 3, 4};

    const auto rng = vec
        | genex::views::duplicates
        | genex::to<std::vector>();
    const auto exp = std::vector<int>{};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDuplicates, DerefProjection) {
    auto vec = std::vector<std::shared_ptr<int>>{};
    vec.emplace_back(std::make_shared<int>(1));
    vec.emplace_back(std::make_shared<int>(2));
    vec.emplace_back(std::make_shared<int>(3));
    vec.emplace_back(std::make_shared<int>(1));
    vec.emplace_back(std::make_shared<int>(2));
    vec.emplace_back(std::make_shared<int>(4));

    const auto rng = vec
        | genex::views::duplicates({}, genex::meta::deref)
        | genex::to<std::vector>();
    const auto exp = std::vector{vec[0], vec[3]};
    EXPECT_EQ(rng, exp);
}
