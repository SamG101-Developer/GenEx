#pragma once
#include <iterator>
#include <memory>


namespace genex {
    template <typename Rng>
    struct iterator;

    template <typename Rng>
    struct sentinel;

    template <typename T>
    struct deref_value;

    template <typename I>
    using iter_value_t = std::iter_value_t<I>;

    template <typename Rng>
    using range_value_t = iter_value_t<typename iterator<Rng>::type>;

    template <typename Rng>
    using iterator_t = iterator<Rng>::type;

    template <typename Rng>
    using sentinel_t = sentinel<Rng>::type;

    template <typename T>
    using deref_value_t = deref_value<T>::type;

    template <typename T>
    using iter_reference_t = std::iter_reference_t<T>;

    template <typename T>
    using range_reference_t = iter_reference_t<iterator_t<T>>;

    template <typename T>
    using iter_difference_t = std::iter_difference_t<T>;

    template <std::size_t N, typename... Ts>
    struct nth_type;

    template <std::size_t N, typename... Ts>
    using nth_type_t = nth_type<N, Ts...>::type;
}


namespace genex {
    template <typename Rng>
    concept has_std_begin = requires(Rng &rng) { { std::begin(std::forward<Rng>(rng)) } -> std::input_or_output_iterator; };

    template <typename Rng>
    concept has_std_end = requires(Rng &rng) { { std::end(std::forward<Rng>(rng)) } -> std::sentinel_for<decltype(std::begin(std::forward<Rng>(rng)))>; };

    template <typename Rng>
    concept has_std_rbegin = requires(Rng &rng) { { std::rbegin(std::forward<Rng>(rng)) } -> std::input_or_output_iterator; };

    template <typename Rng>
    concept has_std_rend = requires(Rng &rng) { { std::rend(std::forward<Rng>(rng)) } -> std::sentinel_for<decltype(std::rbegin(std::forward<Rng>(rng)))>; };

    template <typename Rng>
    concept has_std_size = requires(Rng &rng) { std::size(rng); };


    template <typename Rng>
    concept has_member_begin = requires(Rng &rng) { { rng.begin() }; };

    template <typename Rng>
    concept has_member_end = requires(Rng &rng) { { rng.end() }; };

    template <typename Rng>
    concept has_member_rbegin = requires(Rng &rng) { { rng.rbegin() }; };

    template <typename Rng>
    concept has_member_rend = requires(Rng &rng) { { rng.rend() }; };


    template <typename Rng>
    concept has_member_size = requires(Rng &rng) { rng.size(); };

    template <typename Rng>
    concept has_member_empty = requires(Rng &rng) { rng.empty(); };

    template <typename Rng>
    concept has_member_at = requires(Rng &rng) { rng.at(std::declval<std::size_t>()); };

    template <typename Rng>
    concept has_member_front = requires(Rng &rng) { rng.front(); };

    template <typename Rng>
    concept has_member_back = requires(Rng &rng) { rng.back(); };

    template <typename Rng>
    concept has_member_push_back = requires(Rng &rng) { rng.push_back(std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_emplace_back = requires(Rng &rng) { rng.emplace_back(std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_push_front = requires(Rng &rng) { rng.push_front(std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_emplace_front = requires(Rng &rng) { rng.emplace_front(std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_push = requires(Rng &rng) { rng.push(std::declval<iterator_t<Rng>>(), std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_emplace = requires(Rng &rng) { rng.emplace(std::declval<iterator_t<Rng>>(), std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_insert = requires(Rng &rng) { rng.insert(std::declval<iterator_t<Rng>>(), std::declval<range_value_t<Rng>>()); };

    template <typename Rng>
    concept has_member_pop_back = requires(Rng &rng) { rng.pop_back(); };

    template <typename Rng>
    concept has_member_pop_front = requires(Rng &rng) { rng.pop_front(); };

    template <typename Rng>
    concept has_member_pop = requires(Rng &rng) { rng.pop(std::declval<std::size_t>()); };

    template <typename Rng>
    concept has_member_erase = requires(Rng &rng) { rng.erase(std::declval<iterator_t<Rng>>()); };
}


namespace genex {
    template <typename Rng>
    concept range =
        (has_std_begin<Rng> or has_member_begin<Rng>) and
        (has_std_end<Rng> or has_member_end<Rng>);
}


namespace genex::views::detail {
    template <typename Rng> requires range<Rng>
    struct view_base;
}


namespace genex {
    template <typename Rng>
    concept input_range = range<Rng> and std::input_iterator<iterator_t<Rng>>;

    template <typename Rng>
    concept forward_range = input_range<Rng> and std::forward_iterator<iterator_t<Rng>>;

    template <typename Rng>
    concept bidirectional_range = forward_range<Rng> and std::bidirectional_iterator<iterator_t<Rng>>;

    template <typename Rng>
    concept random_access_range = bidirectional_range<Rng> and std::random_access_iterator<iterator_t<Rng>>;

    template <typename Rng>
    concept contiguous_range = random_access_range<Rng> and std::contiguous_iterator<iterator_t<Rng>>;
}


namespace genex {
    template <std::size_t I = 0, typename... Ts, typename... Us> requires (I <= sizeof...(Ts) and sizeof...(Ts) == sizeof...(Us))
    auto any_iterator_finished(std::tuple<Ts...> &starts, const std::tuple<Us...> &ends) -> bool {
        if constexpr (I < sizeof...(Ts)) {
            return std::get<I>(starts) == std::get<I>(ends) || any_iterator_finished<I + 1>(starts, ends);
        }
        else {
            return false;
        }
    }

    template <typename Tuple, std::size_t... Is>
    auto deref_tuple_impl(Tuple &t, std::index_sequence<Is...>) -> auto {
        return std::tuple<iter_value_t<std::tuple_element_t<Is, Tuple>>...>{(*std::get<Is>(t))...};
    }

    template <typename Tuple>
    auto deref_tuple(Tuple &t) -> auto {
        return deref_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }

    template <typename Tuple, std::size_t... Is>
    auto advance_tuple_impl(Tuple &t, std::index_sequence<Is...>) -> void {
        ((++std::get<Is>(t)), ...);
    }

    template <typename Tuple>
    auto advance_tuple(Tuple &t) -> void {
        advance_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }

    template <typename Tuple, std::size_t... Is>
    auto tuple_tail_impl(Tuple &&t, std::index_sequence<Is...>) -> auto {
        return std::make_tuple(std::get<Is + 1>(std::forward<Tuple>(t))...);
    }

    template <typename Tuple>
    auto tuple_tail(Tuple &&t) -> auto {
        return tuple_tail_impl(std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>> - 1>{});
    }

    template <typename Tuple>
    auto tuple_head(Tuple &&t) -> auto {
        return std::make_tuple(std::get<0>(std::forward<Tuple>(t)));
    }
}


namespace genex {
    template <typename T>
    struct is_unique_ptr : std::false_type {
    };

    template <typename T, typename Deleter>
    struct is_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {
    };

    template <typename T>
    concept unique_ptr = is_unique_ptr<std::remove_cvref_t<T>>::value;

    template <typename T>
    struct is_shared_ptr : std::false_type {
    };

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {
    };

    template <typename T>
    concept shared_ptr = is_shared_ptr<std::remove_cvref_t<T>>::value;

    template <typename T>
    struct is_weak_ptr : std::false_type {
    };

    template <typename T>
    struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {
    };

    template <typename T>
    concept weak_ptr = is_weak_ptr<std::remove_cvref_t<T>>::value;

    template <typename T>
    struct is_tuple_like : std::false_type {
    };

    template <typename... Ts>
    struct is_tuple_like<std::tuple<Ts...>> : std::true_type {
    };

    template <typename T>
    concept tuple_like = is_tuple_like<std::remove_cvref_t<T>>::value;

    template <typename T>
    concept integer_like = std::is_integral_v<std::remove_cvref_t<T>>;
}


template <typename Rng> requires genex::has_std_begin<Rng>
struct genex::iterator<Rng> {
    using type = decltype(std::begin(std::declval<Rng>()));
};


template <typename Rng> requires (not genex::has_std_begin<Rng> and genex::has_member_begin<Rng>)
struct genex::iterator<Rng> {
    using type = decltype(std::declval<Rng>().begin());
};


template <typename Rng> requires genex::has_std_end<Rng>
struct genex::sentinel<Rng> {
    using type = decltype(std::end(std::declval<Rng>()));
};


template <typename Rng> requires (not genex::has_std_end<Rng> and genex::has_member_end<Rng>)
struct genex::sentinel<Rng> {
    using type = decltype(std::declval<Rng>().end());
};


template <typename T> requires requires(T &&t) { *std::declval<T>(); }
struct genex::deref_value<T> {
    using type = decltype(*std::declval<T>());
};


template <typename First, typename... Rest>
struct genex::nth_type<0, First, Rest...> {
    using type = First;
};


template <std::size_t N, typename First, typename... Rest>
struct genex::nth_type<N, First, Rest...> {
    static_assert(N < 1 + sizeof...(Rest), "Index out of bounds");
    using type = nth_type<N - 1, Rest...>::type;
};
