#include "SemanticAnalysisResult.hpp"
#include "scope/Scope.hpp"
#include "type/SemanticType.hpp"

namespace vnlc {
    SemanticAnalysisResult::SemanticAnalysisResult(
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
    )
        : errors(std::move(errors)),
          warnings(std::move(warnings)),
          notes(std::move(notes)),
          customizedTypes(std::move(customizedTypes)),
          localScopeMap(std::move(localScopeMap)),
          importedScopeMap(std::move(importedScopeMap)),
          semanticTypeMap(std::move(semanticTypeMap)),
          inferredValueTypeMap(std::move(inferredValueTypeMap)),
          inferredFunctionReturnTypeMap(std::move(inferredFunctionReturnTypeMap)),
          inferredExpressionTypeMap(std::move(inferredExpressionTypeMap)),
          importedPackages(std::move(importedPackages)) {}

    bool SemanticAnalysisResult::hasErrors() const {
        return !errors.empty();
    }

    bool SemanticAnalysisResult::hasWarnings() const {
        return !warnings.empty();
    }

    bool SemanticAnalysisResult::hasNotes() const {
        return !notes.empty();
    }

    const std::vector<Diagnostic>& SemanticAnalysisResult::getErrors() const {
        return errors;
    }

    const std::vector<Diagnostic>& SemanticAnalysisResult::getWarnings() const {
        return warnings;
    }

    const std::vector<Diagnostic>& SemanticAnalysisResult::getNotes() const {
        return notes;
    }

    const CustomizedType* SemanticAnalysisResult::getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const {
        for (const auto& customizedType : customizedTypes) {
            if (customizedType->getFullTypeName() == fullTypeName) {
                return customizedType.get();
            }
        }
        return nullptr;
    }

    const Scope* SemanticAnalysisResult::getScopeByAstNode(const AstNode& node) const {
        auto it = localScopeMap.find(&node);
        if (it != localScopeMap.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Scope* SemanticAnalysisResult::getScopeByImportedNode(const ImportedItem& node) const {
        auto it = importedScopeMap.find(&node);
        return it == importedScopeMap.end() ? nullptr : it->second.get();
    }

    const SemanticType* SemanticAnalysisResult::getSemanticTypeByTypeNode(const TypeNode* typeNode) const {
        auto it = semanticTypeMap.find(typeNode);
        if (it != semanticTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const ImportedPackage* SemanticAnalysisResult::getImportedPackageByName(std::string_view packageName) const {
        auto it = importedPackages.find(std::string(packageName));
        if (it != importedPackages.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const SemanticType* SemanticAnalysisResult::getInferredValueType(const ValueDeclarationNode* valueDeclaration) const {
        auto it = inferredValueTypeMap.find(valueDeclaration);
        if (it != inferredValueTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const SemanticType* SemanticAnalysisResult::getInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration) const {
        auto it = inferredFunctionReturnTypeMap.find(functionDeclaration);
        if (it != inferredFunctionReturnTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const SemanticType* SemanticAnalysisResult::getInferredExpressionType(const ExpressionNode* expressionNode) const {
        auto it = inferredExpressionTypeMap.find(expressionNode);
        if (it != inferredExpressionTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }
} // namespace vnlc
