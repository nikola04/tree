#include "Printer.hpp"

#include <cstddef>
#include <iostream>
#include <variant>
#include <filesystem>
#include <vector>

namespace tree {
    ObjectPrinter::ObjectPrinter(tree::PrintOptions print_options): m_print_options(print_options) {
    }

    void ObjectPrinter::operator ()(const Root& root) {
        std::cout << ".\n";
        print_children(root.path);
    }

    void ObjectPrinter::operator ()(const File& file) {
        std::cout << " " << file.path.filename() << "\n";
    }

    void ObjectPrinter::operator ()(const Directory& dir) {
        std::cout << " " << dir.path.filename() << "\n";
        print_children(dir.path);
    }

    void ObjectPrinter::print() {
        fs::path path = m_print_options.path.value_or(fs::current_path());
        std::visit(*this, PrintableFileObject { Root { path } });
    }

    void ObjectPrinter::print_children(const fs::path path) {
        if (exceeds_depth()) return;
        
        std::vector<fs::path> childrends;
        for (const auto& child : fs::directory_iterator(path)) {
            childrends.push_back(child.path());
        }

        for (size_t i = 0; i < childrends.size(); ++i) {
            fs::path child = childrends[i];
            bool is_last { i + 1 == childrends.size() };

            m_depth_stack.push_back(is_last);
            print_prefix(is_last);

            std::visit(*this, create_printable(child));

            m_depth_stack.pop_back();
        }
    }

    // │ └ ─ ├
    void ObjectPrinter::print_prefix(bool is_last) {
        for (size_t i = 0; i < m_depth_stack.size() - 1; ++i)
            std::cout << "│  ";

        if (is_last) 
            std::cout << "└──";
        else
            std::cout << "├──";
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