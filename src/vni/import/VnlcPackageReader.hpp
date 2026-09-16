#ifndef VNLC_PACKAGE_READER_HPP
#define VNLC_PACKAGE_READER_HPP

#include "ast/declaration/VnlcImportDeclarationItem.hpp"
#include "config/VnlcConfig.hpp"
#include "vni/import/VnlcImportedPackage.hpp"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class VnlcPackageReader {
private:
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages;

    void readRecursively(
        const VnlcImportDeclarationItem& importItem,
        const std::unordered_map<std::string, std::filesystem::path>& rootPaths,
        std::filesystem::path currentPath,
        VnlcImportedPackage* currentPackage
    );
    void readPackageContents(
        const std::filesystem::path& packagePath,
        VnlcImportedPackage& package,
        const VnlcImportDeclarationItem& importItem,
        std::unordered_set<std::filesystem::path>& activePackagePaths
    );

public:
    VnlcPackageReader(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages);

    void readPackage(const VnlcImportDeclarationItem& importItem, const VnlcConfig& config);
};

#endif // VNLC_PACKAGE_READER_HPP
