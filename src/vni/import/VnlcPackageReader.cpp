#include "VnlcPackageReader.hpp"
#include "error/VnlcPackageReaderError.hpp"
#include "vni/import/VnlcModuleInterfaceFileReader.hpp"
#include <algorithm>

VnlcPackageReader::VnlcPackageReader(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& packages) : packages(packages) {}

void VnlcPackageReader::readPackage(const VnlcImportDeclarationItem& importItem, const VnlcConfig& config) {
    std::vector<std::filesystem::path> candidatePackages;
    std::transform(config.dependencyPackageRootPaths.begin(), config.dependencyPackageRootPaths.end(), std::back_inserter(candidatePackages), [&](const auto& package) {
        return package.first;
    });

    readRecursively(importItem, candidatePackages, nullptr);
}

void VnlcPackageReader::readRecursively(const VnlcImportDeclarationItem& importItem, const std::vector<std::filesystem::path>& candidatePaths, VnlcImportedPackage* currentPackage) {
    std::vector<std::filesystem::path> currentCandidatePaths = std::move(candidatePaths);

    for (const auto& namePrefixNode : importItem.namePrefix) {
        std::string namePrefix = std::string(namePrefixNode->getIdentifierString());

        bool foundCandidate = false;
        bool isModule = false;
        std::filesystem::path currentPath;
        std::string candidateName;
        for (auto& candidate : currentCandidatePaths) {
            candidateName = candidate.filename().string();

            if (candidateName == namePrefix) {
                foundCandidate = true;
                currentPath = candidate;
                break;
            }
            if (candidateName == namePrefix + ".vni") {
                foundCandidate = true;
                isModule = true;
                currentPath = candidate;
                break;
            }
        }
        currentPath /= candidateName;

        if (!foundCandidate) {
            throw VnlcPackageReaderError(fmt::format("Could not find package or module with name: {}", namePrefix), namePrefixNode.get());
        }

        if (isModule) {
            if (!currentPackage) {
                throw VnlcPackageReaderError(fmt::format("Module {} cannot be imported at the root level", namePrefix), namePrefixNode.get());
            }

            VnlcModuleInterfaceFileReader moduleReader(currentPath, importItem);
            currentPackage->addModule(std::move(moduleReader.read()));

            return;
        }

        if (namePrefix == "*") {
            throw VnlcPackageReaderError("Wildcard imports can only be used for modules", namePrefixNode.get());
        }

        if (!currentPackage) {
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
            currentPackage->addSubPackage(
                std::make_unique<VnlcImportedPackage>(
                    namePrefix,
                    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>(),
                    std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>()
                )
            );
            currentPackage = currentPackage->getSubPackages().at(std::string(namePrefix)).get();
        }

        currentCandidatePaths.erase(currentCandidatePaths.begin(), currentCandidatePaths.end());

        for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
            if (entry.is_directory() || (entry.is_regular_file() && entry.path().extension() == ".vni")) {
                currentCandidatePaths.push_back(entry.path());
            }
        }
    }

    for (const auto& suffix : importItem.nameSuffices) {
        readRecursively(*suffix, currentCandidatePaths, currentPackage);
    }
}