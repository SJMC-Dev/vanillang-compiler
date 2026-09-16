#include "VnlcImportedPackage.hpp"

VnlcImportedPackage::VnlcImportedPackage(
    std::string_view name,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& subPackages,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>&& modules
)
    : VnlcImportedItem(name),
      subPackages(std::move(subPackages)),
      modules(std::move(modules)) {}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& VnlcImportedPackage::getSubPackages() const {
    return subPackages;
}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>& VnlcImportedPackage::getModules() const {
    return modules;
}

std::optional<const VnlcImportedPackage*> VnlcImportedPackage::getSubPackageByName(std::string_view name) const {
    auto it = subPackages.find(std::string(name));
    if (it != subPackages.end()) {
        return std::make_optional<const VnlcImportedPackage*>(it->second.get());
    }
    return std::nullopt;
}

std::optional<const VnlcImportedModule*> VnlcImportedPackage::getModuleByName(std::string_view name) const {
    auto it = modules.find(std::string(name));
    if (it != modules.end()) {
        return std::make_optional<const VnlcImportedModule*>(it->second.get());
    }
    return std::nullopt;
}

void VnlcImportedPackage::addSubPackage(std::unique_ptr<VnlcImportedPackage>&& subPackage) {
    if (subPackages.find(std::string(subPackage->getName())) == subPackages.end()) {
        subPackages.emplace(subPackage->getName(), std::move(subPackage));
    }
}

void VnlcImportedPackage::addModule(std::unique_ptr<VnlcImportedModule>&& module) {
    if (modules.find(std::string(module->getName())) == modules.end()) {
        modules.emplace(module->getName(), std::move(module));
    }
}

void VnlcImportedPackage::merge(VnlcImportedPackage&& package) {
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
