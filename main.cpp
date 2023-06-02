#include <fstream>
#include <iostream>
#include <filesystem>

#include <internal/config.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <docopt/docopt.h>

#include "evaluator/state.hpp"

const auto Usage{
    fmt::format(
        R"(Artichoke Lang.
    {}

    Usage:
        arti-lang [options]
        arti-lang [options] <file>

    Options:
        --help      Show this screen
        --version   Show program version
        --ast       Just dump the AST to stdout
    )",
        arti::config::project_description
    )
};

int main(int argc, char* argv[]) {
    using OptionsMap_t = std::map<std::string, docopt::value>;

    const OptionsMap_t options = docopt::docopt(
        Usage,
        { argv + 1, argv + argc },
        true,
        fmt::format("{} v{}, by {}",
            arti::config::project_name,
            arti::config::project_version,
            arti::config::project_author_github
        )
    );

    auto just_ast = [&]{
        if (options.contains("--ast")) {
            return options.at("--ast").asBool();
        }

        return false;
    }();

    arti::lang::state state;
    state.load_builtins();

    if (options.at("<file>").kind() != docopt::Kind::Empty) {
        auto file = options.at("<file>").asString();

        auto loaded_file = state.load_file(file);
        if (not loaded_file) {
            fmt::print("Error: {}\n", loaded_file.error());
        }

        if (just_ast) {
            fmt::print("{}\n", state.loaded->dump());
            return 0;
        }

        auto start = std::chrono::high_resolution_clock::now();
        auto result = state.run();
        auto end = std::chrono::high_resolution_clock::now();

        if (not result) {
            fmt::print("Error: {}\n", result.error());
        }
        else {
            fmt::print(
                "Program returned: {}\nExecution time: {}\n",
                result.value(),
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            );
        }
    }
    else {
        fmt::print("Welcome to the artichoke lang repl!\n>> ");

        std::string line;

        while (std::getline(std::cin, line)) {
            auto loaded = state.load_string(std::move(line));

            if (not loaded) {
                fmt::print("Error: {}\n", loaded.error());
            }
            else {
                auto result = state.run();

                if (not result) {
                    fmt::print("Error: {}\n", result.error());
                }
                else {
                    fmt::print("{}\n", result->to_string());
                }
            }

            fmt::print(">> ");
        }
    }
}
