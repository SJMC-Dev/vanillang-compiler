#ifndef VNLC_MODULE_INTERFACE_FILE_GENERATOR_HPP
#define VNLC_MODULE_INTERFACE_FILE_GENERATOR_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "config/Config.hpp"
#include "semantic/SemanticResult.hpp"
#include <nlohmann/json.hpp>
#include <vector>

namespace vnlc {
    class ModuleInterfaceFileGenerator {
    private:
        std::vector<const DeclarationNode*> declarationNodes;
        std::vector<std::string> importedAliases;
        const Config& config;
        const SemanticResult& semantic;

        [[nodiscard]] nlohmann::json stringifyMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms);
        [[nodiscard]] nlohmann::json stringifyGenericParameters(const std::vector<std::unique_ptr<IdentifierNode>>& genericParameterNames);
        [[nodiscard]] nlohmann::json stringifyParameter(const ValueDeclarationNode* parameter);
        [[nodiscard]] nlohmann::json stringifyVariable(const ValueDeclarationNode* variable);
        [[nodiscard]] nlohmann::json stringifyFunction(const FunctionDeclarationNode* function);
        [[nodiscard]] nlohmann::json stringifyClass(const ClassDeclarationNode* classNode);
        [[nodiscard]] nlohmann::json stringifyInterface(const InterfaceDeclarationNode* interfaceNode);
        [[nodiscard]] nlohmann::json stringifyEnum(const EnumDeclarationNode* enumNode);
        [[nodiscard]] nlohmann::json stringifyEnumMember(const EnumMemberDeclarationNode* enumMember);
        [[nodiscard]] nlohmann::json stringifyEnumValue(const ValueDeclarationNode* enumValue);
        [[nodiscard]] nlohmann::json stringifyTypeAlias(const TypeAliasDeclarationNode* typeAliasNode);
        [[nodiscard]] nlohmann::json stringifyProperty(const ValueDeclarationNode* property);
        [[nodiscard]] nlohmann::json stringifyMethod(const FunctionDeclarationNode* method);
        [[nodiscard]] nlohmann::json stringifyImported(std::string_view importedAlias);

    public:
        ModuleInterfaceFileGenerator(
            std::vector<const DeclarationNode*>&& declarationNodes,
            std::vector<std::string>&& importedAliases,
            const Config& config,
            const SemanticResult& semantic
        ) noexcept;

        void generate();
    };
} // namespace vnlc

#endif // VNLC_MODULE_INTERFACE_FILE_GENERATOR_HPP