#include "PackageReader.hpp"
#include "error/PackageReaderError.hpp"
#include "vni/import/ModuleInterfaceFileReader.hpp"
#include <filesystem>
#include <vector>

namespace vnlc {
    PackageReader::PackageReader(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& packages) : packages(packages) {}

    void PackageReader::readPackageFromSource(const ImportDeclarationItem& importItem, const Config& config) {
        readRecursivelyFromSource(importItem, config.dependencyPackageRootPaths, {}, nullptr);
    }

    void PackageReader::readPackageFromAlias(std::string_view aliasPath, const Config& config) {
        readFromAlias(aliasPath, config.dependencyPackageRootPaths);
    }

    void PackageReader::readFromAlias(std::string_view aliasPath, const std::unordered_map<std::string, std::filesystem::path>& rootPaths) {
        std::vector<std::string> names;
        for (std::size_t start = 0;;) {
            std::size_t end = aliasPath.find('.', start);
            std::string_view name = aliasPath.substr(start, end == std::string_view::npos ? end : end - start);
            if (name.empty() || name.find_first_of("/\\:") != std::string_view::npos || name.find('\0') != std::string_view::npos) {
                throw PackageReaderError(fmt::format("Invalid imported alias path: {}", aliasPath));
            }
            names.emplace_back(name);
            if (end == std::string_view::npos) break;
            start = end + 1;
        }

        auto rootPath = rootPaths.find(names.front());
        if (rootPath == rootPaths.end()) {
            throw PackageReaderError(fmt::format("Could not find package with name: {}", names.front()));
        }
        std::filesystem::path currentPath = rootPath->second;
        if (!std::filesystem::is_directory(currentPath)) {
            throw PackageReaderError(fmt::format("Package path {} is not a directory", currentPath.string()));
        }
        if (!packages.contains(names.front())) {
            packages.emplace(
                names.front(),
                std::make_unique<ImportedPackage>(
                    names.front(),
                    std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                    std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                )
            );
        }
        ImportedPackage* currentPackage = packages.at(names.front()).get();

        for (std::size_t index = 1; index < names.size(); ++index) {
            const std::string& name = names[index];
            std::filesystem::path packagePath = currentPath / name;
            if (!std::filesystem::is_directory(packagePath)) {
                if (currentPackage->getModules().contains(name)) return;

                std::filesystem::path modulePath = currentPath / (name + ".vni");
                if (!std::filesystem::is_regular_file(modulePath)) {
                    throw PackageReaderError(fmt::format("Could not find package or module with name: {}", name));
                }
                ModuleInterfaceFileReader moduleReader(modulePath);
                currentPackage->addModule(moduleReader.read());
                return;
            }

            if (!currentPackage->getSubPackages().contains(name)) {
                currentPackage->addSubPackage(
                    std::make_unique<ImportedPackage>(
                        name,
                        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                        std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                    )
                );
            }
            currentPackage = currentPackage->getSubPackages().at(name).get();
            currentPath = std::move(packagePath);
        }

        std::unordered_set<std::filesystem::path> activePackagePaths;
        readPackageContents(currentPath, *currentPackage, activePackagePaths);
    }

    void PackageReader::readRecursivelyFromSource(
        const ImportDeclarationItem& importItem,
        const std::unordered_map<std::string, std::filesystem::path>& rootPaths,
        std::filesystem::path currentPath,
        ImportedPackage* currentPackage
    ) {
        if (importItem.self) {
            if (currentPackage) {
                std::unordered_set<std::filesystem::path> activePackagePaths;
                readPackageContents(currentPath, *currentPackage, activePackagePaths);
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
                    throw PackageReaderError(fmt::format("Could not find package with name: {}", namePrefix), namePrefixNode.get());
                }
                currentPath = rootPath->second;
                if (!std::filesystem::is_directory(currentPath)) {
                    throw PackageReaderError(fmt::format("Package path {} is not a directory", currentPath.string()), namePrefixNode.get());
                }
                if (packages.find(namePrefix) == packages.end()) {
                    packages.emplace(
                        namePrefix,
                        std::make_unique<ImportedPackage>(
                            namePrefix,
                            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                            std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                        )
                    );
                }

                currentPackage = packages[namePrefix].get();
            } else {
                std::filesystem::path packagePath = currentPath / namePrefix;
                if (!std::filesystem::is_directory(packagePath)) {
                    std::filesystem::path modulePath = currentPath / (namePrefix + ".vni");
                    if (!std::filesystem::is_regular_file(modulePath)) {
                        throw PackageReaderError(fmt::format("Could not find package or module with name: {}", namePrefix), namePrefixNode.get());
                    }
                    if (!currentPackage->getModules().contains(namePrefix)) {
                        ModuleInterfaceFileReader moduleReader(modulePath);
                        currentPackage->addModule(moduleReader.read());
                    }
                    return;
                }

                if (!currentPackage->getSubPackages().contains(namePrefix)) {
                    currentPackage->addSubPackage(
                        std::make_unique<ImportedPackage>(
                            namePrefix,
                            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                            std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                        )
                    );
                }
                currentPackage = currentPackage->getSubPackages().at(namePrefix).get();
                currentPath = std::move(packagePath);
            }
        }

        if (currentPackage && importItem.nameSuffixes.empty() && !importItem.wildcard) {
            std::unordered_set<std::filesystem::path> activePackagePaths;
            readPackageContents(currentPath, *currentPackage, activePackagePaths);
        }

        for (const auto& suffix : importItem.nameSuffixes) {
            readRecursivelyFromSource(*suffix, rootPaths, currentPath, currentPackage);
        }
    }

    void PackageReader::readPackageContents(const std::filesystem::path& packagePath, ImportedPackage& package, std::unordered_set<std::filesystem::path>& activePackagePaths) {
        std::filesystem::path canonicalPath = std::filesystem::canonical(packagePath);
        if (!activePackagePaths.insert(canonicalPath).second) {
            throw PackageReaderError(fmt::format("Package directory cycle detected at {}", packagePath.string()));
        }
        for (const auto& entry : std::filesystem::directory_iterator(packagePath)) {
            if (entry.is_directory()) {
                std::string name = entry.path().filename().string();
                if (!package.getSubPackages().contains(name)) {
                    package.addSubPackage(
                        std::make_unique<ImportedPackage>(
                            name,
                            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                            std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                        )
                    );
                }
                readPackageContents(entry.path(), *package.getSubPackages().at(name), activePackagePaths);
            } else if (entry.is_regular_file() && entry.path().extension() == ".vni" && !package.getModules().contains(entry.path().stem().string())) {
                ModuleInterfaceFileReader moduleReader(entry.path());
                package.addModule(moduleReader.read());
            }
        }
        activePackagePaths.erase(canonicalPath);
    }
} // namespace vnlc
