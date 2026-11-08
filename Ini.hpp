#ifndef INI_HPP
#define INI_HPP

#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Ini {

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
    inline ErrorCode load(std::string_view filepath) {
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

    /*
    * Retrieves a value converted to target type T with fallback support.
    * Return type: T
    * Use case: Type-safe retrieval for std::string, int, float, bool, etc.
    */
    template <typename T>
    [[nodiscard]] inline T get(std::string_view section, std::string_view key, const T& fallback) const {
        auto sec_it = m_data.find(std::string(section));
        if (sec_it == m_data.end()) return fallback;

        auto key_it = sec_it->second.find(std::string(key));
        if (key_it == sec_it->second.end()) return fallback;

        const std::string& val = key_it->second;

        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, bool>) {
            std::string lower_val = val;
            std::transform(lower_val.begin(), lower_val.end(), lower_val.begin(), ::tolower);
            if (lower_val == "true" || lower_val == "yes" || lower_val == "on" || lower_val == "1") return true;
            if (lower_val == "false" || lower_val == "no" || lower_val == "off" || lower_val == "0") return false;
            return fallback;    
        } else {
            std::stringstream ss(val);
            T result;
            if (ss >> result) {
                return result;
            }
            return fallback;
        }
    }

    /*
    * Sets or updates an entry for a given section and key using templates.
    * Return type: void
    * Use case: Set settings directly using native C++ types (int, float, std::string, bool).
    */
    template <typename T>
    inline void set(std::string_view section, std::string_view key, const T& value) {
        if constexpr (std::is_same_v<T, bool>) {
            m_data[std::string(section)][std::string(key)] = value ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>) {
            m_data[std::string(section)][std::string(key)] = std::string(value);
        } else {
            m_data[std::string(section)][std::string(key)] = std::to_string(value);
        }
    }

    /*
    * Writes the in-memory data back out to disk.
    * Return type: ErrorCode
    * Use case: Persisting updated settings to an INI or CFG file.
    */
    inline ErrorCode save(std::string_view filepath = {}) const {
        std::string target_file = filepath.empty() ? m_filename : std::string(filepath);
        if (target_file.empty()) {
            return ErrorCode::WriteError;
        }
    
        std::ofstream file(target_file);
        if (!file.is_open()) {
            return ErrorCode::WriteError;
        }
    
        for (const auto& [section, keys] : m_data) {
            if (!section.empty()) {
                file << "[" << section << "]\n";
            }
            for (const auto& [key, value] : keys) {
                file << key << " = " << value << "\n";
            }
            file << "\n";
        }
    
        return ErrorCode::Success;
    }
    
   /*
    * Clears all in memory data.
    * Return type: void
    * Use case: Resetting state before reloading a configuration file.
    */
    inline void iniclear() noexcept {
        m_data.clear();
        m_filename.clear();
    }

private:
    std::string m_filename;
    DataMap m_data;
};

}

#endif