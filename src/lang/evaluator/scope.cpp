#include "evaluator/scope.hpp"

namespace arti::lang {

    std::shared_ptr<scope> scope::new_enclosed(std::shared_ptr<scope> outer) {
        auto new_scope = std::make_shared<scope>();
        new_scope->outer = std::move(outer);
        return new_scope;
    }

    expected_t<std::reference_wrapper<object>> scope::get_ref(const std::string &name) {
        if (auto it = variables.find(name); it != variables.end()) {
            return it->second;
        }

        if (outer != nullptr) {
            if (auto value = outer->get_ref(name); value) {
                return value.value().get();
            }
        }

        return error_t{ fmt::format("Variable '{}' is not defined", name) };
    }

    expected_t<object> scope::get(const std::string &name) {
        if (auto it = variables.find(name); it != variables.end()) {
            if (it->second.is_object()) {
                return object{ &it->second };
            }
            return it->second;
        }

        if (outer != nullptr) {
            if (auto value = outer->get(name); value) {
                if (value->is_object()) {
                    return object{ &(value.value()) };
                }
                return value.value();
            }
        }

        return error_t{ fmt::format("Variable '{}' is not defined", name) };
    }


    bool scope::is_defined(const std::string &name) {
        if (variables.contains(name)) {
            return true;
        }

        if (outer != nullptr) {
            return outer->is_defined(std::move(name));
        }
        return false;
    }

    void scope::set(const std::string &name, object obj) {
        variables[name] = std::move(obj);
    }

}
