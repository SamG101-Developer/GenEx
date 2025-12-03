module;
#include <genex/macros.hpp>

export module genex.actions.insert;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import std;


namespace genex::actions::detail::concepts {
    template <typename Rng, typename I, typename E>
    concept insertable_range =
        input_range<Rng> and
        std::same_as<std::remove_cvref_t<I>, iterator_t<Rng>> and
        std::constructible_from<range_value_t<Rng>, E&&>;

    template <typename Rng, typename I, typename E>
    concept insertable_select_emplace =
        insertable_range<Rng, I, E> and
        has_member_emplace<Rng>;

    template <typename Rng, typename I, typename E>
    concept insertable_select_push =
        insertable_range<Rng, I, E> and
        has_member_push<Rng> and
        not has_member_emplace<Rng>;

    template <typename Rng, typename I, typename E>
    concept insertable_select_insert =
        insertable_range<Rng, I, E> and
        has_member_insert<Rng> and
        not has_member_push<Rng> and
        not has_member_emplace<Rng>;
}


namespace genex::actions {
    struct insert_fn {
        template <typename Rng, typename I, typename E>
        requires detail::concepts::insertable_select_emplace<Rng, I, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, I it, E elem) const -> decltype(auto) {
            GENEX_ASSERT(std::out_of_range, it >= std::begin(rng) and it <= std::end(rng));
            rng.emplace(std::move(it), std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename I, typename E>
        requires detail::concepts::insertable_select_push<Rng, I, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, I it, E elem) const -> decltype(auto) {
            GENEX_ASSERT(std::out_of_range, it >= std::begin(rng) and it <= std::end(rng));
            rng.push(std::move(it), std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename Rng, typename I, typename E>
        requires detail::concepts::insertable_select_insert<Rng, I, E>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, I it, E elem) const -> decltype(auto) {
            GENEX_ASSERT(std::out_of_range, it >= std::begin(rng) and it <= std::end(rng));
            rng.insert(std::move(it), std::move(elem));
            return std::forward<Rng>(rng);
        }

        template <typename I, typename E>
        requires (not range<I>)
        GENEX_INLINE constexpr auto operator()(I it, E elem) const {
            return meta::bind_back(insert_fn{}, std::move(it), std::move(elem));
        }
    };

    export inline constexpr insert_fn insert{};
}
