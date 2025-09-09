//
// Created by Łukasz Merta on 06/09/2025.
//

#pragma once

#include <string>

namespace wasabi::rendering {

enum class ShaderStage {
    Vertex,
    Fragment,
    Geometry,
    Compute
};

struct ShaderInfo {
    ShaderStage stage;
    std::string path;
};

} // namespace rendering
