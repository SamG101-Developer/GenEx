#include <gtest/gtest.h>

#include <genex/to_container.hpp>
#include <genex/views/move_reverse.hpp>
#include <genex/views/transform.hpp>


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a and b == other.b;
    }
};


TEST(GenexViewsMoveReverse, VecInput) {
    auto vec = std::vector<std::unique_ptr<TestStruct>>{};
    vec.emplace_back(std::make_unique<TestStruct>(TestStruct{"one", 1}));
    vec.emplace_back(std::make_unique<TestStruct>(TestStruct{"two", 2}));

    const auto rng = vec
        | genex::views::move_reverse
        | genex::views::transform([](auto &&x) { return x->b; })
        | genex::to<std::vector>();
    const auto exp = std::vector<unsigned int>{2, 1};
    EXPECT_EQ(rng, exp);
}
