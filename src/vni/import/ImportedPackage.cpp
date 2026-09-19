#include "ImportedPackage.hpp"

namespace vnlc {
    ImportedPackage::ImportedPackage(
        std::string_view name,
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& subPackages,
        std::unordered_map<std::string, std::unique_ptr<ImportedModule>>&& modules
    )
        : ImportedItem(name),
          subPackages(std::move(subPackages)),
          modules(std::move(modules)) {}

    const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& ImportedPackage::getSubPackages() const {
        return subPackages;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedModule>>& ImportedPackage::getModules() const {
        return modules;
    }

    const ImportedPackage* ImportedPackage::getSubPackageByName(std::string_view name) const {
        auto it = subPackages.find(std::string(name));
        if (it != subPackages.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const ImportedModule* ImportedPackage::getModuleByName(std::string_view name) const {
        auto it = modules.find(std::string(name));
        if (it != modules.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void ImportedPackage::addSubPackage(std::unique_ptr<ImportedPackage>&& subPackage) {
        if (subPackages.find(std::string(subPackage->getName())) == subPackages.end()) {
            subPackages.emplace(subPackage->getName(), std::move(subPackage));
        }
    }

    void ImportedPackage::addModule(std::unique_ptr<ImportedModule>&& module) {
        if (modules.find(std::string(module->getName())) == modules.end()) {
            modules.emplace(module->getName(), std::move(module));
        }
    }

    void ImportedPackage::merge(ImportedPackage&& package) {
        for (auto& [name, subPackage] : package.subPackages) {
            auto existing = subPackages.find(name);
            if (existing == subPackages.end()) {
                subPackages.emplace(name, std::move(subPackage));
            } else {
                existing->second->merge(std::move(*subPackage));
            }
        }
        for (auto& [name, module] : package.modules) {
            addModule(std::move(module));
        }
    }
} // namespace vnlc
