#pragma once
#include <coroutine>
#include <generator>


namespace genex {
    template <typename T>
    using generator = std::generator<T>;
}
