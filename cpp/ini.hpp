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

    Ini = default();
};

}

#endif