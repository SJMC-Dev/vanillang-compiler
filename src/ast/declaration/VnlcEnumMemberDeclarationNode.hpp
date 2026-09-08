#ifndef VNLC_ENUM_MEMBER_DECLARATION_NODE_HPP
#define VNLC_ENUM_MEMBER_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include <vector>

class VnlcEnumMemberDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcEnumMemberDeclarationNode() = delete;

    std::unique_ptr<VnlcIdentifierNode> name;
    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> associatedValues;

public:
    VnlcEnumMemberDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcValueDeclarationNode>>&& associatedValues,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcEnumMemberDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcValueDeclarationNode>>&& associatedValues,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcValueDeclarationNode>>& getAssociatedValues() const noexcept;
};

#endif // VNLC_ENUM_MEMBER_DECLARATION_NODE_HPP