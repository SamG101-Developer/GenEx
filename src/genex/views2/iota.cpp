module;
#include <genex/macros.hpp>

export module genex.views2.iota;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::impl {
    template <typename Int>
    requires std::weakly_incrementable<Int>
    struct iota_iterator {
        Int lo;
        Int hi;
        Int step;
        Int it;

        using value_type = Int;
        using reference_type = Int;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(iota_iterator)

        GENEX_INLINE constexpr iota_iterator() = default;

        GENEX_INLINE constexpr iota_iterator(const Int lo, const Int hi, const Int step)
            : lo(lo), hi(hi), step(step), it(lo) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            self.it += self.step;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            self.it -= self.step;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return self.it;
        }

        GENEX_VIEW_ITER_EQ(iota_iterator, iota_iterator) {
            return self.it >= self.hi;
        }
    };

    template <typename Int>
    requires std::weakly_incrementable<Int>
    struct iota_view {
        Int lo;
        Int hi;
        Int step;

        GENEX_INLINE constexpr iota_view(const Int lo, const Int hi, const Int step)
            : lo(lo), hi(hi), step(step) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return iota_iterator<Int>(self.lo, self.hi, self.step);
        }

        template <typename Self>
        GENEX_ITER_END {
            return iota_iterator<Int>(self.hi, self.hi, self.step);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return (self.hi - self.lo + self.step - 1) / self.step;
        }
    };
}


namespace genex::views {
    struct iota_fn {
        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int lo, const Int hi, const Int step = 1) const {
            return detail::impl::iota_view(lo, hi, step);
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int hi) const {
            return detail::impl::iota_view(static_cast<Int>(0), hi, static_cast<Int>(1));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int> and std::numeric_limits<Int>::is_specialized
        GENEX_INLINE constexpr auto operator()() const {
            return detail::impl::iota_view(static_cast<Int>(0), std::numeric_limits<Int>::max(), static_cast<Int>(1));
        }
    };

    export inline constexpr iota_fn iota{};
}
