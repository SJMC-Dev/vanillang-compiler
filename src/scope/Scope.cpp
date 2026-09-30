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

    bool Scope::declare(std::unique_ptr<Symbol>&& symbol) {
        std::string name(symbol->getName());
        auto existingSymbolIterator = symbols.find(name);

        if (existingSymbolIterator != symbols.end()) {
            return false;
        } else {
            symbols.emplace(std::move(name), std::move(symbol));
            return true;
        }
    }

    bool Scope::declare(RegularSymbol&& symbol) {
        return declare(std::make_unique<RegularSymbol>(std::move(symbol)));
    }

    bool Scope::declare(FunctionSymbol&& symbol) {
        const std::string name(symbol.getName());
        auto existingSymbolIterator = symbols.find(name);
        if (existingSymbolIterator == symbols.end()) {
            return declare(std::make_unique<FunctionSymbol>(std::move(symbol)));
        }

        auto* existingFunction = dynamic_cast<FunctionSymbol*>(existingSymbolIterator->second.get());
        if (existingFunction == nullptr) {
            return false;
        }
        if (symbol.getOverloadings().empty() || existingFunction->getOverloadings().empty()) {
            return false;
        }
        if (symbol.getOverloadings().begin()->second.getOrigin() != existingFunction->getOverloadings().begin()->second.getOrigin()) {
            return false;
        }

        for (const auto& [internalName, overloading] : symbol.getOverloadings()) {
            if (existingFunction->getOverloadingByInternalName(internalName) != nullptr) {
                return false;
            }
        }

        for (const auto& overloading : symbol.getOverloadings()) {
            existingFunction->addOverloading(RegularSymbol(overloading.second));
        }
        return true;
    }

    const Symbol* Scope::lookupLocal(std::string_view name) const {
        auto it = symbols.find(std::string(name));
        if (it != symbols.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Symbol* Scope::lookup(std::string_view name) const {
        const Scope* current = this;
        while (current != nullptr) {
            auto it = current->symbols.find(std::string(name));
            if (it != current->symbols.end()) {
                return it->second.get();
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
