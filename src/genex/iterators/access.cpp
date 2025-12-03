module;
#include <genex/macros.hpp>

export module genex.iterators.access;
import genex.concepts;
import std;


namespace genex::iterators::detail::concepts {
    template <typename Rng>
    concept beginable_range =
        input_range<Rng>;

    template <typename Rng>
    concept beginable_select_begin =
        beginable_range<Rng> and
        has_member_begin<Rng>;

    template <typename Rng>
    concept beginable_select_std_begin =
        beginable_range<Rng> and
        has_std_begin<Rng> and
        not has_member_begin<Rng>;

    template <typename Rng>
    concept reverse_beginable_range =
        bidirectional_range<Rng>;

    template <typename Rng>
    concept reverse_beginable_select_rbegin =
        reverse_beginable_range<Rng> and
        has_member_rbegin<Rng>;

    template <typename Rng>
    concept reverse_beginable_select_std_rbegin =
        reverse_beginable_range<Rng> and
        has_std_rbegin<Rng> and
        not has_member_rbegin<Rng>;

    template <typename Rng>
    concept endable_range =
        input_range<Rng>;

    template <typename Rng>
    concept endable_select_end =
        endable_range<Rng> and
        has_member_end<Rng>;

    template <typename Rng>
    concept endable_select_std_end =
        endable_range<Rng> and
        has_std_end<Rng> and
        not has_member_end<Rng>;

    template <typename Rng>
    concept reverse_endable_range =
        bidirectional_range<Rng>;

    template <typename Rng>
    concept reverse_endable_select_rend =
        reverse_endable_range<Rng> and
        has_member_rend<Rng>;

    template <typename Rng>
    concept reverse_endable_select_std_rend =
        reverse_endable_range<Rng> and
        has_std_rend<Rng> and
        not has_member_rend<Rng>;
}


namespace genex::iterators {
    struct begin_fn {
        template <typename Rng>
        requires detail::concepts::beginable_select_begin<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return rng.begin();
        }

        template <typename Rng>
        requires detail::concepts::beginable_select_std_begin<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return std::begin(std::forward<Rng>(rng));
        }
    };

    struct end_fn {
        template <typename Rng>
        requires detail::concepts::endable_select_end<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return rng.end();
        }

        template <typename Rng>
        requires detail::concepts::endable_select_std_end<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return std::end(std::forward<Rng>(rng));
        }
    };

    struct rbegin_fn {
        template <typename Rng>
        requires detail::concepts::reverse_beginable_select_rbegin<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return rng.rbegin();
        }

        template <typename Rng>
        requires detail::concepts::reverse_beginable_select_std_rbegin<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return std::rbegin(std::forward<Rng>(rng));
        }
    };

    struct rend_fn {
        template <typename Rng>
        requires detail::concepts::reverse_endable_select_rend<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return rng.rend();
        }

        template <typename Rng>
        requires detail::concepts::reverse_endable_select_std_rend<Rng>
        GENEX_INLINE auto operator()(Rng &&rng) const {
            return std::rend(std::forward<Rng>(rng));
        }
    };


    export inline constexpr begin_fn begin{};
    export inline constexpr end_fn end{};
    export inline constexpr rbegin_fn rbegin{};
    export inline constexpr rend_fn rend{};
}
