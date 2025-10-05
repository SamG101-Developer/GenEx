#pragma once
#include <iterator>
#include <memory>
#include <ranges>


namespace genex {
    template <typename Rng>
    struct iterator;

    template <typename Rng>
    struct sentinel;

    template <typename T>
    struct deref_value;

    template <typename... Ts>
    struct difference_type_selector;

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

    template <typename T>
    using range_difference_t = iter_difference_t<iterator_t<T>>;

    template <typename... Ts>
    using difference_type_selector_t = difference_type_selector<Ts...>::type;

    template <typename Cat>
    struct iterator_category_rank;

    template <int N>
    struct iterator_category_from_rank;

    template <typename Cat1, typename Cat2>
    struct common_iterator_category {
        static constexpr int rank1 = iterator_category_rank<Cat1>::value;
        static constexpr int rank2 = iterator_category_rank<Cat2>::value;
        static constexpr int min_rank = rank1 < rank2 ? rank1 : rank2;
        using type = iterator_category_from_rank<min_rank>::type;
    };

    template <typename Cat1, typename Cat2>
    using common_iterator_category_t = common_iterator_category<Cat1, Cat2>::type;
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
    concept has_member_data = requires(Rng &rng) { rng.data(); };

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

    template <typename Rng>
    concept has_member_clear = requires(Rng &rng) { rng.clear(); };
}


namespace genex {
    template <typename Rng>
    concept range =
        (has_std_begin<Rng> or has_member_begin<Rng>) and
        (has_std_end<Rng> or has_member_end<Rng>);
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

    template <typename Rng>
    concept borrowed_range = std::is_lvalue_reference_v<Rng> or std::is_const_v<std::remove_reference_t<Rng>>;

    template <typename Rng>
    concept viewable_range = input_range<Rng> and (borrowed_range<Rng> or std::is_rvalue_reference_v<Rng>);

    template <typename W>
    concept weakly_incrementable =
        std::movable<W> and
        requires(W w) {
            { ++w } -> std::same_as<W&>;
            typename iter_difference_t<W>;
            requires std::is_signed_v<iter_difference_t<W>>;
        };

    template <typename W>
    concept incrementable =
        weakly_incrementable<W> and
        std::regular<W> and
        requires(W w) {
            { w++ } -> std::same_as<W>;
        };

    template <typename W>
    concept decrementable =
        incrementable<W> and
        requires(W w) {
            { --w } -> std::same_as<W&>;
            { w-- } -> std::same_as<W>;
        };

    template <typename W>
    concept advancable =
        decrementable<W> and
        requires(W w, iter_difference_t<W> n) {
            { w += n } -> std::same_as<W&>;
            { w -= n } -> std::same_as<W&>;
            { w + n } -> std::same_as<W>;
            { n + w } -> std::same_as<W>;
            { w - n } -> std::same_as<W>;
            { w - w } -> std::same_as<iter_difference_t<W>>;
        };
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

    template <typename To, typename smart_ptr>
    struct smart_pointer_cast;

    template <typename To, typename smart_ptr>
    using smart_pointer_cast_t = typename smart_pointer_cast<To, smart_ptr>::type;

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

    template <typename T>
    struct is_pair_like : std::false_type {
    };

    template <typename T1, typename T2>
    struct is_pair_like<std::pair<T1, T2>> : std::true_type {
    };

    template <typename T>
    concept pair_like = is_pair_like<std::remove_cvref_t<T>>::value;
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


template <typename T> requires std::input_iterator<T>
struct genex::difference_type_selector<T> {
    using type = iter_difference_t<T>;
};


template <typename T> requires genex::input_range<T>
struct genex::difference_type_selector<T> {
    using type = range_difference_t<T>;
};


template <typename T, typename U, typename... Ts>
struct genex::difference_type_selector<T, U, Ts...> {
    using type = std::common_type_t<iter_difference_t<T>, iter_difference_t<U>, iter_difference_t<Ts>...>;
};


template <typename To, typename T, typename D>
struct genex::smart_pointer_cast<std::unique_ptr<T, D>, To> {
    using type = std::unique_ptr<To, D>;
};


template <typename To, typename T>
struct genex::smart_pointer_cast<std::shared_ptr<T>, To> {
    using type = std::shared_ptr<To>;
};


template <typename To, typename T>
struct genex::smart_pointer_cast<std::weak_ptr<T>, To> {
    using type = std::weak_ptr<To>;
};


template <>
struct genex::iterator_category_rank<std::input_iterator_tag> : std::integral_constant<int, 0> {
};

template <>
struct genex::iterator_category_rank<std::forward_iterator_tag> : std::integral_constant<int, 1> {
};

template <>
struct genex::iterator_category_rank<std::bidirectional_iterator_tag> : std::integral_constant<int, 2> {
};

template <>
struct genex::iterator_category_rank<std::random_access_iterator_tag> : std::integral_constant<int, 3> {
};

template <>
struct genex::iterator_category_rank<std::contiguous_iterator_tag> : std::integral_constant<int, 4> {
};

template <>
struct genex::iterator_category_from_rank<0> {
    using type = std::input_iterator_tag;
};

template <>
struct genex::iterator_category_from_rank<1> {
    using type = std::forward_iterator_tag;
};

template <>
struct genex::iterator_category_from_rank<2> {
    using type = std::bidirectional_iterator_tag;
};

template <>
struct genex::iterator_category_from_rank<3> {
    using type = std::random_access_iterator_tag;
};

template <>
struct genex::iterator_category_from_rank<4> {
    using type = std::contiguous_iterator_tag;
};
