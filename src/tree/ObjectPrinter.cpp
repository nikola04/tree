#include "Printer.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <iostream>
#include <variant>
#include <filesystem>
#include <vector>

const std::string RESET   = "\033[0m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string CYAN    = "\033[36m";

namespace tree {
    std::array<std::string, 4> depth_colors {
        BLUE, CYAN, GREEN, YELLOW
    };

    ObjectPrinter::ObjectPrinter(tree::PrintOptions print_options): m_print_options(print_options) {
    }

    void ObjectPrinter::operator ()(const Root& root) {
        std::cout << "*\n";
        print_children(root.path);
    }

    void ObjectPrinter::operator ()(const File& file) {
        std::cout << std::format("─ {}{}\n", 
            RESET,
            file.path.filename().string()
        );
    }

    void ObjectPrinter::operator ()(const Directory& dir) {
        bool is_expanded = !exceeds_depth();

        std::cout << std::format("─ {} {}\n", 
            dir.path.filename().string(),
            RESET
        );

        print_children(dir.path);
    }

    void ObjectPrinter::print() {
        fs::path path = m_print_options.path.value_or(fs::current_path());
        std::visit(*this, PrintableFileObject { Root { path } });
    }

    void ObjectPrinter::print_children(const fs::path path) {
        if (exceeds_depth()) return;
        
        std::vector<fs::directory_entry> childrens;
        for (const auto& child : fs::directory_iterator(path)) {
            childrens.push_back(child);
        }

        if (PrintSort sort = m_print_options.sort.value_or(PrintSort::MIX); 
            sort == PrintSort::FILES || sort == PrintSort::DIRS) 
        {
            std::sort(childrens.begin(), childrens.end(), [sort](const fs::directory_entry& a, const fs::directory_entry& b) {
                if (sort == PrintSort::DIRS)
                    return a.is_directory() && !b.is_directory();
                return !a.is_directory() && b.is_directory();
            });
        }

        for (size_t i = 0; i < childrens.size(); ++i) {
            fs::path child = childrens[i];
            bool is_last { i + 1 == childrens.size() };

            m_depth_stack.push_back(is_last);
            print_prefix(is_last);

            std::visit(*this, create_printable(child));

            m_depth_stack.pop_back();
        }
    }

    // │ └ ─ ├
    void ObjectPrinter::print_prefix(bool is_last) {
        size_t n { m_depth_stack.size() };
        for (size_t i = 0; i < n; ++i) {
            std::string color { depth_colors[i % depth_colors.size()] };

            if (i + 1 < n)
                std::cout << color << "│  ";
            else if (is_last)
                std::cout << color << "└──";
            else
                std::cout << color << "├──";
        }
    }

    PrintableFileObject ObjectPrinter::create_printable(const fs::path path) {
        if (fs::is_directory(path))
            return Directory { path };
        return File { path };
    }

    bool ObjectPrinter::exceeds_depth() {
        size_t max_depth { m_print_options.max_depth.value_or(1) };
        
        return m_depth_stack.size() >= max_depth;
    }
}