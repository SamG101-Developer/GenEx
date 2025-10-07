#pragma once
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views {
    /**
     * Most optimal implementation, when the target container can be constructed from two iterators.
     * @tparam Out Template for the output container, e.g. std::vector, std::list, etc.
     * @tparam Rng The input range type.
     * @param rng The input range.
     * @return The output container.
     */
    template <template <typename> typename Out, typename Rng>
    requires input_range<Rng> and std::copyable<range_value_t<Rng>> and requires(Rng &&rng) { Out<range_value_t<Rng>>(iterators::begin(rng), iterators::end(rng)); }
    GENEX_INLINE auto to_base_fn(Rng &&rng) noexcept(
        noexcept(iterators::begin(rng)) and noexcept(iterators::end(rng))) -> Out<range_value_t<Rng>> {
        return Out<range_value_t<Rng>>(iterators::begin(rng), iterators::end(rng));
    }

    template <typename Out, typename Rng>
    requires input_range<Rng> and std::copyable<range_value_t<Rng>> and requires(Rng &&rng) { Out(iterators::begin(rng), iterators::end(rng)); }
    GENEX_INLINE auto to_base_fn(Rng &&rng) noexcept(
        noexcept(iterators::begin(rng)) and noexcept(iterators::end(rng))) -> Out {
        return Out(iterators::begin(rng), iterators::end(rng));
    }

    template <template <typename> typename Out, typename Rng>
    GENEX_INLINE auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
        Out<range_value_t<Rng>> out;
        auto [first, last] = iterators::iter_pair(rng);
        if constexpr (has_member_size<Rng>) {
            out.reserve(rng.size());
        }
        for (; first != last; ++first) {
            out.push_back(std::move(*first));
        }
        return out;
    }

    template <typename Out, typename Rng>
    requires input_range<Rng>
    GENEX_INLINE auto to_base_fn(Rng &&rng) -> Out {
        Out out;
        auto [first, last] = iterators::iter_pair(rng);
        if constexpr (has_member_size<Rng>) {
            out.reserve(rng.size());
        }
        for (; first != last; ++first) {
            out.push_back(std::move(*first));
        }
        return out;
    }

    template <template <typename...> typename Out>
    GENEX_INLINE auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
            return to_base_fn<Out>(std::forward<Rng>(rng));
        };
    }

    template <typename Out>
    GENEX_INLINE auto to() -> auto {
        return []<typename Rng> requires input_range<Rng>(Rng &&rng) {
            return to_base_fn<Out>(std::forward<Rng>(rng));
        };
    }
}
