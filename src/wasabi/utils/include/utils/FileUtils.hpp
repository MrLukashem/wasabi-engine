
#pragma once

#include "utils/defs.hpp"

#include <cstddef>
#include <string>
#include <filesystem>


namespace wasabi::utils {

Bytes readFile(std::filesystem::path path) noexcept;

} // namespace wasabi::utils

