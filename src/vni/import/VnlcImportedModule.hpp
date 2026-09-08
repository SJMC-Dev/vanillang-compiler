#ifndef VNLC_IMPORTED_MODULE_HPP
#define VNLC_IMPORTED_MODULE_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"
#include "vni/import/VnlcImportedItem.hpp"
#include <memory>
#include <unordered_map>

class VnlcImportedModule : public VnlcImportedItem {
private:
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>> identifiers;

public:
    VnlcImportedModule(std::string_view name, std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>&& identifiers);

    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>& getIdentifiers() const;
    [[nodiscard]] std::optional<const VnlcImportedIdentifier*> getIdentifierByName(std::string_view name) const;

    void addIdentifier(std::unique_ptr<VnlcImportedIdentifier>&& identifier);
};

#endif // VNLC_IMPORTED_MODULE_HPP