#ifndef VNLC_SCOPE_HPP
#define VNLC_SCOPE_HPP

#include "ast/AstNode.hpp"
#include "scope/ScopeKind.hpp"
#include "scope/ScopeOrigin.hpp"
#include "symbol/Symbol.hpp"
#include "vni/import/ImportedItem.hpp"
#include <string>
#include <unordered_map>

namespace vnlc {
    class Scope {

    private:
        ScopeKind kind;
        ScopeOrigin origin;
        const Scope* parent;
        const AstNode* localNode;
        const ImportedItem* importedNode;
        std::unordered_map<std::string, Symbol> symbols;

    public:
        Scope(ScopeKind kind, const Scope* parent, const AstNode* localNode) noexcept;
        Scope(ScopeKind kind, const Scope* parent, const ImportedItem* importedNode) noexcept;

        bool declare(Symbol&& symbol);
        [[nodiscard]] ScopeKind getKind() const noexcept;
        [[nodiscard]] ScopeOrigin getOrigin() const noexcept;
        [[nodiscard]] const Symbol* lookupLocal(std::string_view name) const;
        [[nodiscard]] const Symbol* lookup(std::string_view name) const;
        [[nodiscard]] const Scope* findParent() const noexcept;
        [[nodiscard]] const AstNode* getLocalNode() const noexcept;
        [[nodiscard]] const ImportedItem* getImportedNode() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SCOPE_HPP
