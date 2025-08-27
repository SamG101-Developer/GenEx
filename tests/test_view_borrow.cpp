#include <gtest/gtest.h>

#include <genex/views/borrow.hpp>
#include <genex/views/for_each.hpp>
#include <genex/views/to.hpp>


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a && b == other.b;
    }
};


TEST(GenexViewsBorrow, VecInput) {
    auto vec = std::vector<TestStruct>{};
    vec.emplace_back(TestStruct{"one", 1});
    vec.emplace_back(TestStruct{"two", 2});

    vec
        | genex::views::borrow
        | genex::views::for_each([](auto &&x) { x.b *= 10; });
    EXPECT_EQ(vec[0].b, 10);
    EXPECT_EQ(vec[1].b, 20);
}
