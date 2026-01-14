module;
#include <genex/macros.hpp>

export module genex.to_container;
export import genex.pipe;
import genex.concepts;
import genex.iterators.access;
import genex.iterators.iter_pair;
import std;


namespace genex {
    export template <template <typename> typename Out, typename Rng>
    requires input_range<Rng> and std::copyable<range_value_t<Rng>> and requires(Rng &&rng) { Out<range_value_t<Rng>>(iterators::begin(rng), iterators::end(rng)); }
    GENEX_INLINE auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
        return Out<range_value_t<Rng>>(iterators::begin(rng), iterators::end(rng));
    }

    export template <typename Out, typename Rng>
    requires input_range<Rng> and std::copyable<range_value_t<Rng>> and requires(Rng &&rng) { Out(iterators::begin(rng), iterators::end(rng)); }
    GENEX_INLINE auto to_base_fn(Rng &&rng) -> Out {
        return Out(iterators::begin(rng), iterators::end(rng));
    }

    export template <template <typename> typename Out, typename Rng>
    requires input_range<Rng>
    GENEX_INLINE auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
        Out<range_value_t<Rng>> out;
        auto [first, last] = iterators::iter_pair(rng);
        if constexpr (has_member_size<Rng> and has_member_reserve<Out<range_value_t<Rng>>>) {
            out.reserve(std::bit_cast<std::size_t>(rng.size()));
        }
        for (; first != last; ++first) {
            out.push_back(std::move(*first));
        }
        return out;
    }

    export template <typename Out, typename Rng>
    requires input_range<Rng>
    GENEX_INLINE auto to_base_fn(Rng &&rng) -> Out {
        Out out;
        auto [first, last] = iterators::iter_pair(rng);
        if constexpr (has_member_size<Rng> and has_member_reserve<Out>) {
            out.reserve(std::bit_cast<std::size_t>(rng.size()));
        }
        for (; first != last; ++first) {
            out.push_back(std::move(*first));
        }
        return out;
    }

    export template <template <typename...> typename Out>
    GENEX_INLINE auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
            return to_base_fn<Out>(std::forward<Rng>(rng));
        };
    }

    export template <typename Out>
    GENEX_INLINE auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
            return to_base_fn<Out>(std::forward<Rng>(rng));
        };
    }
}
