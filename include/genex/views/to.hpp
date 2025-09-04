#pragma once
#include <genex/iterators/iter_pair.hpp>
#include <genex/pipe.hpp>


namespace genex::views {
    template <template <typename> typename Out, typename Rng>
        requires input_range<Rng>
    auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
        Out<range_value_t<Rng>> out;
        auto [first, last] = iterators::iter_pair(rng);
        for (; first != last; ++first) {
            auto y = *first;
            out.push_back(std::move(y));
        }
        return out;
    }

    template <typename Out, typename Rng>
        requires input_range<Rng>
    auto to_base_fn(Rng &&rng) -> Out {
        Out out;
        auto [first, last] = iterators::iter_pair(rng);
        for (; first != last; ++first) {
            out.push_back(*first);
        }
        return out;
    }

    template <template <typename...> typename Out>
    auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
            return to_base_fn<Out>(std::forward<Rng>(rng));
        };
    }

    template <typename Out>
    auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
            return to_base_fn<Out>(std::forward<Rng>(rng));
        };
    }
}
