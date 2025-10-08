#pragma once
#include <genex/pipe.hpp>
#include <genex/actions/insert.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng, typename E>
    concept back_insertable_range =
        range<Rng> and
        std::constructible_from<range_value_t<Rng>, E&&>;

    template <typename Rng, typename E>
    concept back_insertable_select_emplace_back =
        back_insertable_range<Rng, E> and
        has_member_emplace_back<Rng>;

    template <typename Rng, typename E>
    concept back_insertable_select_push_back =
        back_insertable_range<Rng, E> and
        has_member_push_back<Rng> and
        not has_member_emplace_back<Rng>;

    template <typename Rng, typename E>
    concept back_insertable_select_insert =
        back_insertable_range<Rng, E> and
        has_member_insert<Rng> and
        not has_member_push_back<Rng> and
        not has_member_emplace_back<Rng>;
}


namespace genex::actions {
    struct push_back_fn {
        template <typename Rng, typename E>
        requires detail::concepts::back_insertable_select_emplace_back<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const -> decltype(auto) {
            rng.emplace_back(std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename E>
        requires detail::concepts::back_insertable_select_push_back<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const -> decltype(auto) {
            rng.push_back(std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename E>
        requires detail::concepts::back_insertable_select_insert<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const -> decltype(auto) {
            actions::insert(rng, iterators::end(rng), std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename E>
        GENEX_INLINE constexpr auto operator()(E elem) const {
            return std::bind_back(push_back_fn{}, std::move(elem));
        }
    };

    inline constexpr push_back_fn push_back{};
}
