#include "InterfaceDeclarationNode.hpp"

namespace vnlc {
    InterfaceDeclarationNode::InterfaceDeclarationNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          methodDeclarations(std::move(methodDeclarations)) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          methodDeclarations(std::move(methodDeclarations)) {}

    const IdentifierNode& InterfaceDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& InterfaceDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getMethodDeclarations() const noexcept {
        return methodDeclarations;
    }
} // namespace vnlc
