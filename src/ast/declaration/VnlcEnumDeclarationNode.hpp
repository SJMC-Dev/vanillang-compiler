#ifndef VNLC_ENUM_DECLARATION_NODE_HPP
#define VNLC_ENUM_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcEnumMemberDeclarationNode.hpp"
#include "ast/declaration/VnlcTypeDeclarationNode.hpp"

class VnlcEnumDeclarationNode : public VnlcTypeDeclarationNode {
private:
    VnlcEnumDeclarationNode() = delete;

    std::unique_ptr<VnlcIdentifierNode> name;
    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameterNames;
    std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>> memberDeclarations;

public:
    VnlcEnumDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>&& memberDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcEnumDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>&& memberDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>& getMemberDeclarations() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcIdentifierNode>>& getGenericParameterNames() const noexcept;
};

#endif // VNLC_ENUM_DECLARATION_NODE_HPP