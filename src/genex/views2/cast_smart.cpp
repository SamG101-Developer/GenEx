module;
#include <genex/macros.hpp>

export module genex.views2.cast_smart;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename Ptr, typename New>
    struct rebind_smart_ptr {
        using type = Ptr;
    };

    template <typename T, typename New>
    struct rebind_smart_ptr<std::shared_ptr<T>, New> {
        using type = std::shared_ptr<New>;
    };

    template <typename T, typename New>
    struct rebind_smart_ptr<std::unique_ptr<T>, New> {
        using type = std::unique_ptr<New>;
    };

    template <typename Ptr, typename New>
    using rebind_smart_ptr_t = typename rebind_smart_ptr<Ptr, New>::type;
}


namespace genex::views2::detail::concepts {
    template <typename To, typename I, typename S>
    concept smart_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        not std::is_pointer_v<To> and
        not std::is_lvalue_reference_v<To>;

    template <typename To, typename Rng>
    concept smart_castable_range =
        input_range<Rng> and
        smart_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views2::detail::impl {
    struct cast_smart_sentinel {};

    template <typename To, typename I, typename S>
    requires concepts::smart_castable_iters<To, I, S>
    struct cast_smart_iterator {
        I it; S st;
        mutable concepts::rebind_smart_ptr_t<iter_value_t<I>, To> cached;

        using value_type = concepts::rebind_smart_ptr_t<iter_value_t<I>, To>;
        using reference_type = concepts::rebind_smart_ptr_t<iter_value_t<I>, To>&;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(cast_smart_iterator)

        GENEX_INLINE constexpr cast_smart_iterator() = default;

        GENEX_INLINE constexpr cast_smart_iterator(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
            fwd_to_valid();
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

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return std::move(self.cached);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(cast_smart_sentinel) {
            return self.it == self.st;
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

        template <typename Self> requires shared_ptr<iter_value_t<I>>
        GENEX_INLINE constexpr auto update_cache(this Self &&self) -> void {
            if (self.it != self.st) { self.cached = std::dynamic_pointer_cast<To>(*self.it); }
            else { self.cached = nullptr; }
        }

        template <typename Self> requires unique_ptr<iter_value_t<I>>
        GENEX_INLINE constexpr auto update_cache(this Self &&self) -> void {
            if (self.it != self.st) {
                auto raw_ptr = dynamic_cast<To*>(self.it->release());
                self.cached = std::unique_ptr<To>(raw_ptr);
            }
            else { self.cached = nullptr; }
        }
    };

    template <typename To, typename I, typename S>
    requires concepts::smart_castable_iters<To, I, S>
    struct cast_smart_view {
        I it; S st;

        GENEX_INLINE constexpr cast_smart_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return cast_smart_iterator<To, I, S>(self.it, self.st);
        }

        template <typename Self>
        GENEX_ITER_END {
            return cast_smart_sentinel();
        }
    };
}


namespace genex::views2 {
    template <typename To>
    struct cast_smart_fn {
        template <typename I, typename S>
        requires detail::concepts::smart_castable_iters<To, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::cast_smart_view<To, I, S>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::smart_castable_range<To, Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::cast_smart_view<To, iterator_t<Rng>, sentinel_t<Rng>>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cast_smart_fn{});
        }
    };

    export template <typename To>
    inline constexpr cast_smart_fn<To> cast_smart{};
}

