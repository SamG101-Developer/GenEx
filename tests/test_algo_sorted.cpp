#include <gtest/gtest.h>

#include <genex/algorithms/sorted.hpp>
#include <genex/algorithms/position.hpp>


struct A {
    std::string value;

    auto operator==(const A &other) const -> bool {
        return value == other.value;
    }
};


TEST(GenexAlgosSorted, VecInput) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::sorted(vec);
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(res, exp);
}


TEST(GenexAlgosSroted, VecComplexProj) {
    auto vec1 = std::vector{A{"banana"}, A{"apple"}, A{"grape"}, A{"orange"}, A{"kiwi"}};
    auto vec2 = std::vector<std::string>{"apple", "banana", "grape", "kiwi", "orange"};

    const auto res = genex::algorithms::sorted(
        std::vector{A{"banana"}, A{"apple"}, A{"grape"}, A{"orange"}, A{"kiwi"}}, {},
        [&vec2](A &x) { return genex::algorithms::position(vec2, [&x](auto &&y) { return y == x.value; }); });

    const auto exp = std::vector{A{"apple"}, A{"banana"}, A{"grape"}, A{"kiwi"}, A{"orange"}};
    EXPECT_EQ(res, exp);
}
