#ifndef VNLC_SCOPE_HPP
#define VNLC_SCOPE_HPP

#include "ast/VnlcAstNode.hpp"
#include "scope/VnlcScopeKind.hpp"
#include "scope/VnlcScopeOrigin.hpp"
#include "symbol/VnlcSymbol.hpp"
#include "vni/import/VnlcImportedItem.hpp"
#include <string>
#include <unordered_map>

class VnlcScope {

private:
    VnlcScopeKind kind;
    VnlcScopeOrigin origin;
    const VnlcScope* parent;
    const VnlcAstNode* localNode;
    const VnlcImportedItem* importedNode;
    std::unordered_map<std::string, VnlcSymbol> symbols;

public:
    VnlcScope(VnlcScopeKind kind, const VnlcScope* parent, const VnlcAstNode* localNode) noexcept;
    VnlcScope(VnlcScopeKind kind, const VnlcScope* parent, const VnlcImportedItem* importedNode) noexcept;

    bool declare(VnlcSymbol&& symbol);
    [[nodiscard]] VnlcScopeKind getKind() const noexcept;
    [[nodiscard]] VnlcScopeOrigin getOrigin() const noexcept;
    [[nodiscard]] const VnlcSymbol* lookupLocal(std::string_view name) const;
    [[nodiscard]] const VnlcSymbol* lookup(std::string_view name) const;
    [[nodiscard]] const VnlcScope* findParent() const noexcept;
    [[nodiscard]] const VnlcAstNode* getLocalNode() const noexcept;
    [[nodiscard]] const VnlcImportedItem* getImportedNode() const noexcept;
};

#endif // VNLC_SCOPE_HPP
