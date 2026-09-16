#include "VnlcImportedModule.hpp"

VnlcImportedModule::VnlcImportedModule(std::string_view name, std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>&& identifiers)
    : VnlcImportedItem(name),
      identifiers(std::move(identifiers)) {}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>& VnlcImportedModule::getIdentifiers() const {
    return identifiers;
}

const VnlcImportedIdentifier* VnlcImportedModule::getIdentifierByName(std::string_view name) const {
    auto it = identifiers.find(std::string(name));
    if (it != identifiers.end()) {
        return it->second.get();
    }
    return nullptr;
}

void VnlcImportedModule::addIdentifier(std::unique_ptr<VnlcImportedIdentifier>&& identifier) {
    if (identifiers.find(std::string(identifier->getName())) == identifiers.end()) {
        identifiers.emplace(identifier->getName(), std::move(identifier));
    }
}