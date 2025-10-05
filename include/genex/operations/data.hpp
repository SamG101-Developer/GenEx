#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::operations::concepts {
    template <typename Rng>
    concept datable_range =
        range<Rng> and
        has_member_data<Rng>;
}


namespace genex::operations {
    struct data_fn {
        template <typename Rng>
        requires concepts::datable_range<Rng>
        constexpr auto operator()(Rng &&r) const -> auto {
            return r.data();
        }
    };

    GENEX_EXPORT_STRUCT(data);
}
