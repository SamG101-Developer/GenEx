#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/operations/_operation_base.hpp>


namespace genex::operations {
    template <typename Rng>
    concept can_size_range =
        input_range<Rng> and
        std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>;

    DEFINE_OPERATION(size) {
        template <typename Rng> requires (can_size_range<Rng> and has_member_size<Rng>)
        auto operator()(Rng &&r) const noexcept -> std::size_t {
            return r.size();
        }

        template <typename Rng> requires (can_size_range<Rng> and not has_member_size<Rng> and has_std_size<Rng>)
        auto operator()(Rng &&r) const noexcept -> std::size_t {
            return std::size(r);
        }

        template <typename Rng> requires (can_size_range<Rng> and not has_member_size<Rng> and not has_std_size<Rng>)
        auto operator()(Rng &&gen) const noexcept -> std::size_t {
            auto count = static_cast<std::size_t>(0);
            for (auto &&_ : gen) {
                ++count;
            }
            return count;
        }
    };

    EXPORT_GENEX_OPERATION(size);


    DEFINE_OPERATION(empty) {
        template <typename Rng> requires (has_member_empty<Rng>)
        auto operator()(Rng &&r) const noexcept -> bool {
            return r.empty();
        }

        template <typename Rng> requires (can_size_range<Rng> and not has_member_empty<Rng>)
        auto operator()(Rng &&r) const noexcept -> bool {
            return size(std::forward<Rng>(r)) == 0;
        }
    };

    EXPORT_GENEX_OPERATION(empty);
}
