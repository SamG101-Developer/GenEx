#include <memory>
#include <benchmark/benchmark.h>
#include <genex/views/address.hpp>
#include <genex/views/borrow.hpp>
#include <genex/views/cast_dynamic.hpp>
#include <genex/views/cast_smart.hpp>
#include <genex/views/cast_static.hpp>
#include <genex/views/chunk.hpp>
#include <genex/views/concat.hpp>
#include <genex/views/cycle.hpp>
#include <genex/views/drop.hpp>
#include <genex/views/drop_last.hpp>
#include <genex/views/drop_while.hpp>
#include <genex/views/enumerate.hpp>
#include <genex/views/filter.hpp>
#include <genex/views/indirect.hpp>
#include <genex/views/interleave.hpp>
#include <genex/views/intersperse.hpp>
#include <genex/views/iota.hpp>
#include <genex/views/join.hpp>
#include <genex/views/join_with.hpp>
#include <genex/views/map.hpp>
#include <genex/views/move.hpp>
#include <genex/views/ptr.hpp>
#include <genex/views/remove.hpp>
#include <genex/views/remove_if.hpp>
#include <genex/views/replace.hpp>
#include <genex/views/replace_if.hpp>
#include <genex/views/reverse.hpp>
#include <genex/views/slice.hpp>
#include <genex/views/split.hpp>
#include <genex/views/take.hpp>
#include <genex/views/take_last.hpp>
#include <genex/views/take_while.hpp>
#include <genex/views/to.hpp>
#include <genex/views/transform.hpp>
#include <genex/views/tuple_nth.hpp>
#include <genex/views/view.hpp>
#include <genex/views/zip.hpp>
#include <range/v3/view/addressof.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/drop_last.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/remove.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/replace.hpp>
#include <range/v3/view/replace_if.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/take_last.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/zip.hpp>


struct TestStruct {
    int a;

    explicit TestStruct(const int a = 0) : a(a) {
    }

    virtual ~TestStruct() = default;
};


struct TestStructDerived final : TestStruct {
    int b;

    explicit TestStructDerived(const int a = 0, const int b = 0) : TestStruct(a), b(b) {
    }

    ~TestStructDerived() override = default;
};


static auto bm_genex_to_vector(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_to_vector(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_borrow(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::borrow
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_borrow(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](auto &v) -> auto& { return v; })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_address(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::address
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_address(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::addressof
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_filter(benchmark::State &state) -> void {
    auto vec = std::vector(1000, TestStruct{42});
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::filter([](const int v) { return v == 42; }, &TestStruct::a)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_filter(benchmark::State &state) -> void {
    auto vec = std::vector(1000, TestStruct{42});
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::filter([](const int v) { return v == 42; }, &TestStruct::a)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_transform(benchmark::State &state) -> void {
    auto vec = std::vector(1000, TestStruct{42});
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::transform([](const TestStruct &t) { return t.a * 5; })
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_transform(benchmark::State &state) -> void {
    auto vec = std::vector(1000, TestStruct{42});
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](const TestStruct &t) { return t.a * 5; })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_cast_dynamic(benchmark::State &state) -> void {
    auto vec = std::vector<TestStruct*>();
    for (auto i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            vec.push_back(new TestStruct(100));
        }
        else {
            vec.push_back(new TestStructDerived(100, 200));
        }
    }
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::cast_dynamic<TestStructDerived*>()
            | genex::views::to<std::vector>();
    }

    for (const auto ptr : vec) {
        delete ptr;
    }
}


static auto bm_ranges_cast_dynamic(benchmark::State &state) -> void {
    auto vec = std::vector<TestStruct*>();
    for (auto i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            vec.push_back(new TestStruct(100));
        }
        else {
            vec.push_back(new TestStructDerived(100, 200));
        }
    }
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](TestStruct *ptr) { return dynamic_cast<TestStructDerived*>(ptr); })
            | ranges::views::filter([](const TestStructDerived *ptr) { return ptr != nullptr; })
            | ranges::to<std::vector>();
    }

    for (const auto ptr : vec) {
        delete ptr;
    }
}


static auto bm_genex_cast_static(benchmark::State &state) -> void {
    auto vec = std::vector<float>(1000, 42.0f);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::cast_static<int>()
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_cast_static(benchmark::State &state) -> void {
    auto vec = std::vector<float>(1000, 42.0f);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](float v) { return static_cast<int>(v); })
            | ranges::to<std::vector>();
    }
}


// static auto bm_genex_cast_unique_ptr(benchmark::State &state) -> void {
//     auto vec = std::vector<std::unique_ptr<TestStruct>>();
//     for (auto i = 0; i < 1000; ++i) {
//         if (i % 2 == 0) {
//             vec.push_back(std::make_unique<TestStruct>(100));
//         }
//         else {
//             vec.push_back(std::make_unique<TestStructDerived>(100, 200));
//         }
//     }
//     for (auto _ : state) {
//         auto vec2 = vec
//             | genex::views::cast_smart<TestStructDerived>()
//             | genex::views::to<std::vector>();
//     }
// }
//
//
// static auto bm_ranges_cast_unique_ptr(benchmark::State &state) -> void {
//     auto vec = std::vector<std::unique_ptr<TestStruct>>();
//     for (auto i = 0; i < 1000; ++i) {
//         if (i % 2 == 0) {
//             vec.push_back(std::make_unique<TestStruct>(100));
//         }
//         else {
//             vec.push_back(std::make_unique<TestStructDerived>(100, 200));
//         }
//     }
//     for (auto _ : state) {
//         auto vec2 = vec
//             | ranges::views::transform([](const std::unique_ptr<TestStruct> &ptr) {
//                 if (const auto derived = dynamic_cast<TestStructDerived*>(ptr.get())) {
//                     return std::unique_ptr<TestStructDerived>(new TestStructDerived(*derived));
//                 }
//                 return std::unique_ptr<TestStructDerived>(nullptr);
//             })
//             | ranges::views::filter([](const std::unique_ptr<TestStructDerived> &ptr) { return ptr != nullptr; })
//             | ranges::to<std::vector>();
//     }
// }
//
//
// static auto bm_genex_cast_shared_ptr(benchmark::State &state) -> void {
//     auto vec = std::vector<std::shared_ptr<TestStruct>>();
//     for (auto i = 0; i < 1000; ++i) {
//         if (i % 2 == 0) {
//             vec.push_back(std::make_shared<TestStruct>(100));
//         }
//         else {
//             vec.push_back(std::make_shared<TestStructDerived>(100, 200));
//         }
//     }
//     for (auto _ : state) {
//         auto vec2 = vec
//             | genex::views::cast_smart<TestStructDerived>()
//             | genex::views::to<std::vector>();
//     }
// }
//
//
// static auto bm_ranges_cast_shared_ptr(benchmark::State &state) -> void {
//     auto vec = std::vector<std::shared_ptr<TestStruct>>();
//     for (auto i = 0; i < 1000; ++i) {
//         if (i % 2 == 0) {
//             vec.push_back(std::make_shared<TestStruct>(100));
//         }
//         else {
//             vec.push_back(std::make_shared<TestStructDerived>(100, 200));
//         }
//     }
//     for (auto _ : state) {
//         auto vec2 = vec
//             | ranges::views::transform([](const std::shared_ptr<TestStruct> &ptr) {
//                 return std::dynamic_pointer_cast<TestStructDerived>(ptr);
//             })
//             | ranges::views::filter([](const std::shared_ptr<TestStructDerived> &ptr) { return ptr != nullptr; })
//             | ranges::to<std::vector>();
//     }
// }


static auto bm_genex_split(benchmark::State &state) -> void {
    auto vec = std::vector<int>();
    for (auto x = 0; x < 100; ++x) {
        for (auto y = 0; y < 10; ++y) {
            vec.push_back(y);
        }
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::split(9)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_split(benchmark::State &state) -> void {
    auto vec = std::vector<int>();
    for (auto x = 0; x < 100; ++x) {
        for (auto y = 0; y < 10; ++y) {
            vec.push_back(y);
        }
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::split(9)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_chunk(benchmark::State &state) -> void {
    auto vec = std::vector<int>();
    for (auto x = 0; x < 10; ++x) {
        for (auto y = 0; y < 100; ++y) {
            vec.push_back(x);
        }
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::chunk(50)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_chunk(benchmark::State &state) -> void {
    auto vec = std::vector<int>();
    for (auto x = 0; x < 10; ++x) {
        for (auto y = 0; y < 100; ++y) {
            vec.push_back(x);
        }
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::chunk(50)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_concat(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);
    auto vec3 = std::vector(1000, 44);

    auto temp_1 = vec1 | genex::views::concat(vec2);
    auto temp_2 = genex::views::to<std::vector>()(temp_1);

    for (auto _ : state) {
        auto vec4 = vec1
            | genex::views::concat(vec2)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_concat(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);

    for (auto _ : state) {
        auto vec4 = ranges::views::concat(vec1, vec2)
            | ranges::to<std::vector>();
    }
}


// static auto bm_genex_cycle(benchmark::State &state) -> void {
//     auto vec = std::vector(1000, 42);
//
//     for (auto _ : state) {
//         auto vec2 = vec
//             | genex::views::cycle
//             | genex::views::take(100)
//             | genex::views::to<std::vector>();
//     }
// }
//
//
// static auto bm_ranges_cycle(benchmark::State &state) -> void {
//     auto vec = std::vector(1000, 42);
//
//     for (auto _ : state) {
//         auto vec2 = vec
//             | ranges::views::cycle
//             | ranges::views::take(100)
//             | ranges::to<std::vector>();
//     }
// }


static auto bm_genex_drop(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::drop(500)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_drop(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::drop(500)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_drop_last(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::drop_last(500)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_drop_last(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::drop_last(500)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_drop_while(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);
    vec1.append_range(std::move(vec2));

    for (auto _ : state) {
        auto vec3 = vec1
            | genex::views::drop_while([](const int v) { return v == 42; })
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_drop_while(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);
    vec1.append_range(std::move(vec2));

    for (auto _ : state) {
        auto vec3 = vec1
            | ranges::views::drop_while([](const int v) { return v == 42; })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_enumerate(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::enumerate
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_enumerate(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::enumerate
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_join(benchmark::State &state) -> void {
    auto vec = std::vector<std::vector<int>>();
    for (auto i = 0; i < 100; ++i) {
        vec.push_back(std::vector(10, i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::join
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_join(benchmark::State &state) -> void {
    auto vec = std::vector<std::vector<int>>();
    for (auto i = 0; i < 100; ++i) {
        vec.push_back(std::vector(10, i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::join
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_join_with(benchmark::State &state) -> void {
    auto vec = std::vector<std::vector<int>>();
    for (auto i = 0; i < 100; ++i) {
        vec.push_back(std::vector(10, i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::join_with(-1)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_join_with(benchmark::State &state) -> void {
    auto vec = std::vector<std::vector<int>>();
    for (auto i = 0; i < 100; ++i) {
        vec.push_back(std::vector(10, i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::join(-1)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_indirect(benchmark::State &state) -> void {
    auto vec = std::vector<std::shared_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.push_back(std::make_shared<int>(i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::indirect
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_indirect(benchmark::State &state) -> void {
    auto vec = std::vector<std::unique_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](const std::unique_ptr<int> &ptr) -> int& { return *ptr; })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_interleave(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);

    for (auto _ : state) {
        auto vec3 = vec1
            | genex::views::interleave(vec2)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_interleave(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);

    for (auto _ : state) {
        auto vec3 = ranges::views::zip(vec1, vec2)
            | ranges::views::transform([](const auto &t) { return std::vector{std::get<0>(t), std::get<1>(t)}; })
            | ranges::views::join
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_intersperse(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::intersperse(0)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_intersperse(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::intersperse(0)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_iota(benchmark::State &state) -> void {
    for (auto _ : state) {
        auto vec = genex::views::iota(0, 1000)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_iota(benchmark::State &state) -> void {
    for (auto _ : state) {
        auto vec = ranges::views::iota(0, 1000)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_map_keys(benchmark::State &state) -> void {
    auto map = std::map<int, int>();
    for (auto i = 0; i < 1000; ++i) {
        map[i] = i * 2;
    }

    for (auto _ : state) {
        auto vec = map
            | genex::views::keys
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_map_keys(benchmark::State &state) -> void {
    auto map = std::map<int, int>();
    for (auto i = 0; i < 1000; ++i) {
        map[i] = i * 2;
    }

    for (auto _ : state) {
        auto vec = map
            | ranges::views::keys
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_map_values(benchmark::State &state) -> void {
    auto map = std::map<int, int>();
    for (auto i = 0; i < 1000; ++i) {
        map[i] = i * 2;
    }

    for (auto _ : state) {
        auto vec = map
            | genex::views::vals
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_map_values(benchmark::State &state) -> void {
    auto map = std::map<int, int>();
    for (auto i = 0; i < 1000; ++i) {
        map[i] = i * 2;
    }

    for (auto _ : state) {
        auto vec = map
            | ranges::views::values
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_move(benchmark::State &state) -> void {
    auto vec = std::vector<std::unique_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::move
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_move(benchmark::State &state) -> void {
    auto vec = std::vector<std::unique_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::move
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_ptr(benchmark::State &state) -> void {
    auto vec = std::vector<std::shared_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.push_back(std::make_shared<int>(i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::ptr
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_ptr(benchmark::State &state) -> void {
    auto vec = std::vector<std::shared_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.push_back(std::make_shared<int>(i));
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](const std::shared_ptr<int> &ptr) { return ptr.get(); })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_remove(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec[500] = 43;
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::remove(43)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_remove(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec[500] = 43;
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::remove(43)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_remove_if(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec.append_range(std::vector(250, 43));
    vec.append_range(std::vector(250, 44));
    vec.append_range(std::vector(250, 45));
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::remove_if([](const int v) { return v > 43; })
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_remove_if(benchmark::State &state) -> void {
    auto vec = std::vector(250, 42);
    vec.append_range(std::vector(250, 43));
    vec.append_range(std::vector(250, 44));
    vec.append_range(std::vector(250, 45));
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::remove_if([](const int v) { return v > 43; })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_replace(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec[500] = 43;
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::replace(43, 44)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_replace(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec[500] = 43;
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::replace(43, 44)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_replace_if(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec.append_range(std::vector(250, 43));
    vec.append_range(std::vector(250, 44));
    vec.append_range(std::vector(250, 45));
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::replace_if([](const int v) { return v > 43; }, 46)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_replace_if(benchmark::State &state) -> void {
    auto vec = std::vector(250, 42);
    vec.append_range(std::vector(250, 43));
    vec.append_range(std::vector(250, 44));
    vec.append_range(std::vector(250, 45));
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::replace_if([](const int v) { return v > 43; }, 46)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_reverse(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::reverse
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_reverse(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::reverse
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_take(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::take(500)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_take(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::take(500)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_take_last(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::take_last(500)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_take_last(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::take_last(500)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_take_while(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    vec.append_range(std::vector(250, 43));
    vec.append_range(std::vector(250, 44));
    vec.append_range(std::vector(250, 45));
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::take_while([](const int v) { return v == 42; })
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_take_while(benchmark::State &state) -> void {
    auto vec = std::vector(250, 42);
    vec.append_range(std::vector(250, 43));
    vec.append_range(std::vector(250, 44));
    vec.append_range(std::vector(250, 45));
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::take_while([](const int v) { return v == 42; })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_slice(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::slice(200, 800)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_slice(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::slice(200, 800)
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_tuple_nth(benchmark::State &state) -> void {
    auto vec = std::vector<std::tuple<int, float, std::string>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.emplace_back(i, static_cast<float>(i) * 1.5f, "test");
    }

    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::tuple_nth<1>()
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_tuple_nth(benchmark::State &state) -> void {
    auto vec = std::vector<std::tuple<int, float, std::string>>();
    for (auto i = 0; i < 1000; ++i) {
        vec.emplace_back(i, static_cast<float>(i) * 1.5f, "test");
    }

    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::transform([](const auto &t) { return std::get<1>(t); })
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_view(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | genex::views::view
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_view(benchmark::State &state) -> void {
    auto vec = std::vector(1000, 42);
    for (auto _ : state) {
        auto vec2 = vec
            | ranges::views::all
            | ranges::to<std::vector>();
    }
}


static auto bm_genex_zip(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);

    for (auto _ : state) {
        auto vec3 = vec1
            | genex::views::zip(vec2)
            | genex::views::to<std::vector>();
    }
}


static auto bm_ranges_zip(benchmark::State &state) -> void {
    auto vec1 = std::vector(1000, 42);
    auto vec2 = std::vector(1000, 43);

    for (auto _ : state) {
        auto vec3 = ranges::views::zip(vec1, vec2)
            | ranges::to<std::vector>();
    }
}


BENCHMARK(bm_genex_to_vector);
BENCHMARK(bm_ranges_to_vector);
BENCHMARK(bm_genex_address);
BENCHMARK(bm_ranges_address);
BENCHMARK(bm_genex_borrow);
BENCHMARK(bm_ranges_borrow);
BENCHMARK(bm_genex_filter);
BENCHMARK(bm_ranges_filter);
BENCHMARK(bm_genex_transform);
BENCHMARK(bm_ranges_transform);
BENCHMARK(bm_genex_cast_dynamic);
BENCHMARK(bm_ranges_cast_dynamic);
BENCHMARK(bm_genex_cast_static);
BENCHMARK(bm_ranges_cast_static);
// BENCHMARK(bm_genex_cast_unique_ptr);
// BENCHMARK(bm_ranges_cast_unique_ptr);
// BENCHMARK(bm_genex_cast_shared_ptr);
// BENCHMARK(bm_ranges_cast_shared_ptr);
BENCHMARK(bm_genex_split);
BENCHMARK(bm_ranges_split);
BENCHMARK(bm_genex_chunk);
BENCHMARK(bm_ranges_chunk);
BENCHMARK(bm_genex_concat);
BENCHMARK(bm_ranges_concat);
// BENCHMARK(bm_genex_cycle);
// BENCHMARK(bm_ranges_cycle);
BENCHMARK(bm_genex_drop);
BENCHMARK(bm_ranges_drop);
BENCHMARK(bm_genex_drop_last);
BENCHMARK(bm_ranges_drop_last);
BENCHMARK(bm_genex_drop_while);
BENCHMARK(bm_ranges_drop_while);
BENCHMARK(bm_genex_enumerate);
BENCHMARK(bm_ranges_enumerate);
BENCHMARK(bm_genex_join);
BENCHMARK(bm_ranges_join);
BENCHMARK(bm_genex_join_with);
BENCHMARK(bm_ranges_join_with);
BENCHMARK(bm_genex_indirect);
BENCHMARK(bm_ranges_indirect);
BENCHMARK(bm_genex_interleave);
BENCHMARK(bm_ranges_interleave);
BENCHMARK(bm_genex_intersperse);
BENCHMARK(bm_ranges_intersperse);
BENCHMARK(bm_genex_iota);
BENCHMARK(bm_ranges_iota);
BENCHMARK(bm_genex_map_keys);
BENCHMARK(bm_ranges_map_keys);
BENCHMARK(bm_genex_map_values);
BENCHMARK(bm_ranges_map_values);
BENCHMARK(bm_genex_move);
BENCHMARK(bm_ranges_move);
BENCHMARK(bm_genex_ptr);
BENCHMARK(bm_ranges_ptr);
BENCHMARK(bm_ranges_remove);
BENCHMARK(bm_ranges_remove);
BENCHMARK(bm_genex_remove_if);
BENCHMARK(bm_ranges_remove_if);
BENCHMARK(bm_genex_replace);
BENCHMARK(bm_ranges_replace);
BENCHMARK(bm_genex_replace_if);
BENCHMARK(bm_ranges_replace_if);
BENCHMARK(bm_genex_reverse);
BENCHMARK(bm_ranges_reverse);
BENCHMARK(bm_genex_take);
BENCHMARK(bm_ranges_take);
BENCHMARK(bm_genex_take_last);
BENCHMARK(bm_ranges_take_last);
BENCHMARK(bm_genex_take_while);
BENCHMARK(bm_ranges_take_while);
BENCHMARK(bm_genex_slice);
BENCHMARK(bm_ranges_slice);
BENCHMARK(bm_genex_tuple_nth);
BENCHMARK(bm_ranges_tuple_nth);
BENCHMARK(bm_genex_view);
BENCHMARK(bm_ranges_view);
BENCHMARK(bm_genex_zip);
BENCHMARK(bm_ranges_zip);


BENCHMARK_MAIN();
