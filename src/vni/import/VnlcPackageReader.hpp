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

    void readRecursivelyFromSource(
        const VnlcImportDeclarationItem& importItem,
        const std::unordered_map<std::string, std::filesystem::path>& rootPaths,
        std::filesystem::path currentPath,
        VnlcImportedPackage* currentPackage
    );
    void readFromAlias(std::string_view aliasPath, const std::unordered_map<std::string, std::filesystem::path>& rootPaths);

    void readPackageContents(const std::filesystem::path& packagePath, VnlcImportedPackage& package, std::unordered_set<std::filesystem::path>& activePackagePaths);

public:
    VnlcPackageReader(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages);

    void readPackageFromSource(const VnlcImportDeclarationItem& importItem, const VnlcConfig& config);
    void readPackageFromAlias(std::string_view aliasPath, const VnlcConfig& config);
};

#endif // VNLC_PACKAGE_READER_HPP
