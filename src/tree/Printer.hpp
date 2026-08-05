#pragma once

#include <cstddef>
#include <optional>
#include <variant>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace tree {
    enum class PrintSort {
        MIX,
        DIRS,
        FILES,
    };

    struct PrintOptions {
        std::optional<size_t> max_depth {};
        std::optional<fs::path> path {};
        std::optional<PrintSort> sort {};
    };

    struct Root {
        fs::path path;
    };

    struct File {
        fs::path path;
    };

    struct Directory {
        fs::path path;
    };

    using PrintableFileObject = std::variant<Root, File, Directory>;

    class ObjectPrinter {
    public:
        ObjectPrinter(PrintOptions print_options);

        void operator ()(const Root& root);
        void operator ()(const File& file);
        void operator ()(const Directory& dir);

        void print();

    private:
        PrintableFileObject create_printable(const fs::path path);

        void print_children(const fs::path path);
        void print_prefix(bool is_last);

        bool exceeds_depth();

        PrintOptions m_print_options {};
        std::vector<bool> m_depth_stack {};
    };
}