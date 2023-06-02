#include "evaluator/native_fns.hpp"

#include <fstream>
#include <filesystem>

#include "parser/parser.hpp"
#include "evaluator/scope.hpp"

namespace arti::lang::native_fns {

    static expected_t<std::filesystem::path> resolve_path(std::string_view path);

    expected_t<types::object_t> import(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() != 1) {
            return error_t{ "Number of arguments mismatch" };
        }

        auto &path_obj = params.front();

        if (not path_obj.is<types::string_t>()) {
            return error_t{ "Import expected a string path" };
        }

        auto &unw_path = path_obj.as<types::string_t>();

        auto expected_path = resolve_path(unw_path);

        if (not expected_path) {
            return error_t{ std::move(expected_path).error() };
        }

        auto expected_source = [&] () -> expected_t<std::string> {
            std::string source;
            std::string line;

            std::ifstream source_istream{ expected_path.value() };

            if (not source_istream.is_open()) {
                return error_t{ fmt::format("Couldn't open '{}'", expected_path.value().string()) };
            }

            while (std::getline(source_istream, line)) {
                source.append(std::move(line));
                source.push_back('\n');
            }

            return source;
        }();

        if (not expected_source) {
            return error_t{ std::move(expected_source).error() };
        }

        auto source = std::move(expected_source).value();

        auto prsr = parser{ source };
        auto expected_ast = prsr.parse();

        if (not expected_ast) {
            return error_t{ std::move(expected_ast).error() };
        }

        auto new_env = scope::new_enclosed(env);
        return eval(expected_ast->get(), new_env);
    }

    static expected_t<std::filesystem::path> resolve_path(std::string_view path) {
        if (std::filesystem::exists(path)) {
            return path;
        }
        if (std::filesystem::exists(fmt::format("{}.arti", path))) {
            return fmt::format("{}.arti", path);
        }
        if (std::filesystem::exists(fmt::format("{}/init.arti", path))) {
            return fmt::format("{}/init.arti", path);
        }
        return error_t{ fmt::format("Not a valid artichoke lang path") };
    }

}
