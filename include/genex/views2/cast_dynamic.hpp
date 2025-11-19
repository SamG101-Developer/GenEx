#pragma once
#include <genex/views2/view_base.hpp>


namespace genex::views2::detail::concepts {
    template <typename To, typename I, typename S>
    concept dynamic_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        (std::is_pointer_v<To> or std::is_lvalue_reference_v<To>);

    template <typename To, typename Rng>
    concept dynamic_castable_range =
        input_range<Rng> and
        dynamic_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views2::detail::impl {
    struct cast_dynamic_sentinel {};

    template <typename To, typename I, typename S>
    requires concepts::dynamic_castable_iters<To, I, S>
    struct cast_dynamic_iterator : view_iterator_base {
        I it;
        S st;
        mutable To cached = nullptr;

        using value_type = To;
        using reference_type = To;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_FORWARD_ITERATOR_OPERATIONS;

        GENEX_INLINE constexpr cast_dynamic_iterator() = default;

        GENEX_INLINE constexpr cast_dynamic_iterator(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return self.cached;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(cast_dynamic_sentinel) {
            return self.it == self.st;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            self.fwd_to_valid();
            return self;
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self &&self) -> void {
            while (self.it != self.st) {
                self.update_cache();
                if (self.cached != nullptr) { break; }
                ++self.it;
            }
        }

        template <typename Self>
        GENEX_INLINE constexpr auto bwd_to_valid(this Self &&self) -> void {
            while (self.it != self.st) {
                self.update_cache();
                if (self.cached != nullptr) { break; }
                --self.it;
            }
        }

        template <typename Self>
        GENEX_INLINE constexpr auto update_cache(this Self &&self) -> void {
            if (self.it != self.st) { self.cached = dynamic_cast<To>(*self.it); }
            else { self.cached = nullptr; }
        }
    };

    template <typename To, typename I, typename S>
    requires concepts::dynamic_castable_iters<To, I, S>
    struct cast_dynamic_view : view_base {
        I it; S st;

        GENEX_INLINE constexpr cast_dynamic_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return cast_dynamic_iterator<To, I, S>{self.it, self.st};
        }

        template <typename Self>
        GENEX_ITER_END {
            return cast_dynamic_sentinel{};
        }
    };
}


namespace genex::views2 {
    template <typename To>
    struct cast_dynamic_fn {
        template <typename I, typename S>
        requires detail::concepts::dynamic_castable_iters<To, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::cast_dynamic_view<To, I, S>(first, last);
        }

        template <typename Rng>
        requires detail::concepts::dynamic_castable_range<To, Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::cast_dynamic_view<To, iterator_t<Rng>, sentinel_t<Rng>>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cast_dynamic_fn{});
        }
    };

    template <typename To>
    inline constexpr cast_dynamic_fn<To> cast_dynamic{};
}

