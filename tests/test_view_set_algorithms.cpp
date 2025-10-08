#include <gtest/gtest.h>

#include <genex/views/set_algorithms.hpp>
#include <genex/to_container.hpp>


TEST(GenexSetDifferenceUnsortedView, VecInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto vec2 = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    const auto rng = vec1
        | genex::views::set_difference_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetIntersectionUnsortedView, VecInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto vec2 = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    const auto rng = vec1
        | genex::views::set_intersection_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{5, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetSymmetricDifferenceUnsortedView, VecInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto vec2 = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    const auto rng = vec1
        | genex::views::set_symmetric_difference_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 10, 11, 12, 13, 14};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetUnionUnsortedView, VecInput) {
    auto vec1 = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto vec2 = std::vector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    const auto rng = vec1
        | genex::views::set_union_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetUnionUnsortedView, VecInputWithDuplicates) {
    auto vec1 = std::vector{0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9};
    auto vec2 = std::vector{5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    const auto rng = vec1
        | genex::views::set_union_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetUnionUnsortedView, VecInputAllDuplicates) {
    auto vec1 = std::vector{5, 5, 5, 5, 5};
    auto vec2 = std::vector{5, 5, 5, 5, 5};

    const auto rng = vec1
        | genex::views::set_union_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{5, 5, 5, 5, 5};
    EXPECT_EQ(rng, exp);
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetUnionUnsortedView, VecInputNoIntersection) {
    auto vec1 = std::vector{0, 1, 2, 3, 4};
    auto vec2 = std::vector{5, 6, 7, 8, 9};

    const auto rng = vec1
        | genex::views::set_union_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetUnionUnsortedView, VecInputOneEmpty) {
    auto vec1 = std::vector{0, 1, 2, 3, 4};
    auto vec2 = std::vector<int>{};

    const auto rng = vec1
        | genex::views::set_union_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4};
    EXPECT_EQ(rng, exp);
}


TEST(GenexSetUnionUnsortedView, VecInputBothEmpty) {
    auto vec1 = std::vector<int>{};
    auto vec2 = std::vector<int>{};

    const auto rng = vec1
        | genex::views::set_union_unsorted(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector<int>{};
    EXPECT_EQ(rng, exp);
}
