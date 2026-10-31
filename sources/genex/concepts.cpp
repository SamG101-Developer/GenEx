export module genex.concepts;
import std;

export namespace genex {
  template <typename Rng>
  struct iterator;

  template <typename Rng>
  struct sentinel;

  template <typename I>
  using iter_value_t = std::iter_value_t<I>;

  template <typename Rng>
  using range_value_t = iter_value_t<typename iterator<Rng>::type>;

  template <typename Rng>
  using iterator_t = iterator<Rng>::type;

  template <typename Rng>
  using sentinel_t = sentinel<Rng>::type;

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
   * What a closure can take as its callable. A parameter by value decays an array, so without the check on pointers,
   * calling @c views::transform(array,f) directly would also match the closure taking @c f as a projection.
   */
  template <typename F>
  concept closure_callable =
    (not range<F>) and
    (not std::is_pointer_v<F> or std::is_function_v<std::remove_pointer_t<F>>);

  /**
   * What a closure can take as a value, which it has to see before any decay as a pointer is a perfectly good value. An
   * array is the range of a direct call instead, other than a string literal, which is a value to any range of strings.
   */
  template <typename E>
  concept closure_value =
    (not range<E>) or
    std::same_as<std::remove_extent_t<std::remove_reference_t<E>>, const char>;

  /// The base of every view. A view is cheap to move, and the iterators it
  /// hands out hold copies of what they use, so they stay valid after the view
  /// itself is gone.
  struct view_base {};

  template <typename T>
  inline constexpr bool enable_view = std::derived_from<T, view_base>;

  template <typename CharT, typename Traits>
  inline constexpr bool enable_view<std::basic_string_view<CharT, Traits>> = true;

  template <typename T, std::size_t Extent>
  inline constexpr bool enable_view<std::span<T, Extent>> = true;

  /// Whether a range's iterators stay valid after the range itself is gone,
  /// because it does not own what they point to.
  template <typename T>
  inline constexpr bool enable_borrowed_range = false;

  template <typename CharT, typename Traits>
  inline constexpr bool enable_borrowed_range<std::basic_string_view<CharT, Traits>> = true;

  template <typename T, std::size_t Extent>
  inline constexpr bool enable_borrowed_range<std::span<T, Extent>> = true;

  /// A range a view can safely point into: an lvalue, or a range whose
  /// iterators do not depend on it staying alive. Any other rvalue would
  /// dangle, so a view takes one through the pipe operator, which keeps it
  /// alive.
  template <typename Rng>
  concept non_dangling_range =
    std::is_lvalue_reference_v<Rng> or
    enable_view<std::remove_cvref_t<Rng>> or
    enable_borrowed_range<std::remove_cvref_t<Rng>>;

  /// What an algorithm returns instead of an iterator into an rvalue range that
  /// is about to be destroyed. It can't be dereferenced, so misusing it is a
  /// compile error.
  struct dangling {
    constexpr dangling() noexcept = default;

    template <typename... Args>
    constexpr dangling(Args &&...) noexcept {
    }
  };

  template <typename Rng>
  using borrowed_iterator_t = std::conditional_t<
    non_dangling_range<Rng>, iterator_t<Rng>, dangling>;

  /// An element of a range: a reference where the range outlives the call,
  /// otherwise a copy.
  template <typename Rng>
  using borrowed_reference_t = std::conditional_t<
    non_dangling_range<Rng>, range_reference_t<Rng>, range_value_t<Rng>>;

  /// The end of a range with no end of its own, which compares unequal to
  /// everything. Having its own type means a loop over one has no end check at
  /// all, and an adaptor that does end it, like "take", knows exactly how long
  /// it is.
  struct unreachable_sentinel {
    template <std::input_or_output_iterator I>
    constexpr friend auto operator==(
      const I &, unreachable_sentinel) noexcept -> bool {
      return false;
    }
  };

  /// A container that can be sized up front and written to directly, with
  /// elements that cost nothing to overwrite. A view's "append_to" can use this
  /// to write elements with an indexed loop, which the compiler can vectorize.
  template <typename Out, typename Reference>
  concept overwritable_with =
    std::is_trivially_copyable_v<typename Out::value_type> and
    std::assignable_from<typename Out::value_type&, Reference> and
    requires(Out &out, std::size_t n) {
      out.resize(n);
      { out.data() } -> std::same_as<typename Out::value_type*>;
    };

  /// The strongest tag an iterator models, for an adaptor's "iterator_concept".
  template <typename I>
  using iter_strength_t =
    std::conditional_t<std::random_access_iterator<I>, std::random_access_iterator_tag,
    std::conditional_t<std::bidirectional_iterator<I>, std::bidirectional_iterator_tag,
    std::conditional_t<std::forward_iterator<I>, std::forward_iterator_tag, std::input_iterator_tag>>>;

  /// An adaptor's "iterator_category", which legacy algorithms dispatch on.
  /// Legacy forward iterators and above must return a true reference, so an
  /// iterator returning a prvalue can only claim to be an input iterator there,
  /// whatever "iterator_concept" says it models.
  template <typename Reference, typename Concept>
  using legacy_iter_category_t =
    std::conditional_t<std::is_lvalue_reference_v<Reference>, Concept, std::input_iterator_tag>;

  template <typename I>
  using iter_category_t = std::iterator_traits<I>::iterator_category;
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
  template <typename T>
  struct is_unique_ptr : std::false_type {
  };

  template <typename T, typename Deleter>
  struct is_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {
  };

  template <typename T>
  concept unique_ptr_like = is_unique_ptr<std::remove_cvref_t<T>>::value;

  template <typename T>
  struct is_shared_ptr : std::false_type {
  };

  template <typename T>
  struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {
  };

  template <typename T>
  concept shared_ptr_like = is_shared_ptr<std::remove_cvref_t<T>>::value;

  template <typename T>
  struct is_tuple_like : std::false_type {
  };

  template <typename... Ts>
  struct is_tuple_like<std::tuple<Ts...>> : std::true_type {
  };

  template <typename T>
  concept tuple_like = is_tuple_like<std::remove_cvref_t<T>>::value;

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

export namespace genex {
  /// What a callable taking a copy of an iterator's element is given, as opposed
  /// to what dereferencing the iterator gives it. A callable has to accept both.
  template <typename I>
  struct indirect_value {
    using type = iter_value_t<I>&;
  };

  template <typename I>
  requires requires { typename I::indirect_value_type; }
  struct indirect_value<I> {
    using type = I::indirect_value_type;
  };

  template <typename I>
  using indirect_value_t = indirect_value<I>::type;

  template <typename F, typename I>
  concept indirectly_unary_invocable =
    std::indirectly_readable<I> and
    std::copy_constructible<F> and
    std::invocable<F&, indirect_value_t<I>> and
    std::invocable<F&, iter_reference_t<I>> and
    std::common_reference_with<
      std::invoke_result_t<F&, indirect_value_t<I>>,
      std::invoke_result_t<F&, iter_reference_t<I>>>;

  template <typename F, typename I>
  concept indirectly_regular_unary_invocable =
    indirectly_unary_invocable<F, I> and
    std::regular_invocable<F&, indirect_value_t<I>> and
    std::regular_invocable<F&, iter_reference_t<I>>;

  /// Nested so that it doesn't have "I" and "Proj" as template arguments. If it
  /// did, argument dependent lookup through it would look inside them, which
  /// needs them complete and can find their friends.
  template <typename I, typename Proj>
  struct projected_impl {
    struct type {
      using value_type = std::remove_cvref_t<std::invoke_result_t<Proj&, iter_reference_t<I>>>;
      using indirect_value_type = std::invoke_result_t<Proj&, indirect_value_t<I>>;
      auto operator*() const -> std::invoke_result_t<Proj&, iter_reference_t<I>>;
    };
  };

  /// Stands in for "I" with "Proj" applied to what it reads, for a concept to
  /// check a callable against. It's never actually dereferenced.
  template <typename I, typename Proj>
  requires std::indirectly_readable<I> and indirectly_regular_unary_invocable<Proj, I>
  using projected = projected_impl<I, Proj>::type;

  template <typename F, typename I>
  concept indirect_unary_predicate =
    std::indirectly_readable<I> and
    std::copy_constructible<F> and
    std::predicate<F&, indirect_value_t<I>> and
    std::predicate<F&, iter_reference_t<I>>;

  template <typename F, typename I1, typename I2>
  concept indirect_binary_predicate =
    std::indirectly_readable<I1> and
    std::indirectly_readable<I2> and
    std::copy_constructible<F> and
    std::predicate<F&, indirect_value_t<I1>, indirect_value_t<I2>> and
    std::predicate<F&, indirect_value_t<I1>, iter_reference_t<I2>> and
    std::predicate<F&, iter_reference_t<I1>, indirect_value_t<I2>> and
    std::predicate<F&, iter_reference_t<I1>, iter_reference_t<I2>>;

  template <typename F, typename I1, typename I2 = I1>
  concept indirect_equivalence_relation =
    std::indirectly_readable<I1> and
    std::indirectly_readable<I2> and
    std::copy_constructible<F> and
    std::equivalence_relation<F&, indirect_value_t<I1>, indirect_value_t<I2>> and
    std::equivalence_relation<F&, indirect_value_t<I1>, iter_reference_t<I2>> and
    std::equivalence_relation<F&, iter_reference_t<I1>, indirect_value_t<I2>> and
    std::equivalence_relation<F&, iter_reference_t<I1>, iter_reference_t<I2>>;

  template <typename F, typename I1, typename I2 = I1>
  concept indirect_strict_weak_order =
    std::indirectly_readable<I1> and
    std::indirectly_readable<I2> and
    std::copy_constructible<F> and
    std::strict_weak_order<F&, indirect_value_t<I1>, indirect_value_t<I2>> and
    std::strict_weak_order<F&, indirect_value_t<I1>, iter_reference_t<I2>> and
    std::strict_weak_order<F&, iter_reference_t<I1>, indirect_value_t<I2>> and
    std::strict_weak_order<F&, iter_reference_t<I1>, iter_reference_t<I2>>;

  template <typename I1, typename I2, typename Comp, typename Proj1, typename Proj2>
  concept indirectly_comparable =
    indirect_binary_predicate<Comp, projected<I1, Proj1>, projected<I2, Proj2>>;

  /// Elements can be moved and swapped between positions, without changing what
  /// they are. A @c std::set 's can't, as they can't be assigned to.
  template <typename I>
  concept permutable =
    std::forward_iterator<I> and
    std::indirectly_movable_storable<I, I> and
    std::indirectly_swappable<I, I>;

  template <typename I, typename Comp, typename Proj>
  concept sortable =
    permutable<I> and
    indirect_strict_weak_order<Comp, projected<I, Proj>>;
}
