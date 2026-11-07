#pragma once

#include <string_view>

#ifndef ASSETROOTLOCATION
#error "ASSETROOTLOCATION is not defined. CMake sets it from PROJECTV_ASSET_ROOT, see the engine CMakeLists.txt."
#endif

namespace projectv::engine
{
    /**
     * @brief Directory that contains the assets, fixed at build time.
     *
     * CMake fills it in from the PROJECTV_ASSET_ROOT cache variable through
     * the ASSETROOTLOCATION compile definition. Asset paths in a LevelDesc
     * are relative to this directory; resolve them with ResolveAssetPath().
     *
     * The path is baked into the binary, so after moving the repository
     * re-run CMake (or pass -DPROJECTV_ASSET_ROOT=...) to update it.
     */
    inline constexpr std::string_view ASSETROOT = ASSETROOTLOCATION;
}
