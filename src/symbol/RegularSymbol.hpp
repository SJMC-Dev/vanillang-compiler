#ifndef VNLC_REGULAR_SYMBOL_HPP
#define VNLC_REGULAR_SYMBOL_HPP

#include "ast/AstNode.hpp"
#include "symbol/RegularSymbolAccessModifier.hpp"
#include "symbol/RegularSymbolKind.hpp"
#include "symbol/RegularSymbolOrigin.hpp"
#include "symbol/Symbol.hpp"
#include "vni/import/ImportedItem.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class RegularSymbol : public Symbol {
    private:
        RegularSymbolKind kind;
        RegularSymbolOrigin origin;
        RegularSymbolAccessModifier accessModifier;
        std::string name;

        const AstNode* localNode;
        const ImportedItem* importedNode;

    public:
        RegularSymbol(RegularSymbolKind kind, RegularSymbolAccessModifier accessModifier, std::string_view name, const AstNode* localNode);
        RegularSymbol(RegularSymbolKind kind, RegularSymbolAccessModifier accessModifier, std::string_view name, const ImportedItem* importedNode);

        [[nodiscard]] RegularSymbolKind getKind() const noexcept;
        [[nodiscard]] RegularSymbolOrigin getOrigin() const noexcept;
        [[nodiscard]] RegularSymbolAccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] std::string_view getName() const noexcept override;
        [[nodiscard]] const AstNode* getLocalNode() const noexcept;
        [[nodiscard]] const ImportedItem* getImportedNode() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_REGULAR_SYMBOL_HPP
