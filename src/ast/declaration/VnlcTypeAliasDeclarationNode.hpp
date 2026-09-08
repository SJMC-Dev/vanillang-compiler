#ifndef VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcTypeDeclarationNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <vector>

class VnlcTypeAliasDeclarationNode : public VnlcTypeDeclarationNode {
private:
    VnlcTypeAliasDeclarationNode() = delete;

    std::unique_ptr<VnlcIdentifierNode> aliasName;
    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameterNames;
    std::unique_ptr<VnlcTypeNode> originalType;

public:
    VnlcTypeAliasDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& aliasName,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::unique_ptr<VnlcTypeNode>&& originalType,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcTypeAliasDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& aliasName,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::unique_ptr<VnlcTypeNode>&& originalType,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcIdentifierNode& getAliasName() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcIdentifierNode>>& getGenericParameterNames() const noexcept;
    [[nodiscard]] const VnlcTypeNode& getOriginalType() const noexcept;
};

#endif // VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP