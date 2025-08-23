#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel_for<I> S> requires (
        categories::input_iterator<I> and
        unique_ptr<deref_value_t<I>>)
    auto do_ptr(I &&first, S &&last) -> generator<typename deref_value_t<I>::pointer> {
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <range Rng> requires (
        categories::input_range<Rng> and
        unique_ptr<range_value_t<Rng>>)
    auto do_ptr(Rng &&rng) -> generator<typename range_value_t<Rng>::pointer> {
        for (auto &&x : rng) {
            co_yield x.get();
        }
    }

    template <iterator I, sentinel_for<I> S> requires (
        categories::input_iterator<I> and
        shared_ptr<deref_value_t<I>>)
    auto do_ptr(I &&first, S &&last) -> generator<typename deref_value_t<I>::pointer> {
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <range Rng> requires (
        categories::input_range<Rng> and
        shared_ptr<range_value_t<Rng>>)
    auto do_ptr(Rng &&rng) -> generator<typename range_value_t<Rng>::pointer> {
        for (auto &&x : rng) {
            co_yield x.get();
        }
    }

    template <iterator I, sentinel_for<I> S> requires (
        categories::input_iterator<I> and
        weak_ptr<deref_value_t<I>>)
    auto do_ptr(I &&first, S &&last) -> generator<typename deref_value_t<I>::pointer> {
        for (; first != last; ++first) {
            co_yield (*first).get();
        }
    }

    template <range Rng> requires (
        categories::input_range<Rng> and
        weak_ptr<range_value_t<Rng>>)
    auto do_ptr(Rng &&rng) -> generator<typename range_value_t<Rng>::pointer> {
        for (auto &&x : rng) {
            co_yield x.get();
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(ptr) {
        DEFINE_OUTPUT_TYPE(ptr);

        template <iterator I, sentinel_for<I> S> requires (
            categories::input_iterator<I> and
            unique_ptr<deref_value_t<I>> or shared_ptr<deref_value_t<I>> or weak_ptr<deref_value_t<I>>)
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_ptr, first, last);
        }

        template <range Rng> requires (
            categories::input_range<Rng> and
            unique_ptr<range_value_t<Rng>> or shared_ptr<range_value_t<Rng>> or weak_ptr<range_value_t<Rng>>)
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_ptr, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(ptr);
}
