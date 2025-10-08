#pragma once
#include <genex/pipe.hpp>
#include <genex/actions/insert.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng, typename E>
    concept front_insertable_range =
        range<Rng> and
        std::constructible_from<range_value_t<Rng>, E&&>;

    template <typename Rng, typename E>
    concept front_insertable_select_emplace_front =
        front_insertable_range<Rng, E> and
        has_member_emplace_front<Rng>;

    template <typename Rng, typename E>
    concept front_insertable_select_push_front =
        front_insertable_range<Rng, E> and
        has_member_push_front<Rng> and
        not has_member_emplace_front<Rng>;

    template <typename Rng, typename E>
    concept front_insertable_select_insert =
        front_insertable_range<Rng, E> and
        has_member_insert<Rng> and
        not has_member_push_front<Rng> and
        not has_member_emplace_front<Rng>;
}


namespace genex::actions {
    struct push_front_fn {
        template <typename Rng, typename E>
        requires detail::concepts::front_insertable_select_emplace_front<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const -> decltype(auto) {
            rng.emplace_front(std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename E>
        requires detail::concepts::front_insertable_select_push_front<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const -> decltype(auto) {
            rng.push_front(std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename E>
        requires detail::concepts::front_insertable_select_insert<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E elem) const -> decltype(auto) {
            actions::insert(rng, iterators::begin(rng), std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename E>
        GENEX_INLINE constexpr auto operator()(E elem) const {
            return std::bind_back(push_front_fn{}, std::move(elem));
        }
    };

    inline constexpr push_front_fn push_front{};
}
