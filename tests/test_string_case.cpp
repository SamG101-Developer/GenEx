#include <coroutine>
#include <gtest/gtest.h>

import genex.strings.cases;
import genex.to_container;

using namespace std::string_literals;


TEST(GenexStringsLowerCase, VecInput) {
    auto str = "HELLO WORLD!"s;
    const auto rng = str
        | genex::strings::lower_case
        | genex::to<std::string>();

    const auto exp = std::string("hello world!");
    EXPECT_EQ(rng, exp);
}


TEST(GenexStringsUpperCase, VecInput) {
    auto str = "hello world!"s;
    const auto rng = str
        | genex::strings::upper_case
        | genex::to<std::string>();

    const auto exp = std::string("HELLO WORLD!");
    EXPECT_EQ(rng, exp);
}
