#ifndef VNLC_SYMBOL_HPP
#define VNLC_SYMBOL_HPP

#include "ast/VnlcAstNode.hpp"
#include "semantic/symbol/VnlcSymbolAccessModifier.hpp"
#include "semantic/symbol/VnlcSymbolKind.hpp"
#include "semantic/symbol/VnlcSymbolOrigin.hpp"
#include "vni/import/VnlcImportedItem.hpp"
#include <string>
#include <string_view>

class VnlcSymbol {
private:
    VnlcSymbolKind kind;
    VnlcSymbolOrigin origin;
    VnlcSymbolAccessModifier accessModifier;
    std::string name;

    const VnlcAstNode* localNode;
    const VnlcImportedItem* importedNode;

public:
    VnlcSymbol(VnlcSymbolKind kind, VnlcSymbolAccessModifier accessModifier, std::string_view name, const VnlcAstNode* localNode);
    VnlcSymbol(VnlcSymbolKind kind, VnlcSymbolAccessModifier accessModifier, std::string_view name, const VnlcImportedItem* importedNode);

    [[nodiscard]] VnlcSymbolKind getKind() const noexcept;
    [[nodiscard]] VnlcSymbolOrigin getOrigin() const noexcept;
    [[nodiscard]] VnlcSymbolAccessModifier getAccessModifier() const noexcept;
    [[nodiscard]] std::string_view getName() const noexcept;
    [[nodiscard]] const VnlcAstNode* getLocalNode() const noexcept;
    [[nodiscard]] const VnlcImportedItem* getImportedNode() const noexcept;
};

#endif // VNLC_SYMBOL_HPP