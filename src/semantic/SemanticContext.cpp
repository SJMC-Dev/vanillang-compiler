#include "SemanticContext.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    void SemanticContext::reportError(const AstNode& node, std::string_view message) {
        errors.emplace_back(DiagnosticPhase::SEMANTIC, DiagnosticSeverity::ERROR, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
    }

    void SemanticContext::reportWarning(const AstNode& node, std::string_view message) {
        warnings.emplace_back(DiagnosticPhase::SEMANTIC, DiagnosticSeverity::WARNING, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
    }

    void SemanticContext::reportNote(const AstNode& node, std::string_view message) {
        notes.emplace_back(DiagnosticPhase::SEMANTIC, DiagnosticSeverity::NOTE, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
    }

    void SemanticContext::pushScope(std::unique_ptr<Scope>&& scope) {
        scopeStack.push_back(std::move(scope));
    }

    void SemanticContext::popScope() {
        if (scopeStack.back()->getOrigin() == ScopeOrigin::IMPORTED) {
            importedScopeMap.emplace(scopeStack.back()->getImportedNode(), std::move(scopeStack.back()));
        } else {
            localScopeMap.emplace(scopeStack.back()->getLocalNode(), std::move(scopeStack.back()));
        }
        scopeStack.pop_back();
    }

    Scope& SemanticContext::getOrCreateImportedScope(ScopeKind kind, const Scope* parent, const ImportedItem& importedNode) {
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if ((*it)->getOrigin() == ScopeOrigin::IMPORTED && (*it)->getImportedNode() == &importedNode) {
                return **it;
            }
        }
        auto [it, inserted] = importedScopeMap.try_emplace(&importedNode);
        if (inserted) {
            it->second = std::make_unique<Scope>(kind, parent, &importedNode);
        }
        return *it->second;
    }

    void SemanticContext::registerCustomizedType(std::string_view fullName, std::unique_ptr<CustomizedType>&& customizedType) {
        customizedTypeRegistry.emplace(std::string(fullName), std::move(customizedType));
    }

    void SemanticContext::mapSemanticType(const TypeNode* typeNode, const SemanticType* semanticType) {
        semanticTypeMap.emplace(typeNode, semanticType);
    }

    void SemanticContext::mapInferredValueType(const ValueDeclarationNode* valueDeclaration, const SemanticType* semanticType) {
        inferredValueTypeMap.emplace(valueDeclaration, semanticType);
    }

    void SemanticContext::mapInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration, const SemanticType* semanticType) {
        inferredFunctionReturnTypeMap.emplace(functionDeclaration, semanticType);
    }

    void SemanticContext::mapInferredExpressionType(const ExpressionNode* expressionNode, const SemanticType* semanticType) {
        inferredExpressionTypeMap.emplace(expressionNode, semanticType);
    }

    void SemanticContext::collectImportedPackages(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& importedPackages) {
        for (auto& [name, package] : importedPackages) {
            auto existing = this->importedPackages.find(name);
            if (existing == this->importedPackages.end()) {
                this->importedPackages.emplace(name, std::move(package));
            } else {
                existing->second->merge(std::move(*package));
            }
        }
    }

    const ImportedPackage* SemanticContext::getImportedPackageByName(std::string_view name) const {
        auto it = importedPackages.find(std::string(name));
        if (it != importedPackages.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const CustomizedType* SemanticContext::getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const {
        auto it = customizedTypeRegistry.find(std::string(fullTypeName));
        if (it != customizedTypeRegistry.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const SemanticType* SemanticContext::getSemanticTypeByTypeNode(const TypeNode* typeNode) const {
        auto it = semanticTypeMap.find(typeNode);
        if (it != semanticTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const SemanticType* SemanticContext::getInferredExpressionType(const ExpressionNode* expressionNode) const {
        auto it = inferredExpressionTypeMap.find(expressionNode);
        if (it != inferredExpressionTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const Scope* SemanticContext::getScopeByAstNode(const AstNode* astNode) const {
        if (astNode == nullptr) return nullptr;
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if ((*it)->getOrigin() == ScopeOrigin::LOCAL && (*it)->getLocalNode() == astNode) {
                return it->get();
            }
        }

        auto it = localScopeMap.find(astNode);
        if (it != localScopeMap.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Scope* SemanticContext::getScopeByImportedNode(const ImportedItem* importedNode) const {
        if (importedNode == nullptr) return nullptr;
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if ((*it)->getOrigin() == ScopeOrigin::IMPORTED && (*it)->getImportedNode() == importedNode) {
                return it->get();
            }
        }
        auto it = importedScopeMap.find(importedNode);
        return it == importedScopeMap.end() ? nullptr : it->second.get();
    }

    Scope& SemanticContext::currentScope() {
        return *scopeStack.back();
    }

    const Scope* SemanticContext::currentModule() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::MODULE) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentFunction() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::FUNCTION) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentClass() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::CLASS) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentInterface() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::INTERFACE) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentEnum() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::ENUM) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentBlock() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::BLOCK) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentLoop() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::LOOP) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentSwitch() {
        const Scope* current = &currentScope();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::SWITCH) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const std::vector<Diagnostic>& SemanticContext::getErrors() const noexcept {
        return errors;
    }

    const std::vector<Diagnostic>& SemanticContext::getWarnings() const noexcept {
        return warnings;
    }

    const std::vector<Diagnostic>& SemanticContext::getNotes() const noexcept {
        return notes;
    }

    unsigned int SemanticContext::getLoopDepth() const noexcept {
        return loopDepth;
    }

    unsigned int SemanticContext::getSwitchDepth() const noexcept {
        return switchDepth;
    }

    std::tuple<std::vector<Diagnostic>, std::vector<Diagnostic>, std::vector<Diagnostic>> SemanticContext::takeDiagnostics() {
        return std::make_tuple(std::move(errors), std::move(warnings), std::move(notes));
    }

    std::unordered_map<const AstNode*, std::unique_ptr<Scope>> SemanticContext::takeLocalScopeMap() {
        return std::move(localScopeMap);
    }

    std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> SemanticContext::takeImportedScopeMap() {
        return std::move(importedScopeMap);
    }

    std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> SemanticContext::takeImportedPackages() {
        return std::move(importedPackages);
    }

    std::unordered_set<std::unique_ptr<CustomizedType>> SemanticContext::takeCustomizedTypeRegistry() {
        std::unordered_set<std::unique_ptr<CustomizedType>> customizedTypes;

        for (auto& [_, customizedType] : customizedTypeRegistry) {
            customizedTypes.insert(std::move(customizedType));
        }

        customizedTypeRegistry.clear();

        return customizedTypes;
    }

    std::unordered_map<const TypeNode*, const SemanticType*> SemanticContext::takeSemanticTypeMap() {
        return std::move(semanticTypeMap);
    }

    std::unordered_map<const ValueDeclarationNode*, const SemanticType*> SemanticContext::takeInferredValueTypeMap() {
        return std::move(inferredValueTypeMap);
    }

    std::unordered_map<const FunctionDeclarationNode*, const SemanticType*> SemanticContext::takeInferredFunctionReturnTypeMap() {
        return std::move(inferredFunctionReturnTypeMap);
    }

    std::unordered_map<const ExpressionNode*, const SemanticType*> SemanticContext::takeInferredExpressionTypeMap() {
        return std::move(inferredExpressionTypeMap);
    }
} // namespace vnlc
