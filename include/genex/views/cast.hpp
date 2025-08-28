#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename To, typename I, typename S>
    concept can_static_cast_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        requires(iter_value_t<I> &&from) { static_cast<To>(from); };

    template <typename To, typename I, typename S>
    concept can_dynamic_cast_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::is_pointer_v<To> and
        requires(iter_value_t<I> &&from) { dynamic_cast<To>(from); };

    template <typename To, typename I, typename S>
    concept can_smart_ptr_cast_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);


    template <typename To, typename Rng>
    concept can_static_cast_range =
        input_range<Rng> and
        can_static_cast_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename To, typename Rng>
    concept can_dynamic_cast_range =
        input_range<Rng> and
        can_dynamic_cast_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename To, typename Rng>
    concept can_smart_ptr_cast_range =
        input_range<Rng> and
        can_smart_ptr_cast_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename To, typename I, typename S> requires concepts::can_static_cast_iters<To, I, S>
    auto do_static_cast_iter(I first, S last) -> generator<To> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield static_cast<To>(*first);
        }
    }

    template <typename To, typename I, typename S> requires concepts::can_dynamic_cast_iters<To, I, S>
    auto do_dynamic_cast_iter(I first, S last) -> generator<To> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = dynamic_cast<To>(*first)) {
                co_yield ptr;
            }
        }
    }

    template <typename To, typename I, typename S> requires (concepts::can_smart_ptr_cast_iters<To, I, S> and unique_ptr<iter_value_t<I>>)
    auto do_unique_ptr_cast_iter(I first, S last) -> generator<std::unique_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = dynamic_cast<To*>(first->get())) {
                first->release();
                co_yield std::unique_ptr<To>(ptr);
            }
        }
    }

    template <typename To, typename I, typename S> requires (concepts::can_smart_ptr_cast_iters<To, I, S> and shared_ptr<iter_value_t<I>>)
    auto do_shared_ptr_cast_iter(I first, S last) -> generator<std::shared_ptr<To>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (auto ptr = std::dynamic_pointer_cast<To>(*first)) {
                co_yield ptr;
            }
        }
    }

    template <typename To, typename I, typename S> requires (concepts::can_smart_ptr_cast_iters<To, I, S> and weak_ptr<iter_value_t<I>>)
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
    DEFINE_VIEW(cast_static_) {
        template <typename To, typename I, typename S> requires concepts::can_static_cast_iters<To, I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_static_cast_iter<To, I, S>(std::move(first), std::move(last));
            return cast_static__view(std::move(gen));
        }

        template <typename To, typename Rng> requires concepts::can_static_cast_range<To, Rng>
        auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(rng), iterators::end(rng));
        }

        template <typename To>
        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_static_cast_range<To, Rng>
                (Rng &&rng) mutable -> auto {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(cast_dynamic_) {
        template <typename To, typename I, typename S> requires concepts::can_dynamic_cast_iters<To, I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_dynamic_cast_iter<To>(std::move(first), std::move(last));
            return cast_dynamic__view(std::move(gen));
        }

        template <typename To, typename Rng> requires concepts::can_dynamic_cast_range<To, Rng>
        auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(rng), iterators::end(rng));
        }

        template <typename To>
        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_dynamic_cast_range<To, Rng>
                (Rng &&rng) mutable -> auto {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(cast_smart_ptr_) {
        template <typename To, typename I, typename S> requires concepts::can_smart_ptr_cast_iters<To, I, S> and unique_ptr<iter_value_t<I>> and std::movable<iter_value_t<I>>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_unique_ptr_cast_iter<To>(std::move(first), std::move(last));
            return cast_smart_ptr__view(std::move(gen));
        }

        template <typename To, typename I, typename S> requires concepts::can_smart_ptr_cast_iters<To, I, S> and shared_ptr<iter_value_t<I>>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_shared_ptr_cast_iter<To>(std::move(first), std::move(last));
            return cast_smart_ptr__view(std::move(gen));
        }

        template <typename To, typename I, typename S> requires concepts::can_smart_ptr_cast_iters<To, I, S> and weak_ptr<iter_value_t<I>>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_weak_ptr_cast_iter<To>(std::move(first), std::move(last));
            return cast_smart_ptr__view(std::move(gen));
        }

        template <typename To, typename Rng> requires concepts::can_smart_ptr_cast_range<To, Rng> and unique_ptr<range_value_t<Rng>> and std::movable<range_value_t<Rng>>
        auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <typename To, typename Rng> requires concepts::can_smart_ptr_cast_range<To, Rng> and shared_ptr<range_value_t<Rng>>
        auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <typename To, typename Rng> requires concepts::can_smart_ptr_cast_range<To, Rng> and weak_ptr<range_value_t<Rng>>
        auto operator()(Rng &&rng) const -> auto {
            return this->operator()<To>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <typename To>
        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_smart_ptr_cast_range<To, Rng>
                (Rng &&rng) mutable -> auto {
                return this->operator()<To>(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(cast_static_);
    EXPORT_GENEX_VIEW(cast_dynamic_);
    EXPORT_GENEX_VIEW(cast_smart_ptr_);
}


#define cast_static cast_static_.operator()
#define cast_dynamic cast_dynamic_.operator()
#define cast_smart_ptr cast_smart_ptr_.operator()
