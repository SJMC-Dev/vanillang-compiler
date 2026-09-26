#include "ImportedItem.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedPackage.hpp"

namespace vnlc {
    ImportedItem::ImportedItem(std::string_view name) : name(name) {}

    std::string_view ImportedItem::getName() const {
        return name;
    }

    const ImportedItem* ImportedItem::getChildByName(std::string_view name) const {
        if (const auto* package = dynamic_cast<const ImportedPackage*>(this)) {
            if (const auto* subPackage = package->getSubPackageByName(name)) {
                return subPackage;
            }
            return package->getModuleByName(name);
        }
        if (const auto* module = dynamic_cast<const ImportedModule*>(this)) {
            return module->getIdentifierByName(name);
        }
        return nullptr;
    }

    const ImportedItem* ImportedItem::getImportedItemByFullPath(const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports, const std::vector<std::string>& fullPath) {
        if (fullPath.empty()) {
            return nullptr;
        }

        const auto rootPackage = imports.find(fullPath.front());
        if (rootPackage == imports.end()) {
            return nullptr;
        }

        const ImportedItem* item = rootPackage->second.get();
        for (std::size_t index = 1; index < fullPath.size(); ++index) {
            item = item->getChildByName(fullPath[index]);

            if (item == nullptr) {
                return nullptr;
            }
        }

        return item;
    }
} // namespace vnlc
