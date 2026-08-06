#pragma once

#include "../tree/Printer.hpp"
#include <span>
#include <variant>

namespace cli {
    struct PrintCommand {
        tree::PrintOptions print_options;
    };

    struct HelpCommand {
    };

    using Command = std::variant<PrintCommand, HelpCommand>;

    Command parse_arguments(std::span<char*> arguments);
}