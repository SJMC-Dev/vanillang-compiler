#include "VnlcEnumDeclarationNode.hpp"
#include "ast/identifier/VnlcIdentifierNode.hpp"

VnlcEnumDeclarationNode::VnlcEnumDeclarationNode(
    std::unique_ptr<VnlcIdentifierNode>&& name,
    std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
    std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>&& memberDeclarations,
    const VnlcToken& firstToken,
    const VnlcToken& lastToken
) noexcept
    : VnlcTypeDeclarationNode(firstToken, lastToken),
      name(std::move(name)),
      memberDeclarations(std::move(memberDeclarations)),
      genericParameterNames(std::move(genericParameterNames)) {}

VnlcEnumDeclarationNode::VnlcEnumDeclarationNode(
    std::unique_ptr<VnlcIdentifierNode>&& name,
    std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
    std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>&& memberDeclarations,
    const VnlcToken& firstToken,
    const VnlcToken& lastToken,
    std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
) noexcept
    : VnlcTypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
      name(std::move(name)),
      memberDeclarations(std::move(memberDeclarations)),
      genericParameterNames(std::move(genericParameterNames)) {}

const VnlcIdentifierNode& VnlcEnumDeclarationNode::getName() const noexcept {
    return *name;
}

const std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>& VnlcEnumDeclarationNode::getMemberDeclarations() const noexcept {
    return memberDeclarations;
}

const std::vector<std::unique_ptr<VnlcIdentifierNode>>& VnlcEnumDeclarationNode::getGenericParameterNames() const noexcept {
    return genericParameterNames;
}