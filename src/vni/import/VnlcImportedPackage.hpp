#ifndef VNLC_IMPORTED_PACKAGE_HPP
#define VNLC_IMPORTED_PACKAGE_HPP

#include "vni/import/VnlcImportedItem.hpp"
#include "vni/import/VnlcImportedModule.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class VnlcImportedPackage : public VnlcImportedItem {
private:
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> subPackages;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>> modules;

public:
    VnlcImportedPackage(
        std::string_view name,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& subPackages,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>&& modules
    );

    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>& getSubPackages() const;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>>& getModules() const;

    [[nodiscard]] std::optional<const VnlcImportedPackage*> getSubPackageByName(std::string_view name) const;
    [[nodiscard]] std::optional<const VnlcImportedModule*> getModuleByName(std::string_view name) const;

    void addSubPackage(std::unique_ptr<VnlcImportedPackage>&& subPackage);
    void addModule(std::unique_ptr<VnlcImportedModule>&& module);
};

#endif // VNLC_IMPORTED_PACKAGE_HPP