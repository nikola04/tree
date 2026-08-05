#include <cstddef>
#include <iostream>
#include <span>
#include <optional>
#include <print>
#include <filesystem>

#include "tree/Printer.hpp"

tree::PrintOptions create_print_options(std::span<char*> arguments) {
    tree::PrintOptions print_options {};

    size_t i = 1;
    while(i < arguments.size()) {
        std::string arg { arguments[i] };
        i++;

        if (arg == "-L") {
            if (arguments.size() == i) {
                std::cerr << "wrong usecase of -L. Number argument required after -L. \n";
                break;
            }

            std::string number_arg { arguments[i] };
            i++;

            try {
                print_options.max_depth = std::stoul(number_arg);
            } catch(...) {
                std::cerr << "wrong usecase of -L. Argument after -L must be a valid number. \n";
            }

        } else if (arg == "-s") {
            if (arguments.size() == i) {
                std::cerr << "wrong usecase of -s. 'mix', 'dirs' or 'files' argument required after -s. \n";
                break;
            }

            std::string sort_arg { arguments[i] };
            i++;

            if (sort_arg == "mix")
                print_options.sort = tree::PrintSort::MIX;
            else if (sort_arg == "files")
                print_options.sort = tree::PrintSort::FILES;
            else if (sort_arg == "dirs")
                print_options.sort = tree::PrintSort::DIRS;
            else
                std::cerr << "wrong usecase of -s. 'mix', 'dirs' or 'file' argument required after -s. \n";

        } else if (arg == "-p") {
            if (arguments.size() == i) {
                std::cerr << "wrong usecase of -p. String argument required after -p. \n";
                break;
            }

            if (!fs::exists(arguments[i])) {
                std::cerr << "wrong usecase of -p. String argument after -p is not valid path. \n";
                break;
            }

            print_options.path = fs::weakly_canonical(arguments[i]);
            i++;

        } else {
            std::cerr << "Invalid argument " << arg << "\n";
            continue;
        }
    }

    return print_options;
}

int main(int argc, char* * argv) {
    auto print_options { create_print_options({ argv, static_cast<size_t>(argc) }) };

    // std::println("-----OPTIONS-----");
    // std::println("maximum depth: {}", print_options.max_depth.value_or(0));
    // std::println();

    std::println("Drawing a tree for a path: \n{}\n", print_options.path.value_or("./").string());

    tree::ObjectPrinter fp { print_options };
    fp.print();
}
