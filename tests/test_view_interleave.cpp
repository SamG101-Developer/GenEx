#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views.interleave;


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a and b == other.b;
    }
};


TEST(GenexViewsInterleave, VecInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4};
    auto vec2 = std::vector{5, 6, 7, 8, 9};

    const auto rng = vec1
        | genex::views::interleave(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 5, 1, 6, 2, 7, 3, 8, 4, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsInterleave, GenInput) {
    auto vec1 = std::vector{0, 1, 2};
    auto vec2 = std::vector{3, 4, 5};
    auto vec3 = std::vector{6, 7, 8, 9, 10, 11};

    const auto rng = vec1
        | genex::views::interleave(vec2)
        | genex::views::interleave(vec3)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 6, 3, 7, 1, 8, 4, 9, 2, 10, 5, 11};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsInterleave, DifferentLengthExtend) {
    auto vec1 = std::vector{0, 1, 2, 3};
    auto vec2 = std::vector{4, 5};

    const auto rng = vec1
        | genex::views::interleave(vec2, true)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 4, 1, 5, 2, 3};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsInterleave, DifferentLengthShrink) {
    auto vec1 = std::vector{0, 1, 2, 3};
    auto vec2 = std::vector{4, 5};

    const auto rng = vec1
        | genex::views::interleave(vec2, false)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 4, 1, 5};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsInterleave, IterInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4, 5, 6};
    const auto it1_begin = vec1.begin();
    const auto it1_end = vec1.end();
    auto vec2 = std::vector{7, 8, 9, 10, 11, 12, 13};
    const auto it2_begin = vec2.begin();
    const auto it2_end = vec2.end();

    const auto rng = genex::views::interleave(it1_begin, it1_end, it2_begin, it2_end)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 7, 1, 8, 2, 9, 3, 10, 4, 11, 5, 12, 6, 13};
    EXPECT_EQ(rng, exp);
}
