#include "ConstructorDeclarationNode.hpp"

namespace vnlc {
    ConstructorDeclarationNode::ConstructorDeclarationNode(
        FunctionDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<BlockStatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          parameters(std::move(parameters)),
          body(std::move(body)) {}

    ConstructorDeclarationNode::ConstructorDeclarationNode(
        FunctionDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<BlockStatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          parameters(std::move(parameters)),
          body(std::move(body)) {}

    const FunctionDeclarationKind::AccessModifier ConstructorDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& ConstructorDeclarationNode::getParameters() const noexcept {
        return parameters;
    }

    const BlockStatementNode& ConstructorDeclarationNode::getBody() const noexcept {
        return *body;
    }
} // namespace vnlc
