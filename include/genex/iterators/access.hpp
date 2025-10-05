#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::iterators::concepts {
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

    // template <typename Rng>
    // concept beginable_select_std_ranges_begin =
    //     beginable_range<Rng> and
    //     requires(Rng &rng) { std::ranges::begin(rng); } and
    //     not has_member_begin<Rng> and
    //     not has_std_begin<Rng>;


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

    // template <typename Rng>
    // concept reverse_beginable_select_std_ranges_rbegin =
    //     reverse_beginable_range<Rng> and
    //     requires(Rng &rng) { std::ranges::rbegin(rng); } and
    //     not has_member_rbegin<Rng> and
    //     not has_std_rbegin<Rng>;


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

    // template <typename Rng>
    // concept endable_select_std_ranges_end =
    //     endable_range<Rng> and
    //     requires(Rng &rng) { std::ranges::end(rng); } and
    //     not has_member_end<Rng> and
    //     not has_std_end<Rng>;


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

    // template <typename Rng>
    // concept reverse_endable_select_std_ranges_rend =
    //     reverse_endable_range<Rng> and
    //     requires(Rng &rng) { std::ranges::rend(rng); } and
    //     not has_member_rend<Rng> and
    //     not has_std_rend<Rng>;
}


namespace genex::iterators {
    struct begin_fn {
        template <typename Rng>
        requires concepts::beginable_select_begin<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.begin())) -> auto {
            return rng.begin();
        }

        template <typename Rng>
        requires concepts::beginable_select_std_begin<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::begin(rng))) -> auto {
            return std::begin(std::forward<Rng>(rng));
        }

        // template <typename Rng>
        // requires concepts::beginable_select_std_ranges_begin<Rng>
        // GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::ranges::begin(rng))) -> auto {
        //     return std::ranges::begin(std::forward<Rng>(rng));
        // }
    };

    struct end_fn {
        template <typename Rng>
        requires concepts::endable_select_end<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.end())) -> auto {
            return rng.end();
        }

        template <typename Rng>
        requires concepts::endable_select_std_end<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::end(rng))) -> auto {
            return std::end(std::forward<Rng>(rng));
        }

        // template <typename Rng>
        // requires concepts::endable_select_std_ranges_end<Rng>
        // GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::ranges::end(rng))) -> auto {
        //     return std::ranges::end(std::forward<Rng>(rng));
        // }
    };

    struct rbegin_fn {
        template <typename Rng>
        requires concepts::reverse_beginable_select_rbegin<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.rbegin())) -> auto {
            return rng.rbegin();
        }

        template <typename Rng>
        requires concepts::reverse_beginable_select_std_rbegin<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::rbegin(rng))) -> auto {
            return std::rbegin(std::forward<Rng>(rng));
        }

        // template <typename Rng>
        // requires concepts::reverse_beginable_select_std_ranges_rbegin<Rng>
        // GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::ranges::rbegin(rng))) -> auto {
        //     return std::ranges::rbegin(std::forward<Rng>(rng));
        // }
    };

    struct rend_fn {
        template <typename Rng>
        requires concepts::reverse_endable_select_rend<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(rng.rend())) -> auto {
            return rng.rend();
        }

        template <typename Rng>
        requires concepts::reverse_endable_select_std_rend<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::rend(rng))) -> auto {
            return std::rend(std::forward<Rng>(rng));
        }

        // template <typename Rng>
        // requires concepts::reverse_endable_select_std_ranges_rend<Rng>
        // GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(noexcept(std::ranges::rend(rng))) -> auto {
        //     return std::ranges::rend(std::forward<Rng>(rng));
        // }
    };


    inline constexpr begin_fn begin{};
    inline constexpr end_fn end{};
    inline constexpr rbegin_fn rbegin{};
    inline constexpr rend_fn rend{};
}
