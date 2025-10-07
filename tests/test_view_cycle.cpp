#include <gtest/gtest.h>

#include <genex/views/cycle.hpp>
#include <genex/views/take.hpp>
#include <genex/views/take_while.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsCycle, VecInputNumerical) {
    auto vec = std::vector{0, 1, 2};

    const auto it = vec
        | genex::views::cycle
        | genex::views::take(9)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 0, 1, 2, 0, 1, 2};
    EXPECT_EQ(it, exp);
}


TEST(GenexViewsCycle, VecInputConditional) {
    auto vec = std::vector{0, 1, 2};

    auto temp_1 = genex::views::cycle(vec);
    auto temp_2 = genex::views::take_while([] (const auto v) { return v < 2; })(temp_1);
    auto temp_3 = genex::views::to<std::vector>()(temp_2);

    const auto it = vec
        | genex::views::cycle
        | genex::views::take_while([](const auto v) { return v < 2; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1};
    EXPECT_EQ(it, exp);
}
