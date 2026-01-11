#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.ptr;


TEST(GenexViewsPtr, VecUniquePtr) {
    auto vec = std::vector<std::unique_ptr<int>>{};
    for (auto i = 1; i <= 5; ++i) {
        vec.push_back(std::make_unique<int>(i));
    }

    const auto rng = vec
        | genex::views2::ptr
        | genex::to<std::vector>();
    const auto exp = std::vector{new int(1), new int(2), new int(3), new int(4), new int(5)};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(*rng[i], *exp[i]);
    }

    for (const auto ptr : exp) {
        delete ptr;
    }
}


TEST(GenexViewsPtr, VecSharedPtr) {
    auto vec = std::vector<std::shared_ptr<int>>{};
    for (auto i = 1; i <= 5; ++i) {
        vec.push_back(std::make_shared<int>(i));
    }
    const auto exp = vec;

    const auto rng = vec
        | genex::views2::ptr
        | genex::to<std::vector>();
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i].get());
    }
}
