#include <gtest/gtest.h>
#include <random>

#include <genex/actions/shuffle.hpp>


TEST(GenexAlgoShuffle, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng = std::mt19937_64{std::random_device{}()};
    vec |= genex::actions::shuffle(rng);

    const auto non_exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_NE(vec, non_exp);
}


TEST(GenexAlgoShuffleRandom, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::shuffle_random;

    const auto non_exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_NE(vec, non_exp);
}
