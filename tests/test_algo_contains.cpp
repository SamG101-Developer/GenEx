#include <coroutine>
#include <gtest/gtest.h>

import genex.algorithms.contains;
import genex.algorithms.contains_if;


TEST(GenexAlgosContains, FindElement) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const auto found = genex::contains(vec, 2);
    EXPECT_TRUE(found);
}


TEST(GenexAlgosContains, FindElementWithProjection) {
    auto vec = std::vector{0, 2, 4, 6, 8};
    const auto found = genex::contains(vec, 1, [](auto &&x) { return x / 2; });
    EXPECT_TRUE(found);
}


TEST(GenexAlgosContains, FindElementWithProjectionPtr) {
    const auto vec = std::vector{new int(0), new int(1), new int(2), new int(3), new int(4)};
    const auto ptr = new int(1);

    const auto found = genex::contains(vec.begin(), vec.end(), *ptr, [](auto &&x) { return *x; });
    EXPECT_TRUE(found);

    for (const auto p : vec) { delete p; }
    delete ptr;
}


TEST(GenexAlgosContainsIf, FindElement) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const auto found = genex::contains_if(vec, [](auto x) { return x == 2; });
    EXPECT_TRUE(found);
}


TEST(GenexAlgosContainsIf, FindElementWithProjection) {
    auto vec = std::vector{0, 2, 4, 6, 8};
    const auto found = genex::contains_if(vec, [](auto x) { return x == 1; }, [](auto &&x) { return x / 2; });
    EXPECT_TRUE(found);
}
