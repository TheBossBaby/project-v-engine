#pragma once

#include <filesystem>
#include <stdexcept>

#include <projectV/engine/logging/log.hpp>

namespace projectv::engine
{
    /**
     * @brief Resolves an asset path against an assets root.
     *
     * Joins @p relativePath onto @p assetsRoot and normalises the result
     * lexically (removes "." and ".." segments and duplicate separators);
     * the file system is not touched. The result is absolute only if
     * @p assetsRoot is absolute, so pass an absolute root.
     *
     * The resolved path must stay inside @p assetsRoot, so a path such as
     * "../secret.txt" is rejected. Rooted paths are rejected too: absolute
     * paths, paths with a drive letter ("C:foo"), and, on Windows, paths
     * that start with a separator ("/foo").
     *
     * @param assetsRoot   Directory that contains the assets; must not be empty.
     * @param relativePath Asset path relative to @p assetsRoot; must not be
     *                     empty or rooted.
     * @return Normalised path to the asset; existence is not checked.
     *
     * @throws std::invalid_argument If @p assetsRoot or @p relativePath is
     *         empty, if @p relativePath is rooted, or if the resolved path
     *         leaves @p assetsRoot.
     */
    [[nodiscard]] inline std::filesystem::path ResolveAssetPath(
        const std::filesystem::path& assetsRoot,
        const std::filesystem::path& relativePath)
    {
        if(assetsRoot.empty())
        {
            LogError("ResolveAssetPath, assetsRoot is empty");
            throw std::invalid_argument("ResolveAssetPath, assetsRoot is empty");
        }

        if(relativePath.empty())
        {
            LogError("ResolveAssetPath, relativePath is empty");
            throw std::invalid_argument("ResolveAssetPath, relativePath is empty");
        }

        // is_absolute() is not enough on Windows: "/foo" and "C:foo" are not
        // absolute there, yet joining them would replace the assets root.
        if(relativePath.has_root_path())
        {
            LogError("ResolveAssetPath, asset path must be relative to the assets root");
            throw std::invalid_argument("ResolveAssetPath, asset path must be relative to the assets root");
        }

        const std::filesystem::path rootNormalised = assetsRoot.lexically_normal();
        const std::filesystem::path resolved       = (assetsRoot / relativePath).lexically_normal();

        // A leading ".." in the path from the root to the result means the
        // asset path climbed out of the assets root.
        const std::filesystem::path fromRoot = resolved.lexically_relative(rootNormalised);
        if(fromRoot.empty() || *fromRoot.begin() == "..")
        {
            LogError("ResolveAssetPath, asset path leaves the assets root");
            throw std::invalid_argument("ResolveAssetPath, asset path leaves the assets root");
        }

        return resolved;
    }
}
