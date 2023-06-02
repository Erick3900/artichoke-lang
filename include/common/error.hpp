#pragma once

#include <string>

#include <tl/expected.hpp>

namespace arti {

    template <typename return_type>
    using expected_t = tl::expected<return_type, std::string>;

    using error_t = tl::unexpected<std::string>;

}
