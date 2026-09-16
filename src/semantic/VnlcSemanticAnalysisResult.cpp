#include "VnlcSemanticAnalysisResult.hpp"
#include "type/VnlcSemanticType.hpp"
#include <optional>

VnlcSemanticAnalysisResult::VnlcSemanticAnalysisResult(
    std::vector<VnlcDiagnostic>&& errors,
    std::vector<VnlcDiagnostic>&& warnings,
    std::vector<VnlcDiagnostic>&& notes,
    std::unordered_set<std::unique_ptr<VnlcCustomizedType>>&& customizedTypes,
    std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*>&& semanticTypeMap,
    std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*>&& inferredValueTypeMap,
    std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*>&& inferredFunctionReturnTypeMap,
    std::unordered_map<const VnlcExpressionNode*, const VnlcSemanticType*>&& inferredExpressionTypeMap,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& importedPackages,
    std::unordered_map<std::string, const VnlcImportedItem*>&& importedBindings
)
    : errors(std::move(errors)),
      warnings(std::move(warnings)),
      notes(std::move(notes)),
      customizedTypes(std::move(customizedTypes)),
      semanticTypeMap(std::move(semanticTypeMap)),
      inferredValueTypeMap(std::move(inferredValueTypeMap)),
      inferredFunctionReturnTypeMap(std::move(inferredFunctionReturnTypeMap)),
      inferredExpressionTypeMap(std::move(inferredExpressionTypeMap)),
      importedPackages(std::move(importedPackages)),
      importedBindings(std::move(importedBindings)) {}

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

const std::optional<const VnlcCustomizedType*> VnlcSemanticAnalysisResult::getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const {
    for (const auto& customizedType : customizedTypes) {
        if (customizedType->getFullTypeName() == fullTypeName) {
            return std::make_optional<const VnlcCustomizedType*>(customizedType.get());
        }
    }
    return std::nullopt;
}

const std::optional<const VnlcSemanticType*> VnlcSemanticAnalysisResult::getSemanticTypeByTypeNode(const VnlcTypeNode* typeNode) const {
    auto it = semanticTypeMap.find(typeNode);
    if (it != semanticTypeMap.end()) {
        return std::make_optional<const VnlcSemanticType*>(it->second);
    }
    return std::nullopt;
}

const std::optional<const VnlcImportedPackage*> VnlcSemanticAnalysisResult::getImportedPackageByName(std::string_view packageName) const {
    auto it = importedPackages.find(std::string(packageName));
    if (it != importedPackages.end()) {
        return std::make_optional<const VnlcImportedPackage*>(it->second.get());
    }
    return std::nullopt;
}

std::optional<const VnlcImportedItem*> VnlcSemanticAnalysisResult::getImportedBindingByName(std::string_view name) const {
    auto it = importedBindings.find(std::string(name));
    if (it != importedBindings.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::optional<const VnlcSemanticType*> VnlcSemanticAnalysisResult::getInferredValueType(const VnlcValueDeclarationNode* valueDeclaration) const {
    auto it = inferredValueTypeMap.find(valueDeclaration);
    if (it != inferredValueTypeMap.end()) {
        return std::make_optional<const VnlcSemanticType*>(it->second);
    }
    return std::nullopt;
}

const std::optional<const VnlcSemanticType*> VnlcSemanticAnalysisResult::getInferredFunctionReturnType(const VnlcFunctionDeclarationNode* functionDeclaration) const {
    auto it = inferredFunctionReturnTypeMap.find(functionDeclaration);
    if (it != inferredFunctionReturnTypeMap.end()) {
        return std::make_optional<const VnlcSemanticType*>(it->second);
    }
    return std::nullopt;
}

const std::optional<const VnlcSemanticType*> VnlcSemanticAnalysisResult::getInferredExpressionType(const VnlcExpressionNode* expressionNode) const {
    auto it = inferredExpressionTypeMap.find(expressionNode);
    if (it != inferredExpressionTypeMap.end()) {
        return std::make_optional<const VnlcSemanticType*>(it->second);
    }
    return std::nullopt;
}
