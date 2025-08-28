#pragma once
#include <coroutine>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/concepts.hpp>


namespace genex::views {
    template <template <typename> typename Out, typename Rng> requires input_range<Rng>
    auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
        Out<range_value_t<Rng>> out;
        auto begin = iterators::begin(rng);
        auto end = iterators::end(rng);
        for (; begin != end; ++begin) {
            auto y = *begin;
            out.push_back(std::move(y));
        }
        return out;
    }

    template <typename Out, typename Rng> requires input_range<Rng>
    auto to_base_fn(Rng &&rng) -> Out {
        Out out;
        auto begin = iterators::begin(rng);
        auto end = iterators::end(rng);
        for (; begin != end; ++begin) {
            out.push_back(*begin);
        }
        return out;
    }

    template <template <typename...> typename Out>
    auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) { return to_base_fn<Out>(std::forward<Rng>(rng)); };
    }

    template <typename Out>
    auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) { return to_base_fn<Out>(std::forward<Rng>(rng)); };
    }
}
