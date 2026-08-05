#include <cstddef>
#include <iostream>
#include <span>
#include <optional>
#include <variant>
#include <print>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct PrintOptions {
    std::optional<size_t> max_depth {};
    std::optional<fs::path> path {};
};

PrintOptions create_print_options(std::span<char*> arguments) {
    PrintOptions print_options {};

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
        } else if (arg == "-p") {
            if (arguments.size() == i) {
                std::cerr << "wrong usecase of -p. String argument required after -p. \n";
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


// │
//
// └
//
// ─
//
// ├

class FilePrinter {
public:
    FilePrinter(PrintOptions print_options): m_print_options(print_options) {}

    void operator ()(const Root& root) {
        std::cout << "." << "\n";
        print_children(root.path);
    }

    void operator ()(const File& file) {
        std::cout << file.path.filename() << "\n";
    }

    void operator ()(const Directory& dir) {
        std::cout << dir.path.filename() << "\n";
        print_children(dir.path);
    }

    void print() {
        fs::path path = m_print_options.path.value_or(fs::current_path());
        std::visit(*this, PrintableFileObject { Root { path } });
    }

private:
    void print_children(const fs::path path) {
        if (m_depth_stack.size() >= m_print_options.max_depth) return;
        
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

    void print_prefix(bool is_last) {
        for (size_t i = 0; i < m_depth_stack.size() - 1; ++i)
            std::cout << "│  ";

        if (is_last) 
            std::cout << "└──";
        else
            std::cout << "├──";
    }

    PrintableFileObject create_printable(const fs::path path) {
        if (fs::is_directory(path))
            return Directory { path };
        return File { path };
    }

    PrintOptions m_print_options {};
    std::vector<bool> m_depth_stack {};
};

int main(int argc, char* * argv) {
    auto print_options { create_print_options({ argv, static_cast<size_t>(argc) }) };

    std::println("-----OPTIONS-----");
    std::println("maximum depth: {}", print_options.max_depth.value_or(0));
    std::println("path: {}", print_options.path.value_or("null").c_str());
    std::println();

    FilePrinter fp { print_options };
    fp.print();
}
