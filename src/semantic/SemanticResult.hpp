#ifndef VNLC_SEMANTIC_ANALYSIS_RESULT_HPP
#define VNLC_SEMANTIC_ANALYSIS_RESULT_HPP

#include "ast/AstNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "diagnostic/Diagnostic.hpp"
#include "scope/Scope.hpp"
#include "type/CustomizedType.hpp"
#include "type/Type.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class SemanticResult {
    private:
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;

        std::unordered_map<std::string, std::unique_ptr<CustomizedType>> customizedTypes;
        std::unordered_map<const AstNode*, std::unique_ptr<Scope>> localScopeMap;
        std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> importedScopeMap;
        std::unordered_map<const TypeReferenceNode*, const Type*> typeMap;
        std::unordered_map<const ValueDeclarationNode*, const Type*> inferredValueTypeMap;
        std::unordered_map<const FunctionDeclarationNode*, const Type*> inferredFunctionReturnTypeMap;
        std::unordered_map<const ExpressionNode*, const Type*> inferredExpressionTypeMap;

    public:
        SemanticResult(
            std::vector<Diagnostic>&& errors,
            std::vector<Diagnostic>&& warnings,
            std::vector<Diagnostic>&& notes,
            std::unordered_map<std::string, std::unique_ptr<CustomizedType>>&& customizedTypes,
            std::unordered_map<const AstNode*, std::unique_ptr<Scope>>&& localScopeMap,
            std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>>&& importedScopeMap,
            std::unordered_map<const TypeReferenceNode*, const Type*>&& typeMap,
            std::unordered_map<const ValueDeclarationNode*, const Type*>&& inferredValueTypeMap,
            std::unordered_map<const FunctionDeclarationNode*, const Type*>&& inferredFunctionReturnTypeMap,
            std::unordered_map<const ExpressionNode*, const Type*>&& inferredExpressionTypeMap
        );
        SemanticResult(const SemanticResult&) = default;
        SemanticResult& operator=(const SemanticResult&) = default;
        SemanticResult(SemanticResult&&) noexcept = default;
        SemanticResult& operator=(SemanticResult&&) noexcept = default;

        [[nodiscard]] bool hasErrors() const;
        [[nodiscard]] bool hasWarnings() const;
        [[nodiscard]] bool hasNotes() const;
        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const;
        [[nodiscard]] const std::vector<Diagnostic>& getWarnings() const;
        [[nodiscard]] const std::vector<Diagnostic>& getNotes() const;

        [[nodiscard]] const CustomizedType* getCustomizedTypeByFullTypeName(const std::string& fullTypeName) const;
        [[nodiscard]] const Scope* getScopeByAstNode(const AstNode& node) const;
        [[nodiscard]] const Scope* getScopeByImportedNode(const ImportedItem& node) const;
        [[nodiscard]] const Type* getTypeByTypeReferenceNode(const TypeReferenceNode* typeNode) const;
        [[nodiscard]] const Type* getInferredValueType(const ValueDeclarationNode* valueDeclaration) const;
        [[nodiscard]] const Type* getInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration) const;
        [[nodiscard]] const Type* getInferredExpressionType(const ExpressionNode* expressionNode) const;
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_ANALYSIS_RESULT_HPP
