#include <map>

#include <gtest/gtest.h>

#include <genex/views/map.hpp>
#include <genex/to_container.hpp>


TEST(GenexViewsKeys, MapInput) {
    auto map = std::map{
        std::pair{"one", 1}, std::pair{"two", 2}, std::pair{"three", 3},
        std::pair{"four", 4}, std::pair{"five", 5}, std::pair{"six", 6}
    };

    const auto keys = map
        | genex::views::keys
        | genex::to<std::vector>();

    const auto exp = std::vector{"one", "two", "three", "four", "five", "six"};
    EXPECT_EQ(keys, exp);
}


TEST(GenexViewsVals, MapInput) {
    auto map = std::map{
        std::pair{"one", 1}, std::pair{"two", 2}, std::pair{"three", 3},
        std::pair{"four", 4}, std::pair{"five", 5}, std::pair{"six", 6}
    };

    const auto vals = map
        | genex::views::vals
        | genex::to<std::vector>();

    const auto exp = std::vector{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(vals, exp);
}
