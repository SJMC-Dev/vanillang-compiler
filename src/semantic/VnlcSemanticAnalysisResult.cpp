#include "VnlcSemanticAnalysisResult.hpp"
#include "scope/VnlcScope.hpp"
#include "type/VnlcSemanticType.hpp"

VnlcSemanticAnalysisResult::VnlcSemanticAnalysisResult(
    std::vector<VnlcDiagnostic>&& errors,
    std::vector<VnlcDiagnostic>&& warnings,
    std::vector<VnlcDiagnostic>&& notes,
    std::unordered_set<std::unique_ptr<VnlcCustomizedType>>&& customizedTypes,
    std::unordered_map<const VnlcAstNode*, std::unique_ptr<VnlcScope>>&& scopeMap,
    std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*>&& semanticTypeMap,
    std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*>&& inferredValueTypeMap,
    std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*>&& inferredFunctionReturnTypeMap,
    std::unordered_map<const VnlcExpressionNode*, const VnlcSemanticType*>&& inferredExpressionTypeMap,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& importedPackages
)
    : errors(std::move(errors)),
      warnings(std::move(warnings)),
      notes(std::move(notes)),
      customizedTypes(std::move(customizedTypes)),
      scopeMap(std::move(scopeMap)),
      semanticTypeMap(std::move(semanticTypeMap)),
      inferredValueTypeMap(std::move(inferredValueTypeMap)),
      inferredFunctionReturnTypeMap(std::move(inferredFunctionReturnTypeMap)),
      inferredExpressionTypeMap(std::move(inferredExpressionTypeMap)),
      importedPackages(std::move(importedPackages)) {}

bool VnlcSemanticAnalysisResult::hasErrors() const {
    return !errors.empty();
}

bool VnlcSemanticAnalysisResult::hasWarnings() const {
    return !warnings.empty();
}

bool VnlcSemanticAnalysisResult::hasNotes() const {
    return !notes.empty();
}

const std::vector<VnlcDiagnostic>& VnlcSemanticAnalysisResult::getErrors() const {
    return errors;
}

const std::vector<VnlcDiagnostic>& VnlcSemanticAnalysisResult::getWarnings() const {
    return warnings;
}

const std::vector<VnlcDiagnostic>& VnlcSemanticAnalysisResult::getNotes() const {
    return notes;
}

const VnlcCustomizedType* VnlcSemanticAnalysisResult::getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const {
    for (const auto& customizedType : customizedTypes) {
        if (customizedType->getFullTypeName() == fullTypeName) {
            return customizedType.get();
        }
    }
    return nullptr;
}

const VnlcScope* VnlcSemanticAnalysisResult::getScopeByAstNode(const VnlcAstNode& node) const {
    auto it = scopeMap.find(&node);
    if (it != scopeMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

const VnlcSemanticType* VnlcSemanticAnalysisResult::getSemanticTypeByTypeNode(const VnlcTypeNode* typeNode) const {
    auto it = semanticTypeMap.find(typeNode);
    if (it != semanticTypeMap.end()) {
        return it->second;
    }
    return nullptr;
}

const VnlcImportedPackage* VnlcSemanticAnalysisResult::getImportedPackageByName(std::string_view packageName) const {
    auto it = importedPackages.find(std::string(packageName));
    if (it != importedPackages.end()) {
        return it->second.get();
    }
    return nullptr;
}

const VnlcSemanticType* VnlcSemanticAnalysisResult::getInferredValueType(const VnlcValueDeclarationNode* valueDeclaration) const {
    auto it = inferredValueTypeMap.find(valueDeclaration);
    if (it != inferredValueTypeMap.end()) {
        return it->second;
    }
    return nullptr;
}

const VnlcSemanticType* VnlcSemanticAnalysisResult::getInferredFunctionReturnType(const VnlcFunctionDeclarationNode* functionDeclaration) const {
    auto it = inferredFunctionReturnTypeMap.find(functionDeclaration);
    if (it != inferredFunctionReturnTypeMap.end()) {
        return it->second;
    }
    return nullptr;
}

const VnlcSemanticType* VnlcSemanticAnalysisResult::getInferredExpressionType(const VnlcExpressionNode* expressionNode) const {
    auto it = inferredExpressionTypeMap.find(expressionNode);
    if (it != inferredExpressionTypeMap.end()) {
        return it->second;
    }
    return nullptr;
}
