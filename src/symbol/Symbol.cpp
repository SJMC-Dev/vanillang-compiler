#include "Symbol.hpp"
#include "symbol/SymbolOrigin.hpp"
#include "vni/import/ImportedItem.hpp"

namespace vnlc {
    Symbol::Symbol(SymbolKind kind, SymbolAccessModifier accessModifier, std::string_view name, const AstNode* localNode)
        : kind(kind),
          origin(SymbolOrigin::LOCAL),
          accessModifier(accessModifier),
          name(std::move(name)),
          localNode(localNode),
          importedNode(nullptr) {}

    Symbol::Symbol(SymbolKind kind, SymbolAccessModifier accessModifier, std::string_view name, const ImportedItem* importedNode)
        : kind(kind),
          origin(SymbolOrigin::IMPORTED),
          accessModifier(accessModifier),
          name(std::move(name)),
          localNode(nullptr),
          importedNode(importedNode) {}

    SymbolKind Symbol::getKind() const noexcept {
        return kind;
    }

    SymbolOrigin Symbol::getOrigin() const noexcept {
        return origin;
    }

    SymbolAccessModifier Symbol::getAccessModifier() const noexcept {
        return accessModifier;
    }

    std::string_view Symbol::getName() const noexcept {
        return name;
    }

    const AstNode* Symbol::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedItem* Symbol::getImportedNode() const noexcept {
        return importedNode;
    }
} // namespace vnlc
