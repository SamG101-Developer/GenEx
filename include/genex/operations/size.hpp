#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/operations/_operation_base.hpp>


namespace genex::operations {
    DEFINE_OPERATION(size) {
        template <typename Rng> requires (has_member_size<Rng>)
        auto operator()(Rng &&r) const noexcept -> std::size_t {
            return r.size();
        }

        template <typename Rng> requires (not has_member_size<Rng> and has_std_size<Rng>)
        auto operator()(Rng &&r) const noexcept -> std::size_t {
            return std::size(r);
        }

        template <typename Rng>
        auto operator()(Rng &&gen) const noexcept -> std::size_t {
            auto count = static_cast<std::size_t>(0);
            for (auto &&_ : std::move(gen)) {
                ++count;
            }
            return count;
        }

        auto operator()() const noexcept -> auto {
            return
                [FWD_CAPTURES()]<typename Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_OPERATION(empty) {
        template <typename Rng> requires (has_member_empty<Rng>)
        auto operator()(Rng &&r) const noexcept -> bool {
            return r.empty();
        }

        template <typename Rng> requires (!has_member_empty<Rng>)
        auto operator()(Rng &&r) const noexcept -> bool {
            return size(std::forward<Rng>(r)) == 0;
        }

        auto operator()() const noexcept -> auto {
            return
                [FWD_CAPTURES()]<typename Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_OPERATION(size);
    EXPORT_GENEX_OPERATION(empty);
}
