#include "ConstructorDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"

namespace vnlc {
    ConstructorDeclarationNode::ConstructorDeclarationNode(
        ConstructorDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<BlockStatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    )
        : DeclarationNode(firstToken, lastToken),
          internalName(""),
          accessModifier(accessModifier),
          parameters(std::move(parameters)),
          body(std::move(body)) {}

    ConstructorDeclarationNode::ConstructorDeclarationNode(
        ConstructorDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<BlockStatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    )
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          internalName(""),
          accessModifier(accessModifier),
          parameters(std::move(parameters)),
          body(std::move(body)) {}

    std::string_view ConstructorDeclarationNode::getInternalName() const noexcept {
        return internalName;
    }

    const ConstructorDeclarationKind::AccessModifier ConstructorDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& ConstructorDeclarationNode::getParameters() const noexcept {
        return parameters;
    }

    const BlockStatementNode& ConstructorDeclarationNode::getBody() const noexcept {
        return *body;
    }

    void ConstructorDeclarationNode::setInternalName(std::string_view internalName) const {
        this->internalName = std::string(internalName);
    }
} // namespace vnlc
