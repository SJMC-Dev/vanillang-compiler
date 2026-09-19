#include "Scope.hpp"

namespace vnlc {
    Scope::Scope(ScopeKind kind, const Scope* parent, const AstNode* localNode) noexcept
        : kind(kind),
          origin(ScopeOrigin::LOCAL),
          parent(parent),
          localNode(localNode),
          importedNode(nullptr) {}

    Scope::Scope(ScopeKind kind, const Scope* parent, const ImportedItem* importedNode) noexcept
        : kind(kind),
          origin(ScopeOrigin::IMPORTED),
          parent(parent),
          localNode(nullptr),
          importedNode(importedNode) {}

    bool Scope::declare(Symbol&& symbol) {
        auto existingSymbolIterator = symbols.find(std::string(symbol.getName()));

        if (existingSymbolIterator != symbols.end()) {
            return false;
        } else {
            symbols.emplace(symbol.getName(), std::move(symbol));
            return true;
        }
    }

    const Symbol* Scope::lookupLocal(std::string_view name) const {
        auto it = symbols.find(std::string(name));
        if (it != symbols.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    const Symbol* Scope::lookup(std::string_view name) const {
        const Scope* current = this;
        while (current != nullptr) {
            auto it = current->symbols.find(std::string(name));
            if (it != current->symbols.end()) {
                return &(it->second);
            }
            current = current->parent;
        }
        return nullptr;
    }

    ScopeKind Scope::getKind() const noexcept {
        return kind;
    }

    ScopeOrigin Scope::getOrigin() const noexcept {
        return origin;
    }

    const Scope* Scope::findParent() const noexcept {
        return parent;
    }

    const AstNode* Scope::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedItem* Scope::getImportedNode() const noexcept {
        return importedNode;
    }
} // namespace vnlc
