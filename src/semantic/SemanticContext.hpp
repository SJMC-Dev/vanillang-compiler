#ifndef VNLC_SEMANTIC_CONTEXT_HPP
#define VNLC_SEMANTIC_CONTEXT_HPP

#include "ast/AstNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "diagnostic/Diagnostic.hpp"
#include "scope/Scope.hpp"
#include "type/CustomizedType.hpp"
#include "type/Type.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class SemanticContext {
    private:
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;

        std::vector<std::unique_ptr<Scope>> scopeStack;
        std::unordered_map<const AstNode*, std::unique_ptr<Scope>> localScopeMap;
        std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> importedScopeMap;

        std::unordered_map<std::string, std::unique_ptr<CustomizedType>> customizedTypeRegistry;
        std::unordered_map<const TypeReferenceNode*, const Type*> typeMap;
        std::unordered_map<const ValueDeclarationNode*, const Type*> inferredValueTypeMap;
        std::unordered_map<const FunctionDeclarationNode*, const Type*> inferredFunctionReturnTypeMap;
        std::unordered_map<const ExpressionNode*, const Type*> inferredExpressionTypeMap;

        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> importedPackages;

        std::size_t loopDepth = 0;
        std::size_t switchDepth = 0;

    public:
        SemanticContext() = default;
        SemanticContext(const SemanticContext&) = delete;
        SemanticContext& operator=(const SemanticContext&) = delete;
        SemanticContext(SemanticContext&&) noexcept = default;
        SemanticContext& operator=(SemanticContext&&) noexcept = default;

        void reportError(const AstNode& node, std::string_view message);
        void reportWarning(const AstNode& node, std::string_view message);
        void reportNote(const AstNode& node, std::string_view message);

        void pushScope(std::unique_ptr<Scope>&& scope);
        void popScope();
        Scope& getOrCreateImportedScope(ScopeKind kind, const Scope* parent, const ImportedItem& importedNode);

        void registerCustomizedType(std::unique_ptr<CustomizedType>&& customizedType);
        void mapType(const TypeReferenceNode* typeNode, const Type* type);
        void mapInferredValueType(const ValueDeclarationNode* valueDeclaration, const Type* type);
        void mapInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration, const Type* type);
        void mapInferredExpressionType(const ExpressionNode* expressionNode, const Type* type);

        void collectImportedPackages(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& importedPackages);

        [[nodiscard]] const ImportedPackage* getImportedPackageByName(std::string_view name) const;
        [[nodiscard]] const CustomizedType* getCustomizedTypeByFullTypeName(const std::string& fullTypeName) const;
        [[nodiscard]] const Type* getTypeByTypeReferenceNode(const TypeReferenceNode* typeNode) const;
        [[nodiscard]] const Type* getInferredExpressionType(const ExpressionNode* expressionNode) const;
        [[nodiscard]] const Scope* getScopeByAstNode(const AstNode* astNode) const;
        [[nodiscard]] const Scope* getScopeByImportedNode(const ImportedItem* importedNode) const;
        [[nodiscard]] const Scope* getScopeBySymbol(const Symbol& symbol) const;

        [[nodiscard]] Scope& currentScope();

        [[nodiscard]] const Scope* currentModule();
        [[nodiscard]] const Scope* currentFunction();
        [[nodiscard]] const Scope* currentClass();
        [[nodiscard]] const Scope* currentInterface();
        [[nodiscard]] const Scope* currentEnum();
        [[nodiscard]] const Scope* currentBlock();
        [[nodiscard]] const Scope* currentLoop();
        [[nodiscard]] const Scope* currentSwitch();

        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getWarnings() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getNotes() const noexcept;

        [[nodiscard]] std::size_t getLoopDepth() const noexcept;
        [[nodiscard]] std::size_t getSwitchDepth() const noexcept;

        [[nodiscard]] std::tuple<std::vector<Diagnostic>, std::vector<Diagnostic>, std::vector<Diagnostic>> takeDiagnostics();
        [[nodiscard]] std::unordered_map<const AstNode*, std::unique_ptr<Scope>> takeLocalScopeMap();
        [[nodiscard]] std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> takeImportedScopeMap();
        [[nodiscard]] std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> takeImportedPackages();
        [[nodiscard]] std::unordered_map<std::string, std::unique_ptr<CustomizedType>> takeCustomizedTypeRegistry();
        [[nodiscard]] std::unordered_map<const TypeReferenceNode*, const Type*> takeTypeMap();
        [[nodiscard]] std::unordered_map<const ValueDeclarationNode*, const Type*> takeInferredValueTypeMap();
        [[nodiscard]] std::unordered_map<const FunctionDeclarationNode*, const Type*> takeInferredFunctionReturnTypeMap();
        [[nodiscard]] std::unordered_map<const ExpressionNode*, const Type*> takeInferredExpressionTypeMap();
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_CONTEXT_HPP
