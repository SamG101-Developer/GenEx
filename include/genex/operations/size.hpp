#pragma once
#include <iterator>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/operations/_operations_base.hpp>


namespace genex::operations {
    template <typename Rng>
    concept has_member_size = requires(Rng &&r) { r.size(); };

    template <typename Rng>
    concept has_std_size = requires(Rng &&r) { std::size(r); };

    struct size_fn final : detail::operations_base {
        template <typename Rng> requires (has_member_size<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> std::size_t {
            return r.size();
        }

        template <typename Rng> requires (not has_member_size<Rng> and has_std_size<Rng>)
        constexpr auto operator()(Rng &&r) const noexcept -> std::size_t {
            return std::size(r);
        }

        template <typename T>
        constexpr auto operator()(generator<T> &&gen) const noexcept -> std::size_t {
            auto count = static_cast<std::size_t>(0);
            for (auto &&_ : std::move(gen)) {
                ++count;
            }
            return count;
        }

        constexpr auto operator()() const noexcept -> decltype(auto) {
            MAP_TO_BASE();
        }
    };

    EXPORT_GENEX_STRUCT(size);
}
