#ifndef VNLC_SEMANTIC_CONTEXT_HPP
#define VNLC_SEMANTIC_CONTEXT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include "diagnostic/VnlcDiagnostic.hpp"
#include "semantic/scope/VnlcScope.hpp"
#include "type/VnlcCustomizedType.hpp"
#include "type/VnlcSemanticType.hpp"
#include "vni/import/VnlcImportedPackage.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class VnlcSemanticContext {
private:
    std::vector<VnlcDiagnostic> errors;
    std::vector<VnlcDiagnostic> warnings;
    std::vector<VnlcDiagnostic> notes;

    std::vector<std::unique_ptr<VnlcScope>> scopeStack;

    std::unordered_map<std::string, std::unique_ptr<VnlcCustomizedType>> customizedTypeRegistry;
    std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*> semanticTypeMap;
    std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*> inferredValueTypeMap;
    std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*> inferredFunctionReturnTypeMap;

    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> importedPackages;

    unsigned int loopDepth = 0;
    unsigned int switchDepth = 0;

public:
    VnlcSemanticContext() = default;
    VnlcSemanticContext(const VnlcSemanticContext&) = delete;
    VnlcSemanticContext& operator=(const VnlcSemanticContext&) = delete;
    VnlcSemanticContext(VnlcSemanticContext&&) noexcept = default;
    VnlcSemanticContext& operator=(VnlcSemanticContext&&) noexcept = default;

    void reportError(const VnlcAstNode& node, std::string_view message);
    void reportWarning(const VnlcAstNode& node, std::string_view message);
    void reportNote(const VnlcAstNode& node, std::string_view message);

    void pushScope(std::unique_ptr<VnlcScope>&& scope);
    void popScope();

    void registerCustomizedType(std::string_view fullName, std::unique_ptr<VnlcCustomizedType>&& customizedType);
    void mapSemanticType(const VnlcTypeNode* typeNode, const VnlcSemanticType* semanticType);
    void mapInferredValueType(const VnlcValueDeclarationNode* valueDeclaration, const VnlcSemanticType* semanticType);
    void mapInferredFunctionReturnType(const VnlcFunctionDeclarationNode* functionDeclaration, const VnlcSemanticType* semanticType);

    void collectImportedPackages(std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>>&& importedPackages);

    [[nodiscard]] const std::optional<const VnlcCustomizedType*> getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const;
    [[nodiscard]] const std::optional<const VnlcSemanticType*> getSemanticTypeByTypeNode(const VnlcTypeNode* typeNode) const;

    [[nodiscard]] VnlcScope& currentScope();

    [[nodiscard]] const VnlcScope* currentModule();
    [[nodiscard]] const VnlcScope* currentFunction();
    [[nodiscard]] const VnlcScope* currentClass();
    [[nodiscard]] const VnlcScope* currentInterface();
    [[nodiscard]] const VnlcScope* currentEnum();
    [[nodiscard]] const VnlcScope* currentBlock();
    [[nodiscard]] const VnlcScope* currentLoop();
    [[nodiscard]] const VnlcScope* currentSwitch();

    [[nodiscard]] const std::vector<VnlcDiagnostic>& getErrors() const noexcept;
    [[nodiscard]] const std::vector<VnlcDiagnostic>& getWarnings() const noexcept;
    [[nodiscard]] const std::vector<VnlcDiagnostic>& getNotes() const noexcept;

    [[nodiscard]] unsigned int getLoopDepth() const noexcept;
    [[nodiscard]] unsigned int getSwitchDepth() const noexcept;

    [[nodiscard]] std::tuple<std::vector<VnlcDiagnostic>, std::vector<VnlcDiagnostic>, std::vector<VnlcDiagnostic>> takeDiagnostics();
    [[nodiscard]] std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> takeImportedPackages();
    [[nodiscard]] std::unordered_set<std::unique_ptr<VnlcCustomizedType>> takeCustomizedTypeRegistry();
    [[nodiscard]] std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*> takeSemanticTypeMap();
    [[nodiscard]] std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*> takeInferredValueTypeMap();
    [[nodiscard]] std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*> takeInferredFunctionReturnTypeMap();
};

#endif // VNLC_SEMANTIC_CONTEXT_HPP