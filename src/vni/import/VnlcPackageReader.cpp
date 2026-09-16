#include "VnlcPackageReader.hpp"
#include "error/VnlcPackageReaderError.hpp"
#include "vni/import/VnlcModuleInterfaceFileReader.hpp"
#include <filesystem>

VnlcPackageReader::VnlcPackageReader(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages) : packages(packages) {}

void VnlcPackageReader::readPackage(const VnlcImportDeclarationItem& importItem, const VnlcConfig& config) {
    readRecursively(importItem, config.dependencyPackageRootPaths, {}, nullptr);
}

void VnlcPackageReader::readRecursively(
    const VnlcImportDeclarationItem& importItem,
    const std::unordered_map<std::string, std::filesystem::path>& rootPaths,
    std::filesystem::path currentPath,
    VnlcImportedPackage* currentPackage
) {
    if (importItem.self) {
        if (currentPackage) {
            std::unordered_set<std::filesystem::path> activePackagePaths;
            readPackageContents(currentPath, *currentPackage, importItem, activePackagePaths);
        }
        return;
    }

    for (const auto& namePrefixNode : importItem.namePrefix) {
        std::string namePrefix = std::string(namePrefixNode->getIdentifierString());
        if (importItem.wildcard && namePrefix == "*") {
            return;
        }

        if (!currentPackage) {
            auto rootPath = rootPaths.find(namePrefix);
            if (rootPath == rootPaths.end()) {
                throw VnlcPackageReaderError(fmt::format("Could not find package with name: {}", namePrefix), namePrefixNode.get());
            }
            currentPath = rootPath->second;
            if (!std::filesystem::is_directory(currentPath)) {
                throw VnlcPackageReaderError(fmt::format("Package path {} is not a directory", currentPath.string()), namePrefixNode.get());
            }
            if (packages.find(namePrefix) == packages.end()) {
                packages.emplace(
                    namePrefix,
                    std::make_unique<VnlcImportedPackage>(
                        namePrefix,
                        std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>(),
                        std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>()
                    )
                );
            }

            currentPackage = packages[namePrefix].get();
        } else {
            std::filesystem::path packagePath = currentPath / namePrefix;
            if (!std::filesystem::is_directory(packagePath)) {
                std::filesystem::path modulePath = currentPath / (namePrefix + ".vni");
                if (!std::filesystem::is_regular_file(modulePath)) {
                    throw VnlcPackageReaderError(fmt::format("Could not find package or module with name: {}", namePrefix), namePrefixNode.get());
                }
                if (!currentPackage->getModules().contains(namePrefix)) {
                    VnlcModuleInterfaceFileReader moduleReader(modulePath, importItem);
                    currentPackage->addModule(moduleReader.read());
                }
                return;
            }

            if (!currentPackage->getSubPackages().contains(namePrefix)) {
                currentPackage->addSubPackage(
                    std::make_unique<VnlcImportedPackage>(
                        namePrefix,
                        std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>(),
                        std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>()
                    )
                );
            }
            currentPackage = currentPackage->getSubPackages().at(namePrefix).get();
            currentPath = std::move(packagePath);
        }
    }

    if (currentPackage && importItem.nameSuffixes.empty() && !importItem.wildcard) {
        std::unordered_set<std::filesystem::path> activePackagePaths;
        readPackageContents(currentPath, *currentPackage, importItem, activePackagePaths);
    }

    for (const auto& suffix : importItem.nameSuffixes) {
        readRecursively(*suffix, rootPaths, currentPath, currentPackage);
    }
}

void VnlcPackageReader::readPackageContents(
    const std::filesystem::path& packagePath,
    VnlcImportedPackage& package,
    const VnlcImportDeclarationItem& importItem,
    std::unordered_set<std::filesystem::path>& activePackagePaths
) {
    std::filesystem::path canonicalPath = std::filesystem::canonical(packagePath);
    if (!activePackagePaths.insert(canonicalPath).second) {
        throw VnlcPackageReaderError(fmt::format("Package directory cycle detected at {}", packagePath.string()));
    }
    for (const auto& entry : std::filesystem::directory_iterator(packagePath)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (!package.getSubPackages().contains(name)) {
                package.addSubPackage(
                    std::make_unique<VnlcImportedPackage>(
                        name,
                        std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>(),
                        std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>()
                    )
                );
            }
            readPackageContents(entry.path(), *package.getSubPackages().at(name), importItem, activePackagePaths);
        } else if (entry.is_regular_file() && entry.path().extension() == ".vni" && !package.getModules().contains(entry.path().stem().string())) {
            VnlcModuleInterfaceFileReader moduleReader(entry.path(), importItem);
            package.addModule(moduleReader.read());
        }
    }
    activePackagePaths.erase(canonicalPath);
}
