#ifndef INI_HPP
#define INI_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <optional>
#include <system_error>

namespace ini {

enum class ErrorCode {
    Success = 0,
    FileNotFound,
    SyntaxError,
    WriteError
};

class Ini {
public:
    using SectionMap = std::unordered_map<std::string, std::string>;
    using DataMap = std::unordered_map<std::string, SectionMap>;

    Ini() = default;

    /*
    * Trims leading and trailing whitespace from a string_view inline.
    * Return type: std::string_view
    * Use case: Internal helper for fast parsing without allocations.
    */
    [[nodiscard]] static inline std::string_view trim(std::string_view str) noexcept {
        const auto start = str.find_first_not_of(" \t\r\n");
        if (start == std::string_view::npos) return {};
        const auto end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    /*
    * Loads and parses an .ini file from disk into memory.
    * Return type: ErrorCode
    * Use case: Call this during application initialization.
    */
    inline ErrorCode iniload(std::string_view filepath) {
        m_filename = filepath;
        m_data.clear();

        std::ifstream file(m_filename);
        if (!file.is_open()) {
            return ErrorCode::FileNotFound;
        }

        std::string line;
        std::string current_section;

        while (std::getline(file, line)) {
            std::string_view sv = trim(line);

            if (sv.empty() || sv.front() == '#' || sv.front() == ';') {
                continue;
            }

            if (sv.front() == '[') {
                if (sv.back() != ']') {
                    return ErrorCode::SyntaxError;
                }
                current_section = std::string(trim(sv.substr(1, sv.size() - 2)));
                continue;
            }
            const auto eq_pos = sv.find('=');
            if (eq_pos != std::string_view::npos) {
                std::string key(trim(sv.substr(0, eq_pos)));
                std::string_view val_sv = trim(sv.substr(eq_pos + 1));
            
                if (val_sv.size() >= 2 && val_sv.front() == '"' && val_sv.back() == '"') {
                    val_sv = val_sv.substr(1, val_sv.size() - 2);
                }
            
                m_data[current_section][key] = std::string(val_sv);
                }
            else {
                return ErrorCode::SyntaxError;
            }
        }
        return ErrorCode::Success;
    }

private:
    std::string m_filename;
    DataMap m_data;
};

}

#endif