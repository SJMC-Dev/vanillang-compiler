#include "FunctionDeclarationNode.hpp"

namespace vnlc {
    FunctionDeclarationNode::FunctionDeclarationNode(
        FunctionDeclarationType::Kind kind,
        FunctionDeclarationType::Context context,
        FunctionDeclarationType::AccessModifier accessModifier,
        FunctionDeclarationType::Binding binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<TypeNode>>&& returnType,
        std::optional<std::unique_ptr<BlockStatementNode>>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)) {}

    FunctionDeclarationNode::FunctionDeclarationNode(
        FunctionDeclarationType::Kind kind,
        FunctionDeclarationType::Context context,
        FunctionDeclarationType::AccessModifier accessModifier,
        FunctionDeclarationType::Binding binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<TypeNode>>&& returnType,
        std::optional<std::unique_ptr<BlockStatementNode>>&& body,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)) {}

    const FunctionDeclarationType::Kind FunctionDeclarationNode::getKind() const noexcept {
        return kind;
    }

    const FunctionDeclarationType::Context FunctionDeclarationNode::getContext() const noexcept {
        return context;
    }

    const FunctionDeclarationType::AccessModifier FunctionDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const FunctionDeclarationType::Binding FunctionDeclarationNode::getBinding() const noexcept {
        return binding;
    }

    const IdentifierNode& FunctionDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& FunctionDeclarationNode::getParameters() const noexcept {
        return parameters;
    }

    const std::optional<std::unique_ptr<TypeNode>>& FunctionDeclarationNode::getReturnType() const noexcept {
        return returnType;
    }

    const std::optional<std::unique_ptr<BlockStatementNode>>& FunctionDeclarationNode::getBody() const noexcept {
        return body;
    }
} // namespace vnlc
