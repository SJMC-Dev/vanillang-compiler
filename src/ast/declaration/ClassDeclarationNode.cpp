#include "ClassDeclarationNode.hpp"

namespace vnlc {
    ClassDeclarationNode::ClassDeclarationNode(
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::optional<std::unique_ptr<TypeReferenceNode>>&& baseClass,
        std::vector<std::unique_ptr<TypeReferenceNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& propertyDeclarations,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        std::vector<std::unique_ptr<ConstructorDeclarationNode>>&& constructorDeclarations,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operatorDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          genericParameterNames(std::move(genericParameterNames)),
          propertyDeclarations(std::move(propertyDeclarations)),
          methodDeclarations(std::move(methodDeclarations)),
          constructorDeclarations(std::move(constructorDeclarations)),
          operatorDeclarations(std::move(operatorDeclarations)) {}

    ClassDeclarationNode::ClassDeclarationNode(
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::optional<std::unique_ptr<TypeReferenceNode>>&& baseClass,
        std::vector<std::unique_ptr<TypeReferenceNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& propertyDeclarations,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        std::vector<std::unique_ptr<ConstructorDeclarationNode>>&& constructorDeclarations,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operatorDeclarations,
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
          propertyDeclarations(std::move(propertyDeclarations)),
          methodDeclarations(std::move(methodDeclarations)),
          constructorDeclarations(std::move(constructorDeclarations)),
          operatorDeclarations(std::move(operatorDeclarations)) {}

    const bool ClassDeclarationNode::isFinal() const noexcept {
        return final;
    }

    const IdentifierNode& ClassDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::optional<std::unique_ptr<TypeReferenceNode>>& ClassDeclarationNode::getBaseClass() const noexcept {
        return baseClass;
    }

    const std::vector<std::unique_ptr<TypeReferenceNode>>& ClassDeclarationNode::getImplementedInterfaces() const noexcept {
        return implementedInterfaces;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& ClassDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& ClassDeclarationNode::getPropertyDeclarations() const noexcept {
        return propertyDeclarations;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& ClassDeclarationNode::getMethodDeclarations() const noexcept {
        return methodDeclarations;
    }

    const std::vector<std::unique_ptr<ConstructorDeclarationNode>>& ClassDeclarationNode::getConstructorDeclarations() const noexcept {
        return constructorDeclarations;
    }

    const std::vector<std::unique_ptr<OperatorDeclarationNode>>& ClassDeclarationNode::getOperatorDeclarations() const noexcept {
        return operatorDeclarations;
    }
} // namespace vnlc
