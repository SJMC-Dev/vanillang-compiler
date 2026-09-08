#ifndef VNLC_MODULE_INTERFACE_FILE_GENERATOR_HPP
#define VNLC_MODULE_INTERFACE_FILE_GENERATOR_HPP

#include "ast/declaration/VnlcClassDeclarationNode.hpp"
#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcEnumDeclarationNode.hpp"
#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include "ast/declaration/VnlcInterfaceDeclarationNode.hpp"
#include "ast/declaration/VnlcTypeAliasDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "config/VnlcConfig.hpp"
#include "semantic/VnlcSemanticAnalysisResult.hpp"
#include <nlohmann/json.hpp>
#include <vector>

class VnlcModuleInterfaceFileGenerator {
private:
    std::vector<const VnlcDeclarationNode*> declarationNodes;
    std::vector<std::string> importedAliases;
    const VnlcConfig& config;
    const VnlcSemanticAnalysisResult& semantic;

    [[nodiscard]] nlohmann::json stringifyMetadata(const std::vector<VnlcDeclarationItem::MetadataTerm>& metadataTerms);
    [[nodiscard]] nlohmann::json stringifyGenericParameters(const std::vector<std::unique_ptr<VnlcIdentifierNode>>& genericParameterNames);
    [[nodiscard]] nlohmann::json stringifyParameter(const VnlcValueDeclarationNode* parameter);
    [[nodiscard]] nlohmann::json stringifyVariable(const VnlcValueDeclarationNode* variable);
    [[nodiscard]] nlohmann::json stringifyFunction(const VnlcFunctionDeclarationNode* function);
    [[nodiscard]] nlohmann::json stringifyClass(const VnlcClassDeclarationNode* classNode);
    [[nodiscard]] nlohmann::json stringifyInterface(const VnlcInterfaceDeclarationNode* interfaceNode);
    [[nodiscard]] nlohmann::json stringifyEnum(const VnlcEnumDeclarationNode* enumNode);
    [[nodiscard]] nlohmann::json stringifyEnumMember(const VnlcEnumMemberDeclarationNode* enumMember);
    [[nodiscard]] nlohmann::json stringifyEnumValue(const VnlcValueDeclarationNode* enumValue);
    [[nodiscard]] nlohmann::json stringifyTypeAlias(const VnlcTypeAliasDeclarationNode* typeAliasNode);
    [[nodiscard]] nlohmann::json stringifyProperty(const VnlcValueDeclarationNode* property);
    [[nodiscard]] nlohmann::json stringifyMethod(const VnlcFunctionDeclarationNode* method);
    [[nodiscard]] nlohmann::json stringifyImported(std::string_view importedAlias);

public:
    VnlcModuleInterfaceFileGenerator(
        std::vector<const VnlcDeclarationNode*>&& declarationNodes,
        std::vector<std::string>&& importedAliases,
        const VnlcConfig& config,
        const VnlcSemanticAnalysisResult& semantic
    ) noexcept;

    void generate();
};

#endif // VNLC_MODULE_INTERFACE_FILE_GENERATOR_HPP