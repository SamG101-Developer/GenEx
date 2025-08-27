#include <gtest/gtest.h>

#include <genex/views/copy.hpp>
#include <genex/views/to.hpp>


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a && b == other.b;
    }
};


TEST(GenexViewsCopy, VecInput) {
    auto vec = std::vector{TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}};

    const auto rng = vec
        | genex::views::copy
        | genex::views::to<std::vector>();
    const auto exp = std::vector{TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}};
    EXPECT_EQ(rng, exp);
}
