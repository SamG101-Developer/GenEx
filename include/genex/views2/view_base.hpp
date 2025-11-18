#pragma once
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/distance.hpp>


namespace genex::views2::detail::impl {
    struct view_base {
        template <typename Self>
        GENEX_ITER_CUSTOM_SIZE
            -> std::iterator_traits<iterator_t<Self>>::difference_type {
            return iterators::distance(self.it, self.st);
        }

        template <typename Self>
        GENEX_ITER_CUSTOM_EMPTY {
            return self.it == self.st;
        }
    };

    struct view_iterator_base {
        // OVERRIDE THIS
        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            return self;
        }

        // OVERRIDE THIS
        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            return self;
        }

        // OVERRIDE THIS
        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.it;
        }

        template <typename Self, typename That>
        requires requires(Self &&self, That &&that) { self.it == that.it; }
        GENEX_INLINE constexpr auto operator==(this Self &&self, That &&that) -> bool {
            return self.it == that.it;
        }

        template <typename Self, typename That>
        requires requires(Self &&self, That &&that) { self.it != that.it; }
        GENEX_INLINE constexpr auto operator!=(this Self &&self, That &&that) -> bool {
            return not(self == that);
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator*(this Self &&self) -> decltype(auto) {
            return self.deref();
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator++(this Self &&self) -> Self& {
            self.next();
            return self;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator++(this Self &&self, int) -> Self {
            auto temp = *self;
            ++*self;
            return temp;
        }

        template <typename Self>
        requires std::bidirectional_iterator<Self>
        GENEX_INLINE constexpr auto operator--(this Self &&self) -> Self& {
            self.prev();
            return self;
        }

        template <typename Self>
        requires std::bidirectional_iterator<Self>
        GENEX_INLINE constexpr auto operator--(this Self &&self, int) -> Self {
            auto temp = *self;
            --*self;
            return temp;
        }

        template <typename Self>
        requires std::random_access_iterator<Self>
        GENEX_INLINE constexpr auto operator+=(this Self &&self, Self::difference_type n) -> Self& {
            self.it += n;
            return *self;
        }

        template <typename Self>
        requires std::random_access_iterator<Self>
        GENEX_INLINE constexpr auto operator+(this Self &&self, Self::difference_type n) -> Self {
            auto temp = *self;
            temp += n;
            return temp;
        }

        template <typename Self>
        requires std::random_access_iterator<Self>
        GENEX_INLINE constexpr auto operator-=(this Self &&self, Self::difference_type n) -> Self& {
            self.it -= n;
            return *self;
        }

        template <typename Self>
        requires std::random_access_iterator<Self>
        GENEX_INLINE constexpr auto operator-(this Self &&self, Self::difference_type n) -> Self {
            auto temp = *self;
            temp -= n;
            return temp;
        }
    };
}
