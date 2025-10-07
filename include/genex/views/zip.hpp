#pragma once
#include <tuple>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename T1, typename T2>
    struct zippable_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct zippable_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (std::input_iterator<Is> and ...) and
            (std::sentinel_for<Ss, Is> and ...) and
            (std::movable<Is> and ...);
    };

    template <typename T1, typename T2>
    concept zippable_iters = zippable_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept zippable_range =
        sizeof...(Rngs) > 0 and
        (input_range<Rngs> and ...) and
        zippable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views::detail {
    template <typename... Rngs>
    requires concepts::zippable_range<Rngs...>
    struct zip_iterator {
        static constexpr auto N = sizeof...(Rngs);

        using reference = std::tuple<range_reference_t<Rngs>...>;
        using value_type = std::tuple<range_value_t<Rngs>...>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_concept =
            std::conditional_t<(std::random_access_iterator<iterator_t<Rngs>> and ...), std::random_access_iterator_tag,
            std::conditional_t<(std::bidirectional_iterator<iterator_t<Rngs>> and ...), std::bidirectional_iterator_tag,
            std::conditional_t<(std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>>>;
        using iterator_category = iterator_concept;
        using difference_type = difference_type_selector_t<iterator_t<Rngs>...>;

        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;
        std::size_t index;

        GENEX_VIEW_ITERATOR_CTOR_DEFINITIONS(
            zip_iterator);

        GENEX_VIEW_ITERATOR_FUNC_DEFINITIONS(
            zip_iterator, its);

        GENEX_INLINE constexpr explicit zip_iterator(Rngs... rngs) :
            its(iterators::begin(rngs)...), sts(iterators::end(rngs)...), index(0) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept
            -> reference {
            return std::apply([](auto &... iters) { return std::tie(*iters...); }, its);
        }

        GENEX_INLINE constexpr auto operator->() const noexcept
            -> pointer {
            return std::apply([](auto &... iters) { return std::make_tuple(std::addressof(*iters)...); }, its);
        }

        GENEX_INLINE constexpr auto operator++() noexcept
            -> zip_iterator& {
            auto at_end = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((std::get<Is>(its) == std::get<Is>(sts)) || ...);
            }(std::index_sequence_for<Rngs...>{});
            if (at_end) { return *this; }

            std::apply([](auto &... iters) { ((void)++iters, ...); }, its);
            ++index;
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept
            -> zip_iterator {
            auto temp = *this;
            ++*this;
            return temp;
        }
    };

    struct zip_sentinel { };

    template <typename ...Rngs>
    requires concepts::zippable_range<Rngs...>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(zip_iterator, zip_sentinel, Rngs...) {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return ((std::get<Is>(it.its) == std::get<Is>(it.sts)) || ...);
        }(std::index_sequence_for<Rngs...>{});
    }

    template <typename... Vs>
    requires concepts::zippable_range<Vs...> and (sizeof...(Vs) > 1)
    struct zip_view : std::ranges::view_interface<zip_view<Vs...>> {
        std::tuple<Vs...> base_rngs;

        GENEX_INLINE constexpr explicit zip_view() noexcept = default;

        GENEX_INLINE constexpr explicit zip_view(Vs ...rngs) noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, Vs...>) :
            base_rngs(std::move(rngs)...) {
        }

        GENEX_INLINE constexpr auto begin() {
            return std::apply([](auto &... rngs) { return zip_iterator{rngs...}; }, base_rngs);
        }

        GENEX_INLINE constexpr auto begin() const requires range<const decltype(base_rngs)> {
            return std::apply([](auto &... rngs) { return zip_iterator{rngs...}; }, base_rngs);
        }

        GENEX_INLINE constexpr auto end() {
            return zip_sentinel{};
        }

        GENEX_INLINE constexpr auto end() const requires range<const decltype(base_rngs)> {
            return zip_sentinel{};
        }

        GENEX_INLINE constexpr auto size() const noexcept -> std::common_type_t<range_size_t<Vs>...> {
            return std::apply([](auto &... rngs) { return std::min({operations::size(rngs)...}); }, base_rngs);
        }
    };
}


namespace genex::views {
    struct zip_fn {
        template <typename ...Rng>
        requires (sizeof...(Rng) > 1 and (detail::concepts::zippable_range<Rng> and ...))
        GENEX_INLINE constexpr auto operator()(Rng &&...rng) const noexcept(
            meta::all_of_v<std::is_nothrow_move_constructible, Rng...>) {
            return detail::zip_view<std::views::all_t<Rng>...>{
                std::forward<Rng>(rng)...};
        }

        template <typename Rng>
        requires detail::concepts::zippable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept(
            meta::all_of_v<std::is_nothrow_default_constructible, zip_fn> and
            meta::all_of_v<std::is_nothrow_constructible, Rng&&>) {
            return std::bind_back(
                zip_fn{}, std::views::all(std::forward<Rng>(rng)));
        }
    };

    inline constexpr zip_fn zip{};
}
