#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.cast_dynamic;
import genex.views2.cast_static;


struct Base {
    virtual ~Base() = default;
};


struct DerivedA final : Base {
    int value;

    explicit DerivedA(const int v) : value(v) {
    }
};


struct DerivedB final : Base {
    std::string text;

    explicit DerivedB(const std::string &t) : text(t) {
    }
};


TEST(GenexViewsStaticCast, VecSimple) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::cast_static<double>()
        | genex::to<std::vector>();
    const auto exp = std::vector{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsStaticCast, VecSimpleAndBack) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::cast_static<double>()
        | genex::views::cast_static<int>()
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDynamicCast, VecPolymorphic) {
    auto a1 = DerivedA(1);
    auto a2 = DerivedA(2);
    auto b1 = DerivedB("three");
    auto b2 = DerivedB("four");
    auto vec = std::vector<Base*>{&a1, &b1, &a2, &b2};

    const auto rng = vec
        | genex::views::cast_dynamic<DerivedA*>()
        | genex::to<std::vector>();
    const auto exp = std::vector<DerivedA*>{&a1, &a2};
    EXPECT_EQ(rng, exp);
}
