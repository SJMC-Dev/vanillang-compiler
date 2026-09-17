#include "VnlcScope.hpp"

VnlcScope::VnlcScope(VnlcScopeKind kind, const VnlcScope* parent, const VnlcAstNode* localNode) noexcept
    : kind(kind),
      origin(VnlcScopeOrigin::LOCAL),
      parent(parent),
      localNode(localNode),
      importedNode(nullptr) {}

VnlcScope::VnlcScope(VnlcScopeKind kind, const VnlcScope* parent, const VnlcImportedItem* importedNode) noexcept
    : kind(kind),
      origin(VnlcScopeOrigin::IMPORTED),
      parent(parent),
      localNode(nullptr),
      importedNode(importedNode) {}

bool VnlcScope::declare(VnlcSymbol&& symbol) {
    auto existingSymbolIterator = symbols.find(std::string(symbol.getName()));

    if (existingSymbolIterator != symbols.end()) {
        return false;
    } else {
        symbols.emplace(symbol.getName(), std::move(symbol));
        return true;
    }
}

const VnlcSymbol* VnlcScope::lookupLocal(std::string_view name) const {
    auto it = symbols.find(std::string(name));
    if (it != symbols.end()) {
        return &(it->second);
    }
    return nullptr;
}

const VnlcSymbol* VnlcScope::lookup(std::string_view name) const {
    const VnlcScope* current = this;
    while (current != nullptr) {
        auto it = current->symbols.find(std::string(name));
        if (it != current->symbols.end()) {
            return &(it->second);
        }
        current = current->parent;
    }
    return nullptr;
}

VnlcScopeKind VnlcScope::getKind() const noexcept {
    return kind;
}

VnlcScopeOrigin VnlcScope::getOrigin() const noexcept {
    return origin;
}

const VnlcScope* VnlcScope::findParent() const noexcept {
    return parent;
}

const VnlcAstNode* VnlcScope::getLocalNode() const noexcept {
    return localNode;
}

const VnlcImportedItem* VnlcScope::getImportedNode() const noexcept {
    return importedNode;
}
