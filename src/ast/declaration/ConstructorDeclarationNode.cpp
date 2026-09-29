#include "ConstructorDeclarationNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceNode.hpp"

namespace vnlc {
    std::string ConstructorDeclarationNode::generateNamespaceIdFromTypeName(const TypeReferenceNode& typeNode) {
        if (const auto* primitiveType = dynamic_cast<const PrimitiveTypeReferenceNode*>(&typeNode)) {
            return std::string(primitiveType->getPrimitiveTypeName());
        }

        const auto* customizedType = dynamic_cast<const CustomizedTypeReferenceNode*>(&typeNode);
        if (customizedType == nullptr) {
            return {};
        }

        std::string identifier;
        for (const auto& part : customizedType->getNameParts()) {
            identifier += std::string(part->getIdentifierString()) + ".";
        }

        if (identifier.ends_with(".")) {
            identifier.pop_back();
        }

        if (!customizedType->getGenericArguments().empty()) {
            identifier += ".-";
            for (const auto& genericArgument : customizedType->getGenericArguments()) {
                identifier += generateNamespaceIdFromTypeName(*genericArgument) + "-";
            }
            identifier.pop_back();
            identifier += "-.";
        }

        return identifier;
    }

    std::string ConstructorDeclarationNode::generateInternalName(const std::vector<std::unique_ptr<ValueDeclarationNode>>& parameters) {
        std::string identifier = "__vnl_constructor";

        if (!parameters.empty()) {
            identifier += '-';
            for (const auto& parameter : parameters) {
                if (parameter->getType().has_value()) {
                    identifier += '-' + generateNamespaceIdFromTypeName(*parameter->getType().value());
                }
            }
        }

        identifier += "__";
        return identifier;
    }

    ConstructorDeclarationNode::ConstructorDeclarationNode(
        ConstructorDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<BlockStatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    )
        : DeclarationNode(firstToken, lastToken),
          internalName(generateInternalName(parameters)),
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
          internalName(generateInternalName(parameters)),
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
} // namespace vnlc
