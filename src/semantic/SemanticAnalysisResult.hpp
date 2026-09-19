#ifndef VNLC_SEMANTIC_ANALYSIS_RESULT_HPP
#define VNLC_SEMANTIC_ANALYSIS_RESULT_HPP

#include "ast/AstNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/type/TypeNode.hpp"
#include "diagnostic/Diagnostic.hpp"
#include "scope/Scope.hpp"
#include "type/CustomizedType.hpp"
#include "type/SemanticType.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace vnlc {
    class SemanticAnalysisResult {
    private:
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;

        std::unordered_set<std::unique_ptr<CustomizedType>> customizedTypes;
        std::unordered_map<const AstNode*, std::unique_ptr<Scope>> localScopeMap;
        std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> importedScopeMap;
        std::unordered_map<const TypeNode*, const SemanticType*> semanticTypeMap;
        std::unordered_map<const ValueDeclarationNode*, const SemanticType*> inferredValueTypeMap;
        std::unordered_map<const FunctionDeclarationNode*, const SemanticType*> inferredFunctionReturnTypeMap;
        std::unordered_map<const ExpressionNode*, const SemanticType*> inferredExpressionTypeMap;

        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> importedPackages;

    public:
        SemanticAnalysisResult(
            std::vector<Diagnostic>&& errors,
            std::vector<Diagnostic>&& warnings,
            std::vector<Diagnostic>&& notes,
            std::unordered_set<std::unique_ptr<CustomizedType>>&& customizedTypes,
            std::unordered_map<const AstNode*, std::unique_ptr<Scope>>&& localScopeMap,
            std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>>&& importedScopeMap,
            std::unordered_map<const TypeNode*, const SemanticType*>&& semanticTypeMap,
            std::unordered_map<const ValueDeclarationNode*, const SemanticType*>&& inferredValueTypeMap,
            std::unordered_map<const FunctionDeclarationNode*, const SemanticType*>&& inferredFunctionReturnTypeMap,
            std::unordered_map<const ExpressionNode*, const SemanticType*>&& inferredExpressionTypeMap,
            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& importedPackages
        );
        SemanticAnalysisResult(const SemanticAnalysisResult&) = default;
        SemanticAnalysisResult& operator=(const SemanticAnalysisResult&) = default;
        SemanticAnalysisResult(SemanticAnalysisResult&&) noexcept = default;
        SemanticAnalysisResult& operator=(SemanticAnalysisResult&&) noexcept = default;

        [[nodiscard]] bool hasErrors() const;
        [[nodiscard]] bool hasWarnings() const;
        [[nodiscard]] bool hasNotes() const;
        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const;
        [[nodiscard]] const std::vector<Diagnostic>& getWarnings() const;
        [[nodiscard]] const std::vector<Diagnostic>& getNotes() const;

        [[nodiscard]] const CustomizedType* getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const;
        [[nodiscard]] const Scope* getScopeByAstNode(const AstNode& node) const;
        [[nodiscard]] const Scope* getScopeByImportedNode(const ImportedItem& node) const;
        [[nodiscard]] const SemanticType* getSemanticTypeByTypeNode(const TypeNode* typeNode) const;
        [[nodiscard]] const ImportedPackage* getImportedPackageByName(std::string_view packageName) const;
        [[nodiscard]] const SemanticType* getInferredValueType(const ValueDeclarationNode* valueDeclaration) const;
        [[nodiscard]] const SemanticType* getInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration) const;
        [[nodiscard]] const SemanticType* getInferredExpressionType(const ExpressionNode* expressionNode) const;
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_ANALYSIS_RESULT_HPP
