#include "RegularSymbol.hpp"

namespace vnlc {
    RegularSymbol::RegularSymbol(RegularSymbolKind kind, RegularSymbolAccessModifier accessModifier, std::string_view name, const AstNode* localNode)
        : kind(kind),
          origin(RegularSymbolOrigin::LOCAL),
          accessModifier(accessModifier),
          name(name),
          localNode(localNode),
          importedNode(nullptr) {}

    RegularSymbol::RegularSymbol(RegularSymbolKind kind, RegularSymbolAccessModifier accessModifier, std::string_view name, const ImportedItem* importedNode)
        : kind(kind),
          origin(RegularSymbolOrigin::IMPORTED),
          accessModifier(accessModifier),
          name(name),
          localNode(nullptr),
          importedNode(importedNode) {}

    RegularSymbolKind RegularSymbol::getKind() const noexcept {
        return kind;
    }

    RegularSymbolOrigin RegularSymbol::getOrigin() const noexcept {
        return origin;
    }

    RegularSymbolAccessModifier RegularSymbol::getAccessModifier() const noexcept {
        return accessModifier;
    }

    std::string_view RegularSymbol::getName() const noexcept {
        return name;
    }

    const AstNode* RegularSymbol::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedItem* RegularSymbol::getImportedNode() const noexcept {
        return importedNode;
    }
} // namespace vnlc
