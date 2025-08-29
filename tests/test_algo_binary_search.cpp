#include <gtest/gtest.h>

#include <genex/algorithms/binary_search.hpp>
#include <genex/iterators/distance.hpp>


TEST(GenexAlgosBinarySearch, FindElementExists) {
    auto vec = std::vector{0, 2, 4, 5, 7};
    const auto found = genex::algorithms::binary_search(vec, 4);
    EXPECT_TRUE(found);
}
