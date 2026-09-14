#include "VnlcSemanticContext.hpp"
#include <memory>
#include <unordered_map>

void VnlcSemanticContext::reportError(const VnlcAstNode& node, std::string_view message) {
    errors.emplace_back(VnlcDiagnosticPhase::SEMANTIC, VnlcDiagnosticSeverity::ERROR, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
}

void VnlcSemanticContext::reportWarning(const VnlcAstNode& node, std::string_view message) {
    warnings.emplace_back(VnlcDiagnosticPhase::SEMANTIC, VnlcDiagnosticSeverity::WARNING, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
}

void VnlcSemanticContext::reportNote(const VnlcAstNode& node, std::string_view message) {
    notes.emplace_back(VnlcDiagnosticPhase::SEMANTIC, VnlcDiagnosticSeverity::NOTE, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
}

void VnlcSemanticContext::pushScope(std::unique_ptr<VnlcScope>&& scope) {
    scopeStack.push_back(std::move(scope));
}

void VnlcSemanticContext::popScope() {
    scopeMap.emplace(scopeStack.back()->getLocalDeclarationNode(), std::move(scopeStack.back()));
    scopeStack.pop_back();
}

void VnlcSemanticContext::registerCustomizedType(std::string_view fullName, std::unique_ptr<VnlcCustomizedType>&& customizedType) {
    customizedTypeRegistry.emplace(std::string(fullName), std::move(customizedType));
}

void VnlcSemanticContext::mapSemanticType(const VnlcTypeNode* typeNode, const VnlcSemanticType* semanticType) {
    semanticTypeMap.emplace(typeNode, semanticType);
}

void VnlcSemanticContext::mapInferredValueType(const VnlcValueDeclarationNode* valueDeclaration, const VnlcSemanticType* semanticType) {
    inferredValueTypeMap.emplace(valueDeclaration, semanticType);
}

void VnlcSemanticContext::mapInferredFunctionReturnType(const VnlcFunctionDeclarationNode* functionDeclaration, const VnlcSemanticType* semanticType) {
    inferredFunctionReturnTypeMap.emplace(functionDeclaration, semanticType);
}

void VnlcSemanticContext::mapInferredExpressionType(const VnlcExpressionNode* expressionNode, const VnlcSemanticType* semanticType) {
    inferredExpressionTypeMap.emplace(expressionNode, semanticType);
}

void VnlcSemanticContext::collectImportedPackages(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& importedPackages) {
    this->importedPackages = std::move(importedPackages);
}

const std::optional<const VnlcCustomizedType*> VnlcSemanticContext::getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const {
    auto it = customizedTypeRegistry.find(std::string(fullTypeName));
    if (it != customizedTypeRegistry.end()) {
        return std::make_optional<const VnlcCustomizedType*>(it->second.get());
    }
    return std::nullopt;
}

const std::optional<const VnlcSemanticType*> VnlcSemanticContext::getSemanticTypeByTypeNode(const VnlcTypeNode* typeNode) const {
    auto it = semanticTypeMap.find(typeNode);
    if (it != semanticTypeMap.end()) {
        return std::make_optional<const VnlcSemanticType*>(it->second);
    }
    return std::nullopt;
}

const std::optional<const VnlcSemanticType*> VnlcSemanticContext::getInferredExpressionType(const VnlcExpressionNode* expressionNode) const {
    auto it = inferredExpressionTypeMap.find(expressionNode);
    if (it != inferredExpressionTypeMap.end()) {
        return std::make_optional<const VnlcSemanticType*>(it->second);
    }
    return std::nullopt;
}

const VnlcScope* VnlcSemanticContext::getScopeByAstNode(const VnlcAstNode* astNode) const {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        if ((*it)->getLocalDeclarationNode() == astNode) {
            return it->get();
        }
    }

    auto it = scopeMap.find(astNode);
    if (it != scopeMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

VnlcScope& VnlcSemanticContext::currentScope() {
    return *scopeStack.back();
}

const VnlcScope* VnlcSemanticContext::currentModule() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::MODULE) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentFunction() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::FUNCTION) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentClass() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::CLASS) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentInterface() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::INTERFACE) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentEnum() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::ENUM) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentBlock() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::BLOCK) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentLoop() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::LOOP) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const VnlcScope* VnlcSemanticContext::currentSwitch() {
    const VnlcScope* current = &currentScope();
    while (current != nullptr) {
        if (current->getKind() == VnlcScopeKind::SWITCH) {
            return current;
        }
        current = current->findParent();
    }

    return nullptr;
}

const std::vector<VnlcDiagnostic>& VnlcSemanticContext::getErrors() const noexcept {
    return errors;
}

const std::vector<VnlcDiagnostic>& VnlcSemanticContext::getWarnings() const noexcept {
    return warnings;
}

const std::vector<VnlcDiagnostic>& VnlcSemanticContext::getNotes() const noexcept {
    return notes;
}

unsigned int VnlcSemanticContext::getLoopDepth() const noexcept {
    return loopDepth;
}

unsigned int VnlcSemanticContext::getSwitchDepth() const noexcept {
    return switchDepth;
}

std::tuple<std::vector<VnlcDiagnostic>, std::vector<VnlcDiagnostic>, std::vector<VnlcDiagnostic>> VnlcSemanticContext::takeDiagnostics() {
    return std::make_tuple(std::move(errors), std::move(warnings), std::move(notes));
}

std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> VnlcSemanticContext::takeImportedPackages() {
    return std::move(importedPackages);
}

std::unordered_set<std::unique_ptr<VnlcCustomizedType>> VnlcSemanticContext::takeCustomizedTypeRegistry() {
    std::unordered_set<std::unique_ptr<VnlcCustomizedType>> customizedTypes;

    for (auto& [_, customizedType] : customizedTypeRegistry) {
        customizedTypes.insert(std::move(customizedType));
    }

    customizedTypeRegistry.clear();

    return customizedTypes;
}

std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*> VnlcSemanticContext::takeSemanticTypeMap() {
    return std::move(semanticTypeMap);
}

std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*> VnlcSemanticContext::takeInferredValueTypeMap() {
    return std::move(inferredValueTypeMap);
}

std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*> VnlcSemanticContext::takeInferredFunctionReturnTypeMap() {
    return std::move(inferredFunctionReturnTypeMap);
}

std::unordered_map<const VnlcExpressionNode*, const VnlcSemanticType*> VnlcSemanticContext::takeInferredExpressionTypeMap() {
    return std::move(inferredExpressionTypeMap);
}
