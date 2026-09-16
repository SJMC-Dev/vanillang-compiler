#include "VnlcSymbol.hpp"
#include "semantic/symbol/VnlcSymbolOrigin.hpp"
#include "vni/import/VnlcImportedItem.hpp"

VnlcSymbol::VnlcSymbol(VnlcSymbolKind kind, VnlcSymbolAccessModifier accessModifier, std::string_view name, const VnlcAstNode* localNode)
    : kind(kind),
      origin(VnlcSymbolOrigin::LOCAL),
      accessModifier(accessModifier),
      name(std::move(name)),
      localNode(localNode),
      importedNode(nullptr) {}

VnlcSymbol::VnlcSymbol(VnlcSymbolKind kind, VnlcSymbolAccessModifier accessModifier, std::string_view name, const VnlcImportedItem* importedNode)
    : kind(kind),
      origin(VnlcSymbolOrigin::IMPORTED),
      accessModifier(accessModifier),
      name(std::move(name)),
      localNode(nullptr),
      importedNode(importedNode) {}

VnlcSymbolKind VnlcSymbol::getKind() const noexcept {
    return kind;
}

VnlcSymbolOrigin VnlcSymbol::getOrigin() const noexcept {
    return origin;
}

VnlcSymbolAccessModifier VnlcSymbol::getAccessModifier() const noexcept {
    return accessModifier;
}

std::string_view VnlcSymbol::getName() const noexcept {
    return name;
}

const VnlcAstNode* VnlcSymbol::getLocalNode() const noexcept {
    return localNode;
}

const VnlcImportedItem* VnlcSymbol::getImportedNode() const noexcept {
    return importedNode;
}