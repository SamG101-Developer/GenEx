#include <coroutine>
#include <gtest/gtest.h>

import genex.conditional.if_;
import genex.views.transform;
import genex.to_container;


TEST(GenexConditionalIf, BasicIf) {
    auto x = 0;
    auto printer = [&x]() mutable {
        x = 1;
        std::cout << "Condition met!" << std::endl;
    };

    const auto vec1 = std::vector{1, 2, 3, 4, 5};
    const auto vec2 = vec1
        | genex::views::transform([](auto x) { return x * 2; })
        | genex::to<std::vector>()
        | genex::if_not(&std::vector<int>::empty, printer);

    const auto exp = std::vector{2, 4, 6, 8, 10};
    EXPECT_EQ(vec2, exp);
    EXPECT_EQ(x, 1);
}
