#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_index(const ast::index &index, const std::shared_ptr<scope> &env) {
        auto expected_var = eval(index.variable.get(), env);

        if (not expected_var) {
            return expected_var;
        }

        auto var = std::move(expected_var).value();

        auto expected_idx = eval(index.index.get(), env);

        if (not expected_idx) {
            return expected_idx;
        }

        auto &idx = [&] () -> types::object_t& {
            if (expected_idx->is_reference()) {
                return expected_idx->unwrap();
            }
            return expected_idx.value();
        }();

        if (var.is_reference() and var.unwrap().is_object()) {
            auto &unw_var = var.unwrap();

            if (unw_var.is<types::array_t>() and idx.is<types::int_t>()) {
                auto &arr_ref = unw_var.as<types::array_t>();
                auto idx_v = idx.as<types::int_t>();

                if (idx_v >= types::int_t(arr_ref.size())) {
                    return error_t{ "Index out of bounds" };
                }

                if (arr_ref.at(idx_v).is_reference()) {
                    return arr_ref.at(idx_v);
                }

                return types::object_t{ &arr_ref.at(idx_v) };
            }
            else if (unw_var.is<types::hash_t>() and idx.is<types::string_t>()) {
                auto &hash_ref = unw_var.as<types::hash_t>();
                auto &idx_v = idx.as<types::string_t>();

                if (hash_ref[idx_v].is_reference()) {
                    return hash_ref[idx_v];
                }

                return types::object_t{ &hash_ref[idx_v] };
            }
            else if (unw_var.is<types::string_t>() and idx.is<types::int_t>()) {
                auto &str_ref = unw_var.as<types::string_t>();
                auto idx_v = idx.as<types::int_t>();

                if (idx_v >= types::int_t(str_ref.size())) {
                    return error_t{ "Index out of bounds" };
                }

                return str_ref.at(idx_v);
            }
        }
        else {
            if (var.is<types::array_t>() and idx.is<types::int_t>()) {
                auto &arr_ref = var.as<types::array_t>();
                auto idx_v = idx.as<types::int_t>();

                if (idx_v >= types::int_t(arr_ref.size())) {
                    return error_t{ "Index out of bounds" };
                }

                return arr_ref.at(idx_v);
            }
            else if (var.is<types::hash_t>() and idx.is<types::int_t>()) {
                auto &hash_ref = var.as<types::hash_t>();
                auto &idx_v = idx.as<types::string_t>();

                return hash_ref[idx_v];
            }
            else if (var.is<types::string_t>() and idx.is<types::int_t>()) {
                auto &str_ref = var.as<types::string_t>();
                auto idx_v = idx.as<types::int_t>();

                if (idx_v >= types::int_t(str_ref.size())) {
                    return error_t{ "Index out of bounds" };
                }

                return str_ref.at(idx_v);
            }
        }

        return error_t{ fmt::format("Invalid index expression {} {}", var.type(), idx.type()) };
	}

}
