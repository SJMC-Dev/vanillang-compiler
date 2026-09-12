#ifndef VNLC_SYMBOL_HPP
#define VNLC_SYMBOL_HPP

#include "ast/VnlcAstNode.hpp"
#include "semantic/symbol/VnlcSymbolAccessModifier.hpp"
#include "semantic/symbol/VnlcSymbolKind.hpp"
#include "semantic/symbol/VnlcSymbolOrigin.hpp"
#include <string>
#include <string_view>

class VnlcSymbol {
private:
    VnlcSymbolKind kind;
    VnlcSymbolOrigin origin;
    VnlcSymbolAccessModifier accessModifier;
    std::string name;
    const VnlcAstNode* localDeclarationNode;

public:
    VnlcSymbol(VnlcSymbolKind kind, VnlcSymbolOrigin origin, VnlcSymbolAccessModifier accessModifier, std::string_view name, const VnlcAstNode* localDeclarationNode);

    [[nodiscard]] VnlcSymbolKind getKind() const noexcept;
    [[nodiscard]] VnlcSymbolOrigin getOrigin() const noexcept;
    [[nodiscard]] VnlcSymbolAccessModifier getAccessModifier() const noexcept;
    [[nodiscard]] std::string_view getName() const noexcept;
    [[nodiscard]] const VnlcAstNode* getLocalDeclarationNode() const noexcept;
};

#endif // VNLC_SYMBOL_HPP