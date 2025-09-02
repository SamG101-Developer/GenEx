#include <gtest/gtest.h>
#include <tuple>

#include <genex/views/tuple_n.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsTupleN, VecInput) {
    auto vec = std::vector<std::tuple<int, int, int>>{
        {0, 10, 20},
        {1, 11, 21},
        {2, 12, 22},
        {3, 13, 23},
        {4, 14, 24},
    };

    const auto rng = vec
        | genex::views::tuple_element<2>()
        | genex::views::to<std::vector>();
    const auto exp = std::vector{20, 21, 22, 23, 24};
    EXPECT_EQ(rng, exp);
}
