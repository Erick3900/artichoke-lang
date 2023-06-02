#pragma once

#include <list>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "error.hpp"

namespace arti::lang {
    struct scope;
    struct object;
    struct ast_node;
    struct function;

    namespace types {

        struct null_t{};
        using object_t = object;

        using eval_fn_t = std::function<expected_t<object_t>(ast_node *, const std::shared_ptr<scope> &)>;

        using fn_t = function;
        using native_fn_t = std::function<expected_t<object_t>(std::list<object_t>, eval_fn_t &&, const std::shared_ptr<scope> &)>;

        using char_t = char;
        using bool_t = bool;
        using int_t = int64_t;
        using float_t = double;
        using reference_t = object *;
        using string_t = std::string;
        using array_t = std::vector<object>;
        using hash_t = std::unordered_map<string_t, object>;

    }
}
