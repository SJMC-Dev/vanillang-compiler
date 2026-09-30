#ifndef VNLC_SEMANTIC_ANALYZER_HPP
#define VNLC_SEMANTIC_ANALYZER_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/ExportDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ImportDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "config/Config.hpp"
#include "metadata/MetadataInfo.hpp"
#include "semantic/SemanticContext.hpp"
#include "semantic/SemanticResult.hpp"
#include "symbol/RegularSymbolAccessModifier.hpp"
#include "symbol/RegularSymbolKind.hpp"
#include "type/Type.hpp"
#include "type/typeinf/TypeInferenceResult.hpp"
#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class PrimitiveType;

    class SemanticAnalyzer {
        friend class SemanticAnalyzerImportTest;

    private:
        const ModuleNode& module;
        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports;
        SemanticContext context;

        [[nodiscard]] static RegularSymbolKind getImportedSymbolKind(const ImportedItem& item);
        [[nodiscard]] static std::optional<ScopeKind> getImportedScopeKind(const ImportedItem& item);
        [[nodiscard]] static RegularSymbolAccessModifier getImportedAccessModifier(const ImportedItem& item);

        [[nodiscard]] const ImportedPackage* getImportedRootPackageByName(std::string_view name) const;

        void declareFunctionOverloading(const FunctionDeclarationNode& funcDecl, RegularSymbolKind kind, std::string_view declarationKind);
        const Type* registerImportedTypeReference(std::string_view fullTypeName);
        void registerImportedTypeReferences(const ImportedItem& importedItem);
        void registerImportedScopes(const ImportedItem& item, const Scope* parent);

        void collectLocalSymbolsInModule(const ModuleNode& moduleNode);
        void collectLocalSymbolsInFunction(const FunctionDeclarationNode& functionNode);
        void collectLocalSymbolsInConstructor(const ConstructorDeclarationNode& constructorNode);
        void collectLocalSymbolsInOperator(const OperatorDeclarationNode& operatorNode);
        void collectLocalSymbolsInClass(const ClassDeclarationNode& classNode);
        void collectLocalSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode);
        void collectLocalSymbolsInEnum(const EnumDeclarationNode& enumNode);
        void collectLocalSymbolsInEnumMember(const EnumMemberDeclarationNode& enumMemberNode);
        void collectLocalSymbolsInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode);

        void checkIdentifierExpressionUse(const IdentifierLikeExpressionNode& exprNode, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);

        [[nodiscard]] MetadataInfo checkMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms, const DeclarationNode& declNode);

        void checkModule(const ModuleNode& moduleNode, const Config& config);
        void checkImport(const ImportDeclarationNode& importDecl, const Config& config);
        void checkExport(const ExportDeclarationNode& exportDecl);
        void checkValueDeclaration(const ValueDeclarationNode& varDecl, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkConstructorDeclaration(const ConstructorDeclarationNode& constructorDecl, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkOperatorDeclaration(const OperatorDeclarationNode& operatorDecl, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkTypeAliasDeclaration(const TypeAliasDeclarationNode& typeAliasDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkStatement(const StatementNode& statement);
        void checkExpression(const ExpressionNode& expression);
        const Type* checkType(const TypeReferenceNode& type);

        [[nodiscard]] TypeInferenceResult inferExpressionType(const ExpressionNode& expression);
        [[nodiscard]] TypeInferenceResult inferReturnType(const BlockStatementNode& statement);

    public:
        explicit SemanticAnalyzer(const ModuleNode& module);
        SemanticAnalyzer(const ModuleNode& module, const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports);

        SemanticAnalyzer() = delete;
        SemanticAnalyzer(const SemanticAnalyzer&) = delete;
        SemanticAnalyzer& operator=(const SemanticAnalyzer&) = delete;

        SemanticAnalyzer(SemanticAnalyzer&&) noexcept = default;
        SemanticAnalyzer& operator=(SemanticAnalyzer&&) noexcept = delete;

        [[nodiscard]] SemanticResult analyze(const Config& config);
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_ANALYZER_HPP
