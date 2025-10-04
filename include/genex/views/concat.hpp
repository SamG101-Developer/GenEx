#pragma once
#include <ranges>
#include <tuple>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


namespace genex::views::detail::concepts {
    template <typename T1, typename T2>
    struct concatenatable_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct concatenatable_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (std::input_iterator<Is> and ...) and
            (std::sentinel_for<Ss, Is> and ...) and
            requires { typename std::common_type_t<iter_value_t<Is>...>; };
    };

    template <typename T1, typename T2>
    concept concatenatable_iters =
        concatenatable_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept concatenatable_range =
        (input_range<Rngs> and ...) and
        concatenatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views::detail {
    template <typename Tuple>
    constexpr decltype(auto) tuple_at_index(Tuple& t, std::size_t index) {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) {
            using result_t = std::common_reference_t<decltype((std::get<Is>(t)))...>;
            const void* ptr = nullptr;
            ((index == Is ? (ptr = static_cast<const void*>(std::addressof(std::get<Is>(t))), true) : false) || ...);
            return *static_cast<std::remove_reference_t<result_t>*>(const_cast<void*>(ptr));
        }(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }


    template <typename ...Rngs>
    struct concat_iterator {
        using reference = std::common_reference_t<range_reference_t<Rngs>...>;
        using value_type = std::common_type_t<range_value_t<Rngs>...>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_concept =
            std::conditional_t<(std::random_access_iterator<iterator_t<Rngs>> && ...), std::random_access_iterator_tag,
            std::conditional_t<(std::bidirectional_iterator<iterator_t<Rngs>> && ...), std::bidirectional_iterator_tag,
            std::conditional_t<(std::forward_iterator<iterator_t<Rngs>> && ...), std::forward_iterator_tag, std::input_iterator_tag>>>;

        using iterator_category = iterator_concept;
        using difference_type = std::ptrdiff_t;

        constexpr concat_iterator() = default;

        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;
        std::size_t index = 0;

        constexpr explicit concat_iterator(std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts) :
            its(std::move(its)), sts(std::move(sts)) {
        }

        constexpr auto operator*() const noexcept
            -> reference {
            return *tuple_at_index(its, index);
        }

        constexpr auto operator->() const noexcept
            -> pointer {
            return std::addressof(*tuple_at_index(its, index));
        }

        constexpr auto operator++()
            -> concat_iterator& {
            auto &&it = tuple_at_index(its, index);
            auto &&st = tuple_at_index(sts, index);
            if (it != st) {
                ++it;
            }
            if (it == st && index + 1 < sizeof...(Rngs)) {
                ++index;
            }

            return *this;
        }

        constexpr auto operator++(int)
            -> concat_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    template <typename ...Rngs>
    struct concat_sentinel {
        std::tuple<sentinel_t<Rngs>...> s;

        constexpr concat_sentinel() = default;
        constexpr explicit concat_sentinel(std::tuple<sentinel_t<Rngs>...> s) :
            s(std::move(s)) {
        }

        GENEX_INLINE constexpr auto operator==(concat_iterator<Rngs...> const &it) const -> bool {
            return it.index == sizeof...(Rngs) - 1 && tuple_at_index(it.its, it.index) == tuple_at_index(s, it.index);
        }

        friend constexpr auto operator==(concat_iterator<Rngs...> const &it, concat_sentinel const &concat_sentinel) -> bool {
            return concat_sentinel == it;
        }
    };

    template <typename ...Vs>
    requires (concepts::concatenatable_range<Vs...> and sizeof...(Vs) > 1)
    struct concat_view : std::ranges::view_interface<concat_view<Vs...>> {
        constexpr concat_view() = default;;

        using iterator = concat_iterator<Vs...>;
        using sentinel = concat_sentinel<Vs...>;

        GENEX_INLINE constexpr auto begin() noexcept((std::is_nothrow_copy_constructible_v<iterator_t<Vs>> && ...))
            -> iterator {
            return iterator{
                std::make_tuple(iterators::begin(std::get<Vs>(rngs))...),
                std::make_tuple(iterators::end(std::get<Vs>(rngs))...)};
        }

        GENEX_INLINE constexpr auto begin() const noexcept((std::is_nothrow_copy_constructible_v<iterator_t<Vs>> && ...))
            -> iterator requires (range<const Vs> && ...) {
            return iterator{
                std::make_tuple(iterators::begin(std::get<Vs>(rngs))...),
                std::make_tuple(iterators::end(std::get<Vs>(rngs))...)};
        }

        GENEX_INLINE constexpr auto end() noexcept((std::is_nothrow_copy_constructible_v<iterator_t<Vs>> && ...))
            -> sentinel {
            return sentinel{
                std::make_tuple(iterators::end(std::get<Vs>(rngs))...)};
        }

        GENEX_INLINE constexpr auto end() const noexcept((std::is_nothrow_copy_constructible_v<iterator_t<Vs>> && ...))
            -> sentinel requires (range<Vs*> && ...) {
            return sentinel{
                std::make_tuple(iterators::end(std::get<Vs>(rngs))...)};
        }

        std::tuple<Vs...> rngs;

        constexpr explicit concat_view(Vs ...rngs) :
            rngs(std::move(rngs)...) {
        }
    };
}


namespace genex::views {
    struct concat_fn {
        template <typename ...Rng>
        requires (sizeof...(Rng) > 1 and (detail::concepts::concatenatable_range<Rng> && ...))
        GENEX_INLINE constexpr auto operator()(Rng &&...rng) const noexcept -> auto {
            return detail::concat_view<std::views::all_t<Rng>...>{
                std::views::all(std::forward<Rng>(rng))...};
        }

        template <typename Rng>
        requires detail::concepts::concatenatable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            return std::bind_back(concat_fn{}, std::forward<Rng>(rng));
        }
    };

    inline constexpr concat_fn concat{};
}
