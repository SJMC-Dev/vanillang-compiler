#ifndef VNLC_IMPORTED_MODULE_HPP
#define VNLC_IMPORTED_MODULE_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedItem.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class ImportedModule : public ImportedItem {
    private:
        std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>> identifiers;

    public:
        ImportedModule(std::string_view name, std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>&& identifiers);

        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>& getIdentifiers() const;
        [[nodiscard]] const ImportedIdentifier* getIdentifierByName(std::string_view name) const;

        void addIdentifier(std::unique_ptr<ImportedIdentifier>&& identifier);
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_MODULE_HPP