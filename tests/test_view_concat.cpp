#include <gtest/gtest.h>

#include <genex/views/concat.hpp>
#include <genex/views/to.hpp>


// TEST(GenexViewsConcat, VecInput) {
//     auto vec1 = std::vector{1, 2, 3, 4, 5, 6};
//     auto vec2 = std::vector{7, 8, 9};
//
//     auto temp_1 = genex::views::concat(vec2)(vec1);
//     auto temp_2 = genex::views::to<std::vector>()(temp_1);
//
//     const auto rng = vec1
//         | genex::views::concat(vec2)
//         | genex::views::to<std::vector>();
//     const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9};
//     EXPECT_EQ(rng, exp);
// }
//
//
// TEST(GenexViewsConcat, VecInputMulti) {
//     auto vec1 = std::vector{1, 2, 3, 4, 5, 6};
//     auto vec2 = std::vector{7, 8, 9};
//     auto vec3 = std::vector{10, 11};
//
//     const auto rng = genex::views::concat(vec1, vec2, vec3)
//         | genex::views::to<std::vector>();
//     const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
//     EXPECT_EQ(rng, exp);
// }
//
//
// TEST(GenexViewsConcat, VecInputMulti2) {
//     auto vec1 = std::vector{1, 2, 3, 4, 5, 6};
//     auto vec2 = std::vector{7, 8, 9};
//     auto vec3 = std::vector{10, 11};
//
//     const auto rng = vec1
//         | genex::views::concat(vec2)
//         | genex::views::concat(vec3)
//         | genex::views::to<std::vector>();
//     const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
//     EXPECT_EQ(rng, exp);
// }
