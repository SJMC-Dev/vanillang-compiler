#ifndef VNLC_SEMANTIC_ANALYZER_HPP
#define VNLC_SEMANTIC_ANALYZER_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/ExportDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ImportDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/IdentifierExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceKind.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "config/Config.hpp"
#include "metadata/MetadataInfo.hpp"
#include "semantic/SemanticContext.hpp"
#include "semantic/SemanticResult.hpp"
#include "type/Type.hpp"
#include "type/typeinf/TypeInferenceResult.hpp"
#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>


namespace vnlc {
    class PrimitiveType;

    class SemanticAnalyzer {
        friend class SemanticAnalyzerAccessTest;
        friend class SemanticAnalyzerImportTest;

    private:
        const ModuleNode& module;
        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports;
        SemanticContext context;

        [[nodiscard]] static SymbolKind getImportedSymbolKind(const ImportedItem& item);
        [[nodiscard]] static std::optional<ScopeKind> getImportedScopeKind(const ImportedItem& item);
        [[nodiscard]] static SymbolAccessModifier getImportedAccessModifier(const ImportedItem& item);
        [[nodiscard]] static const PrimitiveType* getPrimitiveType(PrimitiveTypeReferenceKind kind);
        [[nodiscard]] const ImportedPackage* getImportedPackageByName(std::string_view name) const;
        [[nodiscard]] std::string getFullTypeNameByTypeReferenceNode(const TypeReferenceNode& typeNode) noexcept;
        [[nodiscard]] std::string getUnwrappedTypeNameByTypeReferenceNode(const TypeReferenceNode& typeNode) noexcept;
        void registerImportedScopes(const ImportedItem& item, const Scope* parent);
        void checkIdentifierExpressionUse(const IdentifierExpressionNode& exprNode, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        [[nodiscard]] bool checkAccessModifier(const MemberAccessExpressionNode& memberAccessNode);
        [[nodiscard]] bool checkAccessModifier(const IdentifierExpressionNode& identifierNode);
        [[nodiscard]] bool checkMemberAccessModifier(const TypeDeclarationNode* receiverTypeDeclaration, std::string_view memberName, bool isSuperAccess = false);
        [[nodiscard]] MetadataInfo checkMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms, const DeclarationNode& declNode);
        [[nodiscard]] std::size_t getGenericParameterCount(const Symbol& symbol) const;
        void checkModule(const ModuleNode& moduleNode, const Config& config);
        void checkImport(const ImportDeclarationNode& importDecl, const Config& config);
        void checkExport(const ExportDeclarationNode& exportDecl);
        void checkValueDeclaration(const ValueDeclarationNode& varDecl, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkTypeAliasDeclaration(const TypeAliasDeclarationNode& typeAliasDecl, const Config& config, MetadataInfo metadataInfo = MetadataInfo::DEFAULT);
        void checkStatement(const StatementNode& statement);
        void checkExpression(const ExpressionNode& expression);
        const Type* checkType(const TypeReferenceNode& type);
        const Type* checkUnwrappedType(const TypeReferenceNode& type);
        [[nodiscard]] TypeInferenceResult inferExpressionType(const ExpressionNode& expression);
        [[nodiscard]] TypeInferenceResult inferFunctionReturnType(const FunctionDeclarationNode& funcDecl);

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
