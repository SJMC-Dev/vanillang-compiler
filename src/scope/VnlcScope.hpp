#ifndef VNLC_SCOPE_HPP
#define VNLC_SCOPE_HPP

#include "ast/VnlcAstNode.hpp"
#include "scope/VnlcScopeKind.hpp"
#include "symbol/VnlcSymbol.hpp"
#include <string>
#include <unordered_map>

class VnlcScope {

private:
    VnlcScopeKind kind;
    const VnlcScope* parent;
    const VnlcAstNode* localNode;
    std::unordered_map<std::string, VnlcSymbol> symbols;

public:
    explicit VnlcScope(VnlcScopeKind kind, const VnlcScope* parent = nullptr, const VnlcAstNode* localNode = nullptr) noexcept;

    bool declare(VnlcSymbol&& symbol);
    [[nodiscard]] VnlcScopeKind getKind() const noexcept;
    [[nodiscard]] const VnlcSymbol* lookupLocal(std::string_view name) const;
    [[nodiscard]] const VnlcSymbol* lookup(std::string_view name) const;
    [[nodiscard]] const VnlcScope* findParent() const noexcept;
    [[nodiscard]] const VnlcAstNode* getLocalNode() const noexcept;
};

#endif // VNLC_SCOPE_HPP