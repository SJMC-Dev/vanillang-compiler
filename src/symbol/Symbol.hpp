#ifndef VNLC_SYMBOL_HPP
#define VNLC_SYMBOL_HPP

#include "ast/AstNode.hpp"
#include "symbol/SymbolAccessModifier.hpp"
#include "symbol/SymbolKind.hpp"
#include "symbol/SymbolOrigin.hpp"
#include "vni/import/ImportedItem.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class Symbol {
    private:
        SymbolKind kind;
        SymbolOrigin origin;
        SymbolAccessModifier accessModifier;
        std::string name;

        const AstNode* localNode;
        const ImportedItem* importedNode;

    public:
        Symbol(SymbolKind kind, SymbolAccessModifier accessModifier, std::string_view name, const AstNode* localNode);
        Symbol(SymbolKind kind, SymbolAccessModifier accessModifier, std::string_view name, const ImportedItem* importedNode);

        [[nodiscard]] SymbolKind getKind() const noexcept;
        [[nodiscard]] SymbolOrigin getOrigin() const noexcept;
        [[nodiscard]] SymbolAccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] const AstNode* getLocalNode() const noexcept;
        [[nodiscard]] const ImportedItem* getImportedNode() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SYMBOL_HPP