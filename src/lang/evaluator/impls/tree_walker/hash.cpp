#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_hash(const ast::hash &hash, const std::shared_ptr<scope> &env) {
        auto obj = object{};
        obj.value = types::hash_t{};

        auto &hsh = std::get<types::hash_t>(obj.value);

        for (const auto &pair : hash.elements) {
            auto expected_key = eval(pair.key.get(), env);

            if (not expected_key) {
                return expected_key;
            }

            if (not (expected_key->is<types::string_t>() or (expected_key->is_reference() and expected_key->unwrap().is<types::string_t>()))) {
                return error_t{ "Expected a string as key of hash" };
            }

            auto &key = [&] () -> types::string_t& {
                if (expected_key->is_reference()) {
                    return expected_key->unwrap().as<types::string_t>();
                }

                return expected_key->as<types::string_t>();
            }();

            auto expected_value = eval(pair.value.get(), env);

            if (not expected_value) {
                return expected_value;
            }

            hsh[key] = std::move(expected_value).value();
        }

        return obj;
	}

}
