#include <gtest/gtest.h>

#include <genex/views/chunk.hpp>
#include <genex/views/flatten.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsChunk, DISABLED_VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto rng = vec
        | genex::views::chunk(2)
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::vector<int>>{{1, 2}, {3, 4}, {5, 6}};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i] | genex::views::to<std::vector>(), exp[i]);
    }
}