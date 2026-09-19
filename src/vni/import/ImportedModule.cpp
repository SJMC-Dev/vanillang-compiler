#include "ImportedModule.hpp"

namespace vnlc {
    ImportedModule::ImportedModule(std::string_view name, std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>&& identifiers)
        : ImportedItem(name),
          identifiers(std::move(identifiers)) {}

    const std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>& ImportedModule::getIdentifiers() const {
        return identifiers;
    }

    const ImportedIdentifier* ImportedModule::getIdentifierByName(std::string_view name) const {
        auto it = identifiers.find(std::string(name));
        if (it != identifiers.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void ImportedModule::addIdentifier(std::unique_ptr<ImportedIdentifier>&& identifier) {
        if (identifiers.find(std::string(identifier->getName())) == identifiers.end()) {
            identifiers.emplace(identifier->getName(), std::move(identifier));
        }
    }
} // namespace vnlc
