#include "ClassDeclarationNode.hpp"

namespace vnlc {
    ClassDeclarationNode::ClassDeclarationNode(
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::optional<std::unique_ptr<TypeNode>>&& baseClass,
        std::vector<std::unique_ptr<TypeNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<DeclarationNode>>&& memberDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          genericParameterNames(std::move(genericParameterNames)),
          memberDeclarations(std::move(memberDeclarations)) {}

    ClassDeclarationNode::ClassDeclarationNode(
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::optional<std::unique_ptr<TypeNode>>&& baseClass,
        std::vector<std::unique_ptr<TypeNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<DeclarationNode>>&& memberDeclarations,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          genericParameterNames(std::move(genericParameterNames)),
          memberDeclarations(std::move(memberDeclarations)) {}

    const bool ClassDeclarationNode::isFinal() const noexcept {
        return final;
    }

    const IdentifierNode& ClassDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::optional<std::unique_ptr<TypeNode>>& ClassDeclarationNode::getBaseClass() const noexcept {
        return baseClass;
    }

    const std::vector<std::unique_ptr<TypeNode>>& ClassDeclarationNode::getImplementedInterfaces() const noexcept {
        return implementedInterfaces;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& ClassDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<DeclarationNode>>& ClassDeclarationNode::getMemberDeclarations() const noexcept {
        return memberDeclarations;
    }
} // namespace vnlc
