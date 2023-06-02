#pragma once

#include <ranges>
#include <fstream>
#include <filesystem>

#include "parser/parser.hpp"
#include "evaluator/concept.hpp"
#include "evaluator/impls/tree_walker.hpp"

#include "evaluator/native_fns.hpp"

namespace arti::lang {

    template <concepts::evaluator evaluator_t = tree_walker>
    struct state {
        evaluator_t evaluator;

        void load_builtins() {
            evaluator.builtins["len"] = native_fns::len;
            evaluator.builtins["fmt"] = native_fns::fmt;
            evaluator.builtins["copy"] = native_fns::copy;
            evaluator.builtins["move"] = native_fns::move;
            evaluator.builtins["eval"] = native_fns::eval;
            evaluator.builtins["push"] = native_fns::push;
            evaluator.builtins["import"] = native_fns::import;
            evaluator.builtins["typeof"] = native_fns::typeof;
            evaluator.builtins["printf"] = native_fns::printf;
            evaluator.builtins["resize"] = native_fns::resize;
            evaluator.builtins["reserve"] = native_fns::reserve;
            evaluator.builtins["read_line"] = native_fns::read_line;
            evaluator.builtins["to_string"] = native_fns::to_string;
        }

        expected_t<void> load_file(std::string_view path) {
            if (not std::filesystem::exists(path)) {
                return error_t{ fmt::format("'{}' does not exists", path) };
            }

            std::ifstream in{ std::string{ path } };

            if (not in.is_open()) {
                return error_t{ fmt::format("Coudln't read '{}'", path) };
            }

            std::string code;
            std::string line;

            while (std::getline(in, line)) {
                code.append(std::move(line));
                code.push_back('\n');
            }

            return load_string(std::move(code));
        }

        expected_t<void> load_string(std::string code) {
            auto prsr = parser{ code };
            auto ok = prsr.parse();

            if (not ok) {
                return error_t{ std::move(ok).error() };
            }

            loaded = std::move(ok).value();
            return {};
        }

        expected_t<object> run() {
            if (not loaded) {
                return error_t{ "No code loaded" };
            }
            return evaluator.eval(loaded.get());
        }

        expected_t<object> run_string(std::string code) {
            auto expected_load = load_string(std::move(code));

            if (not expected_load) {
                return error_t{ std::move(expected_load).error() };
            }

            return run();
        }

        ast::ptr loaded;
    };

}
