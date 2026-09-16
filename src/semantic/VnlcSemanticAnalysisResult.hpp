#ifndef VNLC_SEMANTIC_ANALYSIS_RESULT_HPP
#define VNLC_SEMANTIC_ANALYSIS_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include "diagnostic/VnlcDiagnostic.hpp"
#include "type/VnlcCustomizedType.hpp"
#include "type/VnlcSemanticType.hpp"
#include "vni/import/VnlcImportedPackage.hpp"
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class VnlcSemanticAnalysisResult {
private:
    std::vector<VnlcDiagnostic> errors;
    std::vector<VnlcDiagnostic> warnings;
    std::vector<VnlcDiagnostic> notes;

    std::unordered_set<std::unique_ptr<VnlcCustomizedType>> customizedTypes;
    std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*> semanticTypeMap;
    std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*> inferredValueTypeMap;
    std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*> inferredFunctionReturnTypeMap;
    std::unordered_map<const VnlcExpressionNode*, const VnlcSemanticType*> inferredExpressionTypeMap;

    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> importedPackages;

public:
    VnlcSemanticAnalysisResult(
        std::vector<VnlcDiagnostic>&& errors,
        std::vector<VnlcDiagnostic>&& warnings,
        std::vector<VnlcDiagnostic>&& notes,
        std::unordered_set<std::unique_ptr<VnlcCustomizedType>>&& customizedTypes,
        std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*>&& semanticTypeMap,
        std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*>&& inferredValueTypeMap,
        std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*>&& inferredFunctionReturnTypeMap,
        std::unordered_map<const VnlcExpressionNode*, const VnlcSemanticType*>&& inferredExpressionTypeMap,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& importedPackages
    );
    VnlcSemanticAnalysisResult(const VnlcSemanticAnalysisResult&) = default;
    VnlcSemanticAnalysisResult& operator=(const VnlcSemanticAnalysisResult&) = default;
    VnlcSemanticAnalysisResult(VnlcSemanticAnalysisResult&&) noexcept = default;
    VnlcSemanticAnalysisResult& operator=(VnlcSemanticAnalysisResult&&) noexcept = default;

    [[nodiscard]] bool hasErrors() const;
    [[nodiscard]] bool hasWarnings() const;
    [[nodiscard]] bool hasNotes() const;
    [[nodiscard]] const std::vector<VnlcDiagnostic>& getErrors() const;
    [[nodiscard]] const std::vector<VnlcDiagnostic>& getWarnings() const;
    [[nodiscard]] const std::vector<VnlcDiagnostic>& getNotes() const;

    [[nodiscard]] const std::optional<const VnlcCustomizedType*> getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const;
    [[nodiscard]] const std::optional<const VnlcSemanticType*> getSemanticTypeByTypeNode(const VnlcTypeNode* typeNode) const;
    [[nodiscard]] const std::optional<const VnlcImportedPackage*> getImportedPackageByName(std::string_view packageName) const;
    [[nodiscard]] const std::optional<const VnlcSemanticType*> getInferredValueType(const VnlcValueDeclarationNode* valueDeclaration) const;
    [[nodiscard]] const std::optional<const VnlcSemanticType*> getInferredFunctionReturnType(const VnlcFunctionDeclarationNode* functionDeclaration) const;
    [[nodiscard]] const std::optional<const VnlcSemanticType*> getInferredExpressionType(const VnlcExpressionNode* expressionNode) const;
};

#endif // VNLC_SEMANTIC_ANALYSIS_RESULT_HPP
