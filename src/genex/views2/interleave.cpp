module;
#include <genex/macros.hpp>

export module genex.views2.interleave;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename T1, typename T2>
    using interleave_common_t = std::common_type_t<
        std::remove_cvref_t<iter_value_t<T1>>,
        std::remove_cvref_t<iter_value_t<T2>>>;

    template <typename I1, typename S1, typename I2, typename S2>
    concept interleavable_iters =
        std::input_iterator<I1> and
        std::input_iterator<I2> and
        std::sentinel_for<S1, I1> and
        std::sentinel_for<S2, I2> and
        std::convertible_to<iter_value_t<I1>, interleave_common_t<I1, I2>> and
        std::convertible_to<iter_value_t<I2>, interleave_common_t<I1, I2>>;

    template <typename Rng1, typename Rng2>
    concept interleavable_range =
        input_range<Rng1> and
        input_range<Rng2> and
        interleavable_iters<iterator_t<Rng1>, sentinel_t<Rng1>, iterator_t<Rng2>, sentinel_t<Rng2>>;
}


namespace genex::views2::detail::impl {
    template <typename I1, typename S1, typename I2, typename S2>
    requires concepts::interleavable_iters<I1, S1, I2, S2>
    struct interleave_iterator {
        I1 it1;
        S1 st1;
        I2 it2;
        S2 st2;
        bool use_first = true;

        using value_type = concepts::interleave_common_t<I1, I2>;
        using reference_type = value_type;
        using difference_type = std::common_type_t<iter_difference_t<I1>, iter_difference_t<I2>>;
        using iterator_category = std::common_type_t<
            typename std::iterator_traits<I1>::iterator_category,
            typename std::iterator_traits<I2>::iterator_category>;
        using iterator_concept = iterator_category;

        GENEX_INLINE friend constexpr auto operator==(
            interleave_iterator const &self,
            interleave_iterator const &that)
            -> bool {
            return self.it1 == that.it1 and self.it2 == that.it2;
        }

        GENEX_INLINE friend constexpr auto operator!=(
            interleave_iterator const &self,
            interleave_iterator const &that)
            -> bool {
            return !(self == that);
        }

        GENEX_INLINE friend constexpr auto operator*(
            interleave_iterator const &self)
            -> decltype(auto) {
            return self.deref();
        }

        GENEX_INLINE friend constexpr auto operator++(
            interleave_iterator &self)
            -> interleave_iterator& {
            self.next();
            return self;
        }

        GENEX_INLINE friend constexpr auto operator++(
            interleave_iterator &self, int)
            -> interleave_iterator {
            auto temp = self;
            ++self;
            return temp;
        }

        GENEX_INLINE friend constexpr auto operator--(
            interleave_iterator &self)
            -> interleave_iterator& requires std::bidirectional_iterator<decltype(std::declval<interleave_iterator>().it)> {
            self.prev();
            return self;
        }

        GENEX_INLINE friend constexpr auto operator--(
            interleave_iterator &self, int)
            -> interleave_iterator requires std::bidirectional_iterator<decltype(std::declval<interleave_iterator>().it)> {
            auto temp = self;
            --self;
            return temp;
        }

        GENEX_INLINE friend constexpr auto operator+(
            interleave_iterator const &self,
            difference_type m)
            -> interleave_iterator requires std::random_access_iterator<decltype(std::declval<interleave_iterator>().it)> {
            auto temp = *self;
            temp += m;
            return temp;
        }

        GENEX_INLINE friend constexpr auto operator-(
            interleave_iterator const &self,
            difference_type m)
            -> interleave_iterator requires std::random_access_iterator<decltype(std::declval<interleave_iterator>().it)> {
            auto temp = *self;
            temp -= m;
            return temp;
        }

        GENEX_INLINE constexpr interleave_iterator() = default;

        GENEX_INLINE constexpr interleave_iterator(I1 first1, S1 last1, I2 first2, S2 last2) :
            it1(std::move(first1)), st1(std::move(last1)),
            it2(std::move(first2)), st2(std::move(last2)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            if (self.use_first) {
                if (self.it1 != self.st1) { ++self.it1; }
            }
            else {
                if (self.it2 != self.st2) { ++self.it2; }
            }
            self.use_first = not self.use_first;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            if (self.use_first) {
                if (self.it2 != self.st2) { --self.it2; }
            }
            else {
                if (self.it1 != self.st1) { --self.it1; }
            }
            self.use_first = not self.use_first;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            if (self.use_first) {
                return static_cast<value_type>(*self.it1);
            }
            return static_cast<value_type>(*self.it2);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(interleave_iterator) {
            return self.it1 == that.it1 and self.it2 == that.it2;
        }
    };

    template <typename I1, typename S1, typename I2, typename S2>
    requires concepts::interleavable_iters<I1, S1, I2, S2>
    struct interleave_view {
        I1 it1; S1 st1;
        I2 it2; S2 st2;

        GENEX_INLINE constexpr interleave_view(I1 first1, S1 last1, I2 first2, S2 last2) :
            it1(std::move(first1)), st1(std::move(last1)),
            it2(std::move(first2)), st2(std::move(last2)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return interleave_iterator(self.it1, self.st1, self.it2, self.st2);
        }

        template <typename Self>
        GENEX_ITER_END {
            return interleave_iterator(self.st1, self.st1, self.st2, self.st2);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it1, self.st1) + iterators::distance(self.it2, self.st2);
        }
    };
}


namespace genex::views2 {
    struct interleave_fn {
        template <typename I1, typename S1, typename I2, typename S2>
        requires detail::concepts::interleavable_iters<I1, S1, I2, S2>
        GENEX_INLINE constexpr auto operator()(I1 first1, S1 last1, I2 first2, S2 last2) const {
            return detail::impl::interleave_view(std::move(first1), std::move(last1), std::move(first2), std::move(last2));
        }

        template <typename Rng1, typename Rng2>
        requires detail::concepts::interleavable_range<Rng1, Rng2>
        GENEX_INLINE constexpr auto operator()(Rng1 &&rng1, Rng2 &&rng2) const {
            auto [first1, last1] = iterators::iter_pair(rng1);
            auto [first2, last2] = iterators::iter_pair(rng2);
            return detail::impl::interleave_view(std::move(first1), std::move(last1), std::move(first2), std::move(last2));
        }

        template <typename Rng2>
        GENEX_INLINE constexpr auto operator()(Rng2 &&rng2) const {
            return meta::bind_back(interleave_fn{}, std::forward<Rng2>(rng2));
        }
    };

    export inline constexpr interleave_fn interleave{};
}
