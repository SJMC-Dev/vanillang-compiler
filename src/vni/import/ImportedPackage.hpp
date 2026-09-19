#ifndef VNLC_IMPORTED_PACKAGE_HPP
#define VNLC_IMPORTED_PACKAGE_HPP

#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedModule.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vnlc {
    class ImportedPackage : public ImportedItem {
    private:
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> subPackages;
        std::unordered_map<std::string, std::unique_ptr<ImportedModule>> modules;

    public:
        ImportedPackage(
            std::string_view name,
            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& subPackages,
            std::unordered_map<std::string, std::unique_ptr<ImportedModule>>&& modules
        );

        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& getSubPackages() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedModule>>& getModules() const;

        [[nodiscard]] const ImportedPackage* getSubPackageByName(std::string_view name) const;
        [[nodiscard]] const ImportedModule* getModuleByName(std::string_view name) const;

        void addSubPackage(std::unique_ptr<ImportedPackage>&& subPackage);
        void addModule(std::unique_ptr<ImportedModule>&& module);
        void merge(ImportedPackage&& package);
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_PACKAGE_HPP
