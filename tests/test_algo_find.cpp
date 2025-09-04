#include <gtest/gtest.h>

#include <genex/algorithms/find.hpp>
#include <genex/algorithms/find_last.hpp>
#include <genex/algorithms/find_if.hpp>
#include <genex/algorithms/find_last_if.hpp>
#include <genex/algorithms/find_if_not.hpp>
#include <genex/algorithms/find_last_if_not.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/views/materialize.hpp>
#include <genex/views/view.hpp>


TEST(GenexAlgosFind, FindElementExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto it = genex::algorithms::find(vec, 4);
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(genex::iterators::distance(vec.begin(), it), 3);
}


TEST(GenexAlgosFind, FindElementNotExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto it = genex::algorithms::find(vec, 10);
    EXPECT_EQ(it, vec.end());
}


TEST(GenexAlgosFind, FindElementExistsInGenerator) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6} | genex::views::view | genex::views::materialize();
    const auto it = genex::algorithms::find(vec, 4);
    EXPECT_EQ(*it, 4);
}


TEST(GenexAlgosFindLast, FindLastElementExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6, 4};
    const auto it = genex::algorithms::find_last(vec, 4);
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(genex::iterators::distance(vec.begin(), it), 6);
}


TEST(GenexAlgosFindLast, FindLastElementNotExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto it = genex::algorithms::find_last(vec, 10);
    EXPECT_EQ(it, vec.end());
}


TEST(GenexAlgosFindLast, FindLastElementExistsInGenerator) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6, 4} | genex::views::view | genex::views::materialize();
    const auto it = genex::algorithms::find_last(vec, 4);
    EXPECT_EQ(*it, 4);
}


TEST(GenexAlgosFindIf, FindIfElementExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto it = genex::algorithms::find_if(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(genex::iterators::distance(vec.begin(), it), 1);
}


TEST(GenexAlgosFindIf, FindIfElementNotExists) {
    auto vec = std::vector{1, 3, 5, 7};
    const auto it = genex::algorithms::find_if(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(it, vec.end());
}


TEST(GenexAlgosFindIf, FindIfElementExistsInGenerator) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6} | genex::views::view | genex::views::materialize();
    const auto it = genex::algorithms::find_if(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 2);
}


TEST(GenexAlgosFindLastIf, FindLastIfElementExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6, 4};
    const auto it = genex::algorithms::find_last_if(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(genex::iterators::distance(vec.begin(), it), 6);
}


TEST(GenexAlgosFindLastIf, FindLastIfElementNotExists) {
    auto vec = std::vector{1, 3, 5, 7};
    const auto it = genex::algorithms::find_last_if(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(it, vec.end());
}


TEST(GenexAlgosFindLastIf, FindLastIfElementExistsInGenerator) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6, 4} | genex::views::view | genex::views::materialize();
    const auto it = genex::algorithms::find_last_if(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 4);
}


TEST(GenexAlgosFindIfNot, FindIfNotElementExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto it = genex::algorithms::find_if_not(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(genex::iterators::distance(vec.begin(), it), 0);
}


TEST(GenexAlgosFindIfNot, FindIfNotElementNotExists) {
    auto vec = std::vector{2, 4, 6, 8};
    const auto it = genex::algorithms::find_if_not(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(it, vec.end());
}


TEST(GenexAlgosFindIfNot, FindIfNotElementExistsInGenerator) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6} | genex::views::view | genex::views::materialize();
    const auto it = genex::algorithms::find_if_not(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 1);
}


TEST(GenexAlgosFindLastIfNot, FindLastIfNotElementExists) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6, 3};
    const auto it = genex::algorithms::find_last_if_not(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(genex::iterators::distance(vec.begin(), it), 6);
}


TEST(GenexAlgosFindLastIfNot, FindLastIfNotElementNotExists) {
    auto vec = std::vector{2, 4, 6, 8};
    const auto it = genex::algorithms::find_last_if_not(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(it, vec.end());
}


TEST(GenexAlgosFindLastIfNot, FindLastIfNotElementExistsInGenerator) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6, 3} | genex::views::view | genex::views::materialize();
    const auto it = genex::algorithms::find_last_if_not(vec, [](const int v) { return v % 2 == 0; });
    EXPECT_EQ(*it, 3);
}
