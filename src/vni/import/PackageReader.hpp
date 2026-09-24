#ifndef VNLC_PACKAGE_READER_HPP
#define VNLC_PACKAGE_READER_HPP

#include "config/Config.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <filesystem>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace vnlc {
    class PackageReader {
    private:
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& packages;

        void readFromPath(std::string_view path, const std::unordered_map<std::string, std::filesystem::path>& rootPaths);

        void readPackageContents(const std::filesystem::path& packagePath, ImportedPackage& package, std::unordered_set<std::filesystem::path>& activePackagePaths);

    public:
        PackageReader(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& packages);

        void readPackageFromPath(std::string_view path, const Config& config);
    };
} // namespace vnlc

#endif // VNLC_PACKAGE_READER_HPP
