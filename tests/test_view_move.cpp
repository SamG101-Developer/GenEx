#include <gtest/gtest.h>

#include <genex/views/move.hpp>
#include <genex/views/to.hpp>
#include <genex/views/transform.hpp>


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a && b == other.b;
    }
};


TEST(GenexViewsMove, VecInput) {
    auto vec = std::vector<std::unique_ptr<TestStruct>>{};
    vec.emplace_back(std::make_unique<TestStruct>(TestStruct{"one", 1}));
    vec.emplace_back(std::make_unique<TestStruct>(TestStruct{"two", 2}));

    const auto rng = vec
        | genex::views::move
        | genex::views::transform([](auto &&x) { return x->b; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector<unsigned int>{1, 2};
    EXPECT_EQ(rng, exp);
}
