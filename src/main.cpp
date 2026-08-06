#include <cstddef>
#include <span>
#include <print>
#include <variant>

#include "cli/Parser.hpp"
#include "tree/Printer.hpp"

struct CommandVisitor {
    void operator()(const cli::HelpCommand& help_command) {
        std::println("tree — display directory structure as a tree\n");
        std::println("USAGE:");
        std::println("  tree [OPTIONS]\n");
        std::println("OPTIONS:");
        std::println("  -L <depth>          Maximum depth to display (default: 1)");
        std::println("  -s <mix|dirs|files> Sort order for entries (default: mix)");
        std::println("  -p <path>           Path to display (default: current directory)");
        std::println("  -h, --help          Show this help message\n");
        std::println("EXAMPLES:");
        std::println("  tree -L 3 -s dirs -p ./src");
        std::println("  tree -p /Users/nikola/Projects");
    }

    void operator()(const cli::PrintCommand& print_command) {
        auto print_options = print_command.print_options;
        std::println("Drawing a tree for a path: \n{}\n", print_options.path.value_or("./").string());

        tree::ObjectPrinter fp { print_options };
        fp.print();
    }
};

int main(int argc, char* * argv) {
    auto cli_command { cli::parse_arguments({ argv, static_cast<size_t>(argc) }) };

    std::visit(CommandVisitor{}, cli_command);
}
