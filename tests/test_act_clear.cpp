#include <gtest/gtest.h>

#include <genex/actions/clear.hpp>


TEST(GenexActionsClear, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5};
    vec |= genex::actions::clear();
    ASSERT_TRUE(vec.empty());
}