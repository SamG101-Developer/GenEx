#pragma once
#include <utility>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/operations/size.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::operations {
    template <typename Rng>
    concept has_member_at = requires(Rng &&r) { r.at(std::declval<std::size_t>()); };

    template <typename Rng>
    concept has_member_front = requires(Rng &&r) { r.front(); };

    template <typename Rng>
    concept has_member_back = requires(Rng &&r) { r.back(); };

    DEFINE_OPERATION(at) {
        template <typename Rng> requires (has_member_at<Rng>)
        constexpr auto operator()(Rng &&r, const std::size_t n) const noexcept -> range_value_t<Rng>& {
            return r.at(n);
        }

        template <typename T>
        constexpr auto operator()(std::generator<T> &&gen, const std::size_t n) const noexcept -> T {
            auto it = iterators::begin(gen);
            for (size_t i = 0; i < n && it != iterators::end(gen); ++i, ++it) {
            }
            return *it;
        }

        constexpr auto operator()(const std::size_t n) const noexcept -> auto {
            MAKE_CLOSURE(n);
        }
    };

    DEFINE_OPERATION(front) {
        template <typename Rng> requires (has_member_front<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.front();
        }

        template <typename Rng> requires (has_member_at<Rng> and not has_member_front<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.at(0);
        }

        template <typename Rng>
        constexpr auto operator()(Rng &&gen) const noexcept -> range_value_t<Rng> {
            return *gen.begin();
        }

        constexpr auto operator()() const noexcept -> auto {
            MAKE_CLOSURE();
        }
    };

    DEFINE_OPERATION(back) {
        template <typename Rng> requires (has_member_back<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.back();
        }

        template <typename Rng> requires (has_member_at<Rng> and not has_member_back<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> range_value_t<Rng>& {
            return r.at(size(r) - 1);
        }

        template <typename Rng>
        constexpr auto operator()(Rng &&gen) const noexcept -> range_value_t<Rng> {
            auto it = iterators::begin(gen);
            for (; it != iterators::end(gen); ++it) {
            }
            return *it;
        }

        constexpr auto operator()() const noexcept -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_OPERATION(at);
    EXPORT_GENEX_OPERATION(front);
    EXPORT_GENEX_OPERATION(back);
}
