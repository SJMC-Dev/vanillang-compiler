#ifndef VNLC_CONSTRUCTOR_DECLARATION_NODE_HPP
#define VNLC_CONSTRUCTOR_DECLARATION_NODE_HPP

#include "ast/declaration/ConstructorDeclarationKind.hpp"
#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class ConstructorDeclarationNode : public DeclarationNode {
    private:
        ConstructorDeclarationNode() = delete;

        [[nodiscard]] static std::string generateNamespaceIdFromTypeName(const TypeReferenceNode& typeNode);
        [[nodiscard]] static std::string generateInternalName(const std::vector<std::unique_ptr<ValueDeclarationNode>>& parameters);

        std::string internalName;
        ConstructorDeclarationKind::AccessModifier accessModifier;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::unique_ptr<BlockStatementNode> body;

    public:
        ConstructorDeclarationNode(
            ConstructorDeclarationKind::AccessModifier accessModifier,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::unique_ptr<BlockStatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        );

        ConstructorDeclarationNode(
            ConstructorDeclarationKind::AccessModifier accessModifier,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::unique_ptr<BlockStatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        );

        [[nodiscard]] std::string_view getInternalName() const noexcept;
        [[nodiscard]] const ConstructorDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getParameters() const noexcept;
        [[nodiscard]] const BlockStatementNode& getBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_DECLARATION_NODE_HPP
