#pragma once

namespace arti {

    template <typename ...Ts>
    struct overload_set : Ts... {
        using Ts::operator()...;
    };

    template <typename ...Ts>
    overload_set(Ts...) -> overload_set<Ts...>;

}
