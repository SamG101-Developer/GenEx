#include <gtest/gtest.h>

#include <genex/actions/remove.hpp>
#include <genex/views/ptr.hpp>
#include <genex/views/to.hpp>


TEST(GenexActionsRemove, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 3, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::remove(3);
    const auto exp = std::vector{0, 1, 2, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsRemove, VecInputElemNotExist) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::remove(10);
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsRemoveIf, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::remove_if([](const int x) { return x % 2 == 0; });
    const auto exp = std::vector{1, 3, 5, 7, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsRemoveIf, VecInputNoMatch) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::remove_if([](const int x) { return x > 10; });
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsRemoveIf, DISABLED_VecInputUniquePtr) {
    auto vec = std::vector<std::unique_ptr<int>>{};
    vec.push_back(std::make_unique<int>(1));
    vec.push_back(std::make_unique<int>(2));
    vec.push_back(std::make_unique<int>(3));
    vec |= genex::actions::remove_if([](auto &&x) { return *x % 2 == 0; });
    const auto exp = std::vector{1, 3};

    for (auto i = 0; i < vec.size(); ++i) {
        EXPECT_EQ(*vec[i], exp[i]);
    }
}
