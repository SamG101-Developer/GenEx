export module genex.concepts;
import std;

export namespace genex {
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

  template <typename T>
  using range_difference_t = iter_difference_t<iterator_t<T>>;

  template <typename T>
  using iter_element_t = std::remove_reference_t<iter_reference_t<T>>;

  template <typename T>
  using range_element_t = iter_element_t<iterator_t<T>>;
}

export namespace genex {
  template <typename Rng>
  concept has_std_begin = requires(Rng &rng) {
    { std::begin(std::forward<Rng>(rng)) } -> std::input_or_output_iterator;
  };

  template <typename Rng>
  concept has_std_end = requires(Rng &rng) {
    {
      std::end(std::forward<Rng>(rng))
    } -> std::sentinel_for<decltype(std::begin(std::forward<Rng>(rng)))>;
  };

  template <typename Rng>
  concept has_std_rbegin = requires(Rng &rng) {
    { std::rbegin(std::forward<Rng>(rng)) } -> std::input_or_output_iterator;
  };

  template <typename Rng>
  concept has_std_rend = requires(Rng &rng) {
    {
      std::rend(std::forward<Rng>(rng))
    } -> std::sentinel_for<decltype(std::rbegin(std::forward<Rng>(rng)))>;
  };

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
  concept has_member_reserve = requires(Rng &rng) {
    rng.reserve(std::declval<std::size_t>());
  };

  template <typename Rng>
  concept has_member_reserve_hint = requires(Rng &rng) { rng.reserve_hint(); };

  template <typename Rng>
  concept has_member_empty = requires(Rng &rng) { rng.empty(); };

  template <typename Rng>
  concept has_member_push_back = requires(Rng &rng) {
    rng.push_back(std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_emplace_back = requires(Rng &rng) {
    rng.emplace_back(std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_push_front = requires(Rng &rng) {
    rng.push_front(std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_emplace_front = requires(Rng &rng) {
    rng.emplace_front(std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_push = requires(Rng &rng) {
    rng.push(std::declval<iterator_t<Rng>>(),
             std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_emplace = requires(Rng &rng) {
    rng.emplace(std::declval<iterator_t<Rng>>(),
                std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_insert = requires(Rng &rng) {
    rng.insert(std::declval<iterator_t<Rng>>(),
               std::declval<range_value_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_pop_back = requires(Rng &rng) { rng.pop_back(); };

  template <typename Rng>
  concept has_member_pop_front = requires(Rng &rng) { rng.pop_front(); };

  template <typename Rng>
  concept has_member_pop = requires(Rng &rng) {
    rng.pop(std::declval<std::size_t>());
  };

  template <typename Rng>
  concept has_member_erase = requires(Rng &rng) {
    rng.erase(std::declval<iterator_t<Rng>>());
  };

  template <typename Rng>
  concept has_member_clear = requires(Rng &rng) { rng.clear(); };
}

export namespace genex {
  template <typename Rng>
  concept range =
    (has_std_begin<Rng> or has_member_begin<Rng>) and
    (has_std_end<Rng> or has_member_end<Rng>);

  /**
   * The base of every view. A view is cheap to move, and the iterators it hands out hold copies of what they use, so
   * they stay valid after the view itself is gone.
   */
  struct view_base {
  };

  template <typename T>
  inline constexpr bool enable_view = std::derived_from<T, view_base>;

  template <typename CharT, typename Traits>
  inline constexpr bool enable_view<std::basic_string_view<CharT, Traits>> = true;

  template <typename T, std::size_t Extent>
  inline constexpr bool enable_view<std::span<T, Extent>> = true;

  /**
   * Whether a range's iterators stay valid after the range itself is gone, because it does not own what they point to.
   */
  template <typename T>
  inline constexpr bool enable_borrowed_range = false;

  template <typename CharT, typename Traits>
  inline constexpr bool enable_borrowed_range<std::basic_string_view<CharT, Traits>> = true;

  template <typename T, std::size_t Extent>
  inline constexpr bool enable_borrowed_range<std::span<T, Extent>> = true;

  /**
   * A range a view can safely point into: an lvalue, or a range whose iterators do not depend on it staying alive. Any
   * other rvalue would dangle, so a view takes one through the pipe operator, which keeps it alive.
   */
  template <typename Rng>
  concept non_dangling_range =
    std::is_lvalue_reference_v<Rng> or
    enable_view<std::remove_cvref_t<Rng>> or
    enable_borrowed_range<std::remove_cvref_t<Rng>>;

  /**
   * What an algorithm returns instead of an iterator into an rvalue range that is about to be destroyed. It can't be
   * dereferenced, so misusing it is a compile error.
   */
  struct dangling {
    constexpr dangling() noexcept = default;

    template <typename... Args>
    constexpr dangling(Args &&...) noexcept {
    }
  };

  template <typename Rng>
  using borrowed_iterator_t = std::conditional_t<non_dangling_range<Rng>, iterator_t<Rng>, dangling>;

  /**
   * An element of a range: a reference where the range outlives the call, otherwise a copy.
   */
  template <typename Rng>
  using borrowed_reference_t = std::conditional_t<non_dangling_range<Rng>, range_reference_t<Rng>, range_value_t<Rng>>;

  /**
   * The strongest tag an iterator models, for an adaptor's @c iterator_concept.
   */
  template <typename I>
  using iter_strength_t =
    std::conditional_t<std::random_access_iterator<I>, std::random_access_iterator_tag,
    std::conditional_t<std::bidirectional_iterator<I>, std::bidirectional_iterator_tag,
    std::conditional_t<std::forward_iterator<I>, std::forward_iterator_tag, std::input_iterator_tag>>>;

  /**
   * An adaptor's @c iterator_category, which legacy algorithms dispatch on. Legacy forward iterators and above must
   * return a true reference, so an iterator returning a prvalue can only claim to be an input iterator there, whatever
   * @c iterator_concept says it models.
   */
  template <typename Reference, typename Concept>
  using legacy_iter_category_t =
    std::conditional_t<std::is_lvalue_reference_v<Reference>, Concept, std::input_iterator_tag>;
}

export namespace genex {
  template <typename Rng>
  concept input_range = range<Rng>
    and std::input_iterator<iterator_t<Rng>>;

  template <typename Rng>
  concept forward_range = input_range<Rng>
    and std::forward_iterator<iterator_t<Rng>>;

  template <typename Rng>
  concept bidirectional_range = forward_range<Rng>
    and std::bidirectional_iterator<iterator_t<Rng>>;

  template <typename Rng>
  concept random_access_range = bidirectional_range<Rng>
    and std::random_access_iterator<iterator_t<Rng>>;

  template <typename Rng>
  concept contiguous_range = random_access_range<Rng>
    and std::contiguous_iterator<iterator_t<Rng>>;

  template <typename Rng>
  concept sized_range = input_range<Rng>
    and std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>;
}

export namespace genex {
  template <std::size_t I = 0, typename... Ts, typename... Us>
    requires (I <= sizeof...(Ts) and sizeof...(Ts) == sizeof...(Us))
  constexpr auto any_iterator_finished(const std::tuple<Ts...> &starts,
    const std::tuple<Us...> &ends) -> bool {
    if constexpr (I < sizeof...(Ts)) {
      return std::get<I>(starts) == std::get<I>(ends) or any_iterator_finished<I
        + 1>(starts, ends);
    }
    else {
      return false;
    }
  }

  template <typename Tuple, std::size_t... Is>
  constexpr auto
  deref_tuple_impl(Tuple &t, std::index_sequence<Is...>) -> auto {
    return std::tuple<iter_reference_t<std::tuple_element_t<Is, Tuple>>...>{
      (*std::get<Is>(t))...
    };
  }

  template <typename Tuple>
  constexpr auto deref_tuple(Tuple &t) -> auto {
    return deref_tuple_impl(
      t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, std::size_t... Is>
  constexpr auto advance_tuple_impl(Tuple &t,
    std::index_sequence<Is...>) -> void {
    ((++std::get<Is>(t)), ...);
  }

  template <typename Tuple>
  constexpr auto advance_tuple(Tuple &t) -> void {
    advance_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, typename Diff, std::size_t... Is>
  constexpr auto advance_tuple_impl(Tuple &t, Diff n,
    std::index_sequence<Is...>) -> void {
    ((std::get<Is>(t) += n), ...);
  }

  template <typename Tuple, typename Diff>
  constexpr auto advance_tuple(Tuple &t, Diff n) -> void {
    advance_tuple_impl(
      t, n, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, std::size_t... Is>
  constexpr auto retreat_tuple_impl(Tuple &t,
    std::index_sequence<Is...>) -> void {
    ((--std::get<Is>(t)), ...);
  }

  template <typename Tuple>
  constexpr auto retreat_tuple(Tuple &t) -> void {
    retreat_tuple_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple, typename Diff, std::size_t... Is>
  constexpr auto retreat_tuple_impl(Tuple &t, Diff n,
    std::index_sequence<Is...>) -> void {
    ((std::get<Is>(t) -= n), ...);
  }

  template <typename Tuple, typename Diff>
  constexpr auto retreat_tuple(Tuple &t, Diff n) -> void {
    retreat_tuple_impl(
      t, n, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

  template <typename Tuple>
    requires (std::tuple_size_v<Tuple> > 0)
  constexpr auto distance_tuple(const Tuple &a, const Tuple &b) -> auto {
    return std::get<0>(a) - std::get<0>(b);
  }
}

export namespace genex {
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

  template <typename C>
  concept strict_char_like =
    std::same_as<std::remove_cv_t<C>, char> or
    std::same_as<std::remove_cv_t<C>, signed char> or
    std::same_as<std::remove_cv_t<C>, unsigned char>;

  template <typename C>
  concept wide_char_like =
    std::same_as<std::remove_cv_t<C>, wchar_t>;

  template <typename C>
  concept utf_char_like =
#if __cpp_char8_t >= 201811L
    std::same_as<std::remove_cv_t<C>, char8_t> or
#endif
    std::same_as<std::remove_cv_t<C>, char16_t> or
    std::same_as<std::remove_cv_t<C>, char32_t>;

  template <typename C>
  concept char_like =
    strict_char_like<C> or
    wide_char_like<C> or
    utf_char_like<C>;

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
  using type = decltype(std::begin(std::declval<Rng&>()));
};

template <typename Rng>
  requires (not genex::has_std_begin<Rng> and genex::has_member_begin<Rng>)
struct genex::iterator<Rng> {
  using type = decltype(std::declval<Rng&>().begin());
};

template <typename Rng> requires genex::has_std_end<Rng>
struct genex::sentinel<Rng> {
  using type = decltype(std::end(std::declval<Rng&>()));
};

template <typename Rng>
  requires (not genex::has_std_end<Rng> and genex::has_member_end<Rng>)
struct genex::sentinel<Rng> {
  using type = decltype(std::declval<Rng&>().end());
};

template <typename T> requires requires(T &&t) { *std::declval<T>(); }
struct genex::deref_value<T> {
  using type = decltype(*std::declval<T>());
};
