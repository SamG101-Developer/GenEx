#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.cast_dynamic;
import genex.views2.cast_smart;
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
        | genex::views2::cast_static<double>()
        | genex::to<std::vector>();
    const auto exp = std::vector{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsStaticCast, VecSimpleAndBack) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views2::cast_static<double>()
        | genex::views2::cast_static<int>()
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
        | genex::views2::cast_dynamic<DerivedA*>()
        | genex::to<std::vector>();
    const auto exp = std::vector<DerivedA*>{&a1, &a2};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsSharedCast, VecPolymorphicSmartPtr) {
    const auto a1 = std::make_shared<DerivedA>(1);
    const auto a2 = std::make_shared<DerivedA>(2);
    const auto b1 = std::make_shared<DerivedB>("three");
    const auto b2 = std::make_shared<DerivedB>("four");
    auto vec = std::vector<std::shared_ptr<Base>>{a1, b1, a2, b2};

    const auto rng = vec
        | genex::views2::cast_smart<DerivedA>()
        | genex::to<std::vector>();
    const auto exp = std::vector{a1, a2};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsUniqueCast, VecPolymorphicSmartPtr) {
    auto vec = std::vector<std::unique_ptr<Base>>{};
    vec.push_back(std::make_unique<DerivedA>(1));
    vec.push_back(std::make_unique<DerivedB>("two"));
    vec.push_back(std::make_unique<DerivedA>(3));
    vec.push_back(std::make_unique<DerivedB>("four"));

    const auto rng = vec
        | genex::views2::cast_smart<DerivedA>()
        | genex::to<std::vector>();
    auto exp = std::vector<std::unique_ptr<DerivedA>>{};
    exp.emplace_back(std::make_unique<DerivedA>(1));
    exp.emplace_back(std::make_unique<DerivedA>(3));

    ASSERT_EQ(rng.size(), exp.size());
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i]->value, exp[i]->value);
    }
}
