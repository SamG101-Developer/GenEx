#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename To, typename I, typename S>
    concept static_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        requires(iter_value_t<I> &&from) { static_cast<To>(from); };

    template <typename To, typename I, typename S>
    concept dynamic_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::is_pointer_v<To> and
        requires(iter_value_t<I> &&from) { dynamic_cast<To>(from); };

    template <typename To, typename I, typename S>
    concept smart_ptr_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename To, typename Rng>
    concept static_castable_range =
        input_range<Rng> and
        static_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename To, typename Rng>
    concept dynamic_castable_range =
        input_range<Rng> and
        dynamic_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename To, typename Rng>
    concept smart_ptr_castable_range =
        input_range<Rng> and
        smart_ptr_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename To, typename I, typename S>
        requires concepts::static_castable_iters<To, I, S>
    auto do_static_cast_iter(I first, S last) -> generator<To> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield static_cast<To>(*first);
        }
    }

    template <typename To, typename I, typename S>
        requires concepts::dynamic_castable_iters<To, I, S>
    auto do_dynamic_cast_iter(I first, S last) -> generator<To> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = dynamic_cast<To>(*first)) {
                co_yield ptr;
            }
        }
    }

    template <typename To, typename I, typename S>
        requires (concepts::smart_ptr_castable_iters<To, I, S> and unique_ptr<iter_value_t<I>>)
    auto do_unique_ptr_cast_iter(I first, S last) -> generator<std::unique_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = dynamic_cast<To*>((*first).get())) {
                (*first).release();
                co_yield std::unique_ptr<To>(ptr);
            }
        }
    }

    template <typename To, typename I, typename S>
        requires (concepts::smart_ptr_castable_iters<To, I, S> and shared_ptr<iter_value_t<I>>)
    auto do_shared_ptr_cast_iter(I first, S last) -> generator<std::shared_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = std::dynamic_pointer_cast<To>(*first)) {
                co_yield ptr;
            }
        }
    }

    template <typename To, typename I, typename S>
        requires (concepts::smart_ptr_castable_iters<To, I, S> and weak_ptr<iter_value_t<I>>)
    auto do_weak_ptr_cast_iter(I first, S last) -> generator<std::weak_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = std::dynamic_pointer_cast<To>(first->lock())) {
                co_yield std::weak_ptr<To>(ptr);
            }
        }
    }
}


namespace genex::views {
    struct cast_static_impl_fn {
        template <typename To, typename I, typename S>
            requires concepts::static_castable_iters<To, I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_static_cast_iter<To, I, S>(std::move(first), std::move(last));
        }

        template <typename To, typename Rng>
            requires concepts::static_castable_range<To, Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return this->operator()<To>(std::move(first), std::move(last));
        }

        template <typename To>
        constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>(Rng &&rng) requires concepts::static_castable_range<To, Rng> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    struct cast_dynamic_impl_fn {
        template <typename To, typename I, typename S>
            requires concepts::dynamic_castable_iters<To, I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_dynamic_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename To, typename Rng>
            requires concepts::dynamic_castable_range<To, Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return this->operator()<To>(std::move(first), std::move(last));
        }

        template <typename To>
        constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>(Rng &&rng) requires concepts::dynamic_castable_range<To, Rng> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    struct cast_smart_ptr_impl_fn {
        template <typename To, typename I, typename S>
            requires concepts::smart_ptr_castable_iters<To, I, S> and unique_ptr<iter_value_t<I>> and std::movable<iter_value_t<I>>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_unique_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename To, typename I, typename S>
            requires concepts::smart_ptr_castable_iters<To, I, S> and shared_ptr<iter_value_t<I>>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_shared_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename To, typename I, typename S>
            requires concepts::smart_ptr_castable_iters<To, I, S> and weak_ptr<iter_value_t<I>>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_weak_ptr_cast_iter<To>(std::move(first), std::move(last));
        }

        template <typename To, typename Rng>
            requires concepts::smart_ptr_castable_range<To, Rng> and unique_ptr<range_value_t<Rng>> and std::movable<range_value_t<Rng>>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <typename To, typename Rng>
            requires concepts::smart_ptr_castable_range<To, Rng> and shared_ptr<range_value_t<Rng>>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <typename To, typename Rng>
            requires concepts::smart_ptr_castable_range<To, Rng> and weak_ptr<range_value_t<Rng>>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <typename To>
        constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>(Rng &&rng) requires concepts::smart_ptr_castable_range<To, Rng> {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_STRUCT(cast_static_impl);
    EXPORT_GENEX_STRUCT(cast_dynamic_impl);
    EXPORT_GENEX_STRUCT(cast_smart_ptr_impl);
}


#define cast_static cast_static_impl.operator()
#define cast_dynamic cast_dynamic_impl.operator()
#define cast_smart_ptr cast_smart_ptr_impl.operator()
