module;
#include <genex/macros.hpp>

export module genex.views2.set_algorithms;
export import genex.pipe;
import genex.algorithms.contains;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views2::detail::concepts {
    template <typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    concept set_algorithmicable_sorted_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirectly_comparable<I1, I2, Comp, Proj1, Proj2>;

    template <typename Rng1, typename Rng2, typename Comp, typename Proj1, typename Proj2>
    concept set_algorithmicable_sorted_range =
        input_range<Rng1> and
        input_range<Rng2> and
        set_algorithmicable_sorted_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Comp, Proj1, Proj2>;

    template <typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    concept set_algorithmicable_unsorted_iters =
        std::forward_iterator<I1> and
        std::forward_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::indirectly_comparable<I1, I2, operations::eq, Proj1, Proj2>;

    template <typename Rng1, typename Rng2, typename Proj1, typename Proj2>
    concept set_algorithmicable_unsorted_range =
        forward_range<Rng1> and
        forward_range<Rng2> and
        set_algorithmicable_unsorted_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Proj1, Proj2>;
}


namespace genex::views2::detail::impl {
    enum class set_op { difference, intersection, symmetric_difference, union_ };
    struct set_algorithm_sentinel {};

    template <set_op Op, typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_sorted_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    struct set_sorted_iterator {
        I1 it1; S1 st1;
        I2 it2; S2 st2;
        GENEX_NO_UNIQUE_ADDRESS Comp comp;
        GENEX_NO_UNIQUE_ADDRESS Proj1 proj1;
        GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;

        using value_type = std::common_type_t<iter_value_t<I1>, iter_value_t<I2>>;
        using reference_type = std::common_reference_t<iter_reference_t<I1>, iter_reference_t<I2>>;
        using difference_type = std::common_type_t<iter_difference_t<I1>, iter_difference_t<I2>>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;

        std::optional<value_type> cur_elem = std::nullopt;
        GENEX_ITER_OPS_MINIMAL(set_sorted_iterator)

        GENEX_INLINE constexpr set_sorted_iterator() = default;

        GENEX_INLINE constexpr set_sorted_iterator(I1 first1, S1 last1, I2 first2, S2 last2, Comp comp, Proj1 proj1, Proj2 proj2) :
            it1(std::move(first1)), st1(std::move(last1)),
            it2(std::move(first2)), st2(std::move(last2)),
            comp(std::move(comp)), proj1(std::move(proj1)), proj2(std::move(proj2)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV = delete;

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.cur_elem;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(set_algorithm_sentinel) {
            return not self.cur_elem.has_value();
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self &&self) -> void {
            self.cur_elem.reset();

            auto less12 = [&](auto const &a, auto const &b) {
                return meta::invoke(self.comp, meta::invoke(self.proj1, a), meta::invoke(self.proj2, b));
            };
            auto less21 = [&](auto const &a, auto const &b) {
                return meta::invoke(self.comp, meta::invoke(self.proj2, b), meta::invoke(self.proj1, a));
            };

            if constexpr (Op == set_op::difference) {
                while (self.it1 != self.st1) {
                    if (self.it2 == self.st2 or less12(*self.it1, *self.it2)) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    if (less12(*self.it1, *self.it2)) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    if (less21(*self.it1, *self.it2)) {
                        ++self.it2;
                        continue;
                    }
                    ++self.it1; ++self.it2;
                }
                return;
            }

            if constexpr (Op == set_op::intersection) {
                while (self.it1 != self.st1 and self.it2 != self.st2) {
                    if (less12(*self.it1, *self.it2)) { ++self.it1; continue; }
                    if (less21(*self.it1, *self.it2)) { ++self.it2; continue; }
                    self.cur_elem = *self.it1;
                    ++self.it1; ++self.it2;
                    return;
                }
                return;
            }

            if constexpr (Op == set_op::symmetric_difference) {
                while (self.it1 != self.st1 or self.it2 != self.st2) {
                    if (self.it1 == self.st1) {
                        self.cur_elem = *self.it2;
                        ++self.it2;
                        return;
                    }
                    if (self.it2 == self.st2) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    if (less12(*self.it1, *self.it2)) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    if (less21(*self.it1, *self.it2)) {
                        self.cur_elem = *self.it2;
                        ++self.it2;
                        return;
                    }
                    ++self.it1; ++self.it2;
                }
                return;
            }

            if constexpr (Op == set_op::union_) {
                while (self.it1 != self.st1 or self.it2 != self.st2) {
                    if (self.it1 == self.st1) {
                        self.cur_elem = *self.it2;
                        ++self.it2;
                        return;
                    }
                    if (self.it2 == self.st2) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    if (less12(*self.it1, *self.it2)) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    if (less21(*self.it1, *self.it2)) {
                        self.cur_elem = *self.it2;
                        ++self.it2;
                        return;
                    }
                    self.cur_elem = *self.it1;
                    ++self.it1; ++self.it2;
                    return;
                }
                return;
            }
        }
    };

    template <set_op Op, typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_unsorted_iters<I1, S1, I2, S2, Proj1, Proj2>
    struct set_unsorted_iterator {
        I1 it1; S1 st1;
        I2 it2; S2 st2;
        I1 f1; I2 f2;
        GENEX_NO_UNIQUE_ADDRESS Proj1 proj1;
        GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;
        bool second_pass = false;;

        using value_type = std::common_type_t<iter_value_t<I1>, iter_value_t<I2>>;
        using reference_type = std::common_reference_t<iter_reference_t<I1>, iter_reference_t<I2>>;
        using difference_type = std::common_type_t<iter_difference_t<I1>, iter_difference_t<I2>>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;

        std::optional<value_type> cur_elem = std::nullopt;
        GENEX_ITER_OPS_MINIMAL(set_unsorted_iterator)

        GENEX_INLINE constexpr set_unsorted_iterator() = default;

        GENEX_INLINE constexpr set_unsorted_iterator(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1, Proj2 proj2) :
            it1(std::move(first1)), st1(std::move(last1)),
            it2(std::move(first2)), st2(std::move(last2)),
            f1(it1), f2(it2),
            proj1(std::move(proj1)), proj2(std::move(proj2)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV = delete;

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.cur_elem;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(set_algorithm_sentinel) {
            return not self.cur_elem.has_value();
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self &&self) -> void {
            self.cur_elem.reset();

            auto contains2 = [&](auto const &v1) {
                return genex::contains(self.f2, self.st2, meta::invoke(self.proj1, v1), self.proj2);
            };

            auto contains1 = [&](auto const &v2) {
                return genex::contains(self.f1, self.st1, meta::invoke(self.proj2, v2), self.proj1);
            };

            if constexpr (Op == set_op::difference) {
                while (self.it1 != self.st1) {
                    if (not contains2(*self.it1)) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    ++self.it1;
                }
                return;
            }

            if constexpr (Op == set_op::intersection) {
                while (self.it1 != self.st1) {
                    if (contains2(*self.it1)) {
                        self.cur_elem = *self.it1;
                        ++self.it1;
                        return;
                    }
                    ++self.it1;
                }
                return;
            }

            if constexpr (Op == set_op::symmetric_difference) {
                if (not self.second_pass) {
                    while (self.it1 != self.st1) {
                        if (not contains2(*self.it1)) {
                            self.cur_elem = *self.it1;
                            ++self.it1;
                            return;
                        }
                        ++self.it1;
                    }
                    self.second_pass = true;
                    self.it2 = std::move(self.f2);
                }
                while (self.it2 != self.st2) {
                    if (not contains1(*self.it2)) {
                        self.cur_elem = *self.it2;
                        ++self.it2;
                        return;
                    }
                    ++self.it2;
                }
                return;
            }

            if constexpr (Op == set_op::union_) {
                if (not self.second_pass) {
                    while (self.it1 != self.st1) {
                        if (not genex::contains(self.f1, self.it1, meta::invoke(self.proj1, *self.it1), self.proj1)) {
                            self.cur_elem = *self.it1;
                            ++self.it1;
                            return;
                        }
                        ++self.it1;
                    }
                    self.second_pass = true;
                    self.it2 = std::move(self.f2);
                }

                while (self.it2 != self.st2) {
                    const bool already_in_1 = contains1(*self.it2);
                    const bool already_emitted_in_2 =
                        genex::contains(self.f2, self.it2, meta::invoke(self.proj2, *self.it2), self.proj2);

                    if (not already_in_1 and not already_emitted_in_2) {
                        self.cur_elem = *self.it2;
                        ++self.it2;
                        return;
                    }
                    ++self.it2;
                }
                return;
            }
        }
    };

    template <set_op Op, typename I1, typename S1, typename I2, typename S2, typename Comp, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_sorted_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
    struct set_algorithm_sorted_view {
        I1 first1; S1 last1;
        I2 first2; S2 last2;
        GENEX_NO_UNIQUE_ADDRESS Comp comp;
        GENEX_NO_UNIQUE_ADDRESS Proj1 proj1;
        GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;

        GENEX_INLINE constexpr set_algorithm_sorted_view(I1 f1, S1 l1, I2 f2, S2 l2, Comp c, Proj1 p1, Proj2 p2) :
            first1(std::move(f1)), last1(std::move(l1)),
            first2(std::move(f2)), last2(std::move(l2)),
            comp(std::move(c)), proj1(std::move(p1)), proj2(std::move(p2)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return set_sorted_iterator<Op, I1, S1, I2, S2, Comp, Proj1, Proj2>(self.first1, self.last1, self.first2, self.last2, self.comp, self.proj1, self.proj2);
        }

        template <typename Self>
        GENEX_ITER_END {
            return set_algorithm_sentinel();
        }
    };

    template <set_op Op, typename I1, typename S1, typename I2, typename S2, typename Proj1, typename Proj2>
    requires concepts::set_algorithmicable_unsorted_iters<I1, S1, I2, S2, Proj1, Proj2>
    struct set_algorithm_unsorted_view {
        I1 first1; S1 last1;
        I2 first2; S2 last2;
        GENEX_NO_UNIQUE_ADDRESS Proj1 proj1;
        GENEX_NO_UNIQUE_ADDRESS Proj2 proj2;

        GENEX_INLINE constexpr set_algorithm_unsorted_view(I1 f1, S1 l1, I2 f2, S2 l2, Proj1 p1, Proj2 p2) :
            first1(std::move(f1)), last1(std::move(l1)),
            first2(std::move(f2)), last2(std::move(l2)),
            proj1(std::move(p1)), proj2(std::move(p2)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return set_unsorted_iterator<Op, I1, S1, I2, S2, Proj1, Proj2>(self.first1, self.last1, self.first2, self.last2, self.proj1, self.proj2);
        }

        template <typename Self>
        GENEX_ITER_END {
            return set_algorithm_sentinel();
        }
    };
}


namespace genex::views2 {
    template <detail::impl::set_op Op>
    struct set_algorithms_sorted_base_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::set_algorithmicable_sorted_iters<I1, S1, I2, S2, Comp, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return detail::impl::set_algorithm_sorted_view<Op, I1, S1, I2, S2, Comp, Proj1, Proj2>(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(comp), std::move(proj1), std::move(proj2));
        }

        template <typename Rng1, typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::set_algorithmicable_sorted_range<Rng1, Rng2, Comp, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return detail::impl::set_algorithm_sorted_view<Op, iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Comp, Proj1, Proj2>(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(comp), std::move(proj1), std::move(proj2));
        }

        template <typename Rng2, typename Comp = operations::eq, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires (range<Rng2> and not range<Comp>)
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return meta::bind_back(set_algorithms_sorted_base_fn{}, std::forward<Rng2>(rng2), std::move(comp), std::move(proj1), std::move(proj2));
        }
    };

    template <detail::impl::set_op Op>
    struct set_algorithms_unsorted_base_fn {
        template <typename I1, typename S1, typename I2, typename S2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::set_algorithmicable_unsorted_iters<I1, S1, I2, S2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return detail::impl::set_algorithm_unsorted_view<Op, I1, S1, I2, S2, Proj1, Proj2>(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(proj1), std::move(proj2));
        }

        template <typename Rng1, typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires detail::concepts::set_algorithmicable_unsorted_range<Rng1, Rng2, Proj1, Proj2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return detail::impl::set_algorithm_unsorted_view<Op, iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>, Proj1, Proj2>(std::move(first1), std::move(last1), std::move(first2), std::move(last2), std::move(proj1), std::move(proj2));
        }

        template <typename Rng2, typename Proj1 = meta::identity, typename Proj2 = meta::identity>
        requires (range<Rng2> and not range<Proj1>)
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
            return meta::bind_back(set_algorithms_unsorted_base_fn{}, std::forward<Rng2>(rng2), std::move(proj1), std::move(proj2));
        }
    };

    using set_difference_sorted_fn = set_algorithms_sorted_base_fn<detail::impl::set_op::difference>;
    using set_intersection_sorted_fn = set_algorithms_sorted_base_fn<detail::impl::set_op::intersection>;
    using set_symmetric_difference_sorted_fn = set_algorithms_sorted_base_fn<detail::impl::set_op::symmetric_difference>;
    using set_union_sorted_fn = set_algorithms_sorted_base_fn<detail::impl::set_op::union_>;

    using set_difference_unsorted_fn = set_algorithms_unsorted_base_fn<detail::impl::set_op::difference>;
    using set_intersection_unsorted_fn = set_algorithms_unsorted_base_fn<detail::impl::set_op::intersection>;
    using set_symmetric_difference_unsorted_fn = set_algorithms_unsorted_base_fn<detail::impl::set_op::symmetric_difference>;
    using set_union_unsorted_fn = set_algorithms_unsorted_base_fn<detail::impl::set_op::union_>;

    export inline constexpr set_difference_sorted_fn set_difference_sorted{};
    export inline constexpr set_intersection_sorted_fn set_intersection_sorted{};
    export inline constexpr set_symmetric_difference_sorted_fn set_symmetric_difference_sorted{};
    export inline constexpr set_union_sorted_fn set_union_sorted{};

    export inline constexpr set_difference_unsorted_fn set_difference_unsorted{};
    export inline constexpr set_intersection_unsorted_fn set_intersection_unsorted{};
    export inline constexpr set_symmetric_difference_unsorted_fn set_symmetric_difference_unsorted{};
    export inline constexpr set_union_unsorted_fn set_union_unsorted{};
}
