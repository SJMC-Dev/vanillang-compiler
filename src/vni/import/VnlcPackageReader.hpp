#ifndef VNLC_PACKAGE_READER_HPP
#define VNLC_PACKAGE_READER_HPP

#include "ast/declaration/VnlcImportDeclarationItem.hpp"
#include "config/VnlcConfig.hpp"
#include "vni/import/VnlcImportedPackage.hpp"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

class VnlcPackageReader {
private:
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages;

    void readRecursively(const VnlcImportDeclarationItem& importItem, const std::vector<std::filesystem::path>& candidatePaths, VnlcImportedPackage* currentPackage);

public:
    VnlcPackageReader(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages);

    void readPackage(const VnlcImportDeclarationItem& importItem, const VnlcConfig& config);
};

#endif // VNLC_PACKAGE_READER_HPP