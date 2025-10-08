#include <gtest/gtest.h>

#include <genex/to_container.hpp>
#include <genex/views/chunk.hpp>


TEST(GenexViewsChunkN, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto rng = vec
        | genex::views::chunk(2)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::vector<int>>{{1, 2}, {3, 4}, {5, 6}};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}


TEST(GenexViewsChunkN, StrInput) {
    auto str = std::string("Hello, world!");
    const auto rng = str
        | genex::views::chunk(5)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::string>{"Hello", ", wor", "ld!"};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i] | genex::to<std::string>(), exp[i]);
    }
}
