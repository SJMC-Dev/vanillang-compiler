#ifndef VNLC_CONFIG_HPP
#define VNLC_CONFIG_HPP

#include "config/VnlcRunningMode.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

struct VnlcConfig {
    VnlcRunningMode mode;

    std::string vanillangVersion;
    std::string minecraftVersion;

    std::filesystem::path packageRootPath;
    std::filesystem::path inputFilePath;
    std::optional<std::filesystem::path> outputDirectory;

    // module interface files will not be generated if moduleInterfaceOutputDirectory is std::nullopt
    // root package should be a subdirectory of this directory instead of itself
    std::optional<std::filesystem::path> moduleInterfaceOutputDirectory;

    // should include module interface files (.vni) instead of source files (.vnl)
    std::unordered_map<std::string, std::filesystem::path> dependencyPackageRootPaths;

    std::optional<int> optimizationLevel;
};

#endif // VNLC_CONFIG_HPP