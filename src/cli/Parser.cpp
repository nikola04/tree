#include "Parser.hpp"
#include <iostream>

const std::string RESET   = "\033[0m";
const std::string RED     = "\033[31m";

namespace cli {
    void write_error(const std::string& error) {
        std::cerr << RED << error << RESET << "\n";
    }

    tree::PrintOptions create_print_options(std::span<char*> arguments) {
        tree::PrintOptions print_options {};

        size_t i = 1;
        while(i < arguments.size()) {
            std::string arg { arguments[i] };
            i++;

            if (arg == "-L") {
                if (arguments.size() == i) {
                    write_error("wrong usecase of -L. Number argument required after -L.");
                    break;
                }

                std::string number_arg { arguments[i] };
                i++;

                try {
                    print_options.max_depth = std::stoul(number_arg);
                } catch(...) {
                    write_error("wrong usecase of -L. Argument after -L must be a valid number.");
                }

            } else if (arg == "-s") {
                if (arguments.size() == i) {
                    write_error("wrong usecase of -s. 'mix', 'dirs' or 'files' argument required after -s.");
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
                    write_error("wrong usecase of -s. 'mix', 'dirs' or 'file' argument required after -s.");

            } else if (arg == "-p") {
                if (arguments.size() == i) {
                    write_error("wrong usecase of -p. String argument required after -p.");
                    break;
                }

                if (!fs::exists(arguments[i])) {
                    write_error("wrong usecase of -p. String argument after -p is not valid path.");
                    break;
                }

                print_options.path = fs::weakly_canonical(arguments[i]);
                i++;

            } else {
                write_error("Invalid argument " + arg);
                continue;
            }
        }

        return print_options;
    }

    Command parse_arguments(std::span<char*> arguments) {
        for (size_t i = 1; i < arguments.size(); ++i) {
            std::string arg { arguments[i] };
            if (arg == "--help" || arg == "-h") return HelpCommand {};
        }

        return PrintCommand { create_print_options(arguments) };
    }
}