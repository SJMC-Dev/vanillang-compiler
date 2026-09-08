#ifndef VNLC_TYPE_DECLARATION_NODE_HPP
#define VNLC_TYPE_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
class VnlcTypeDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcTypeDeclarationNode() = delete;

protected:
    VnlcTypeDeclarationNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
    VnlcTypeDeclarationNode(const VnlcToken& firstToken, const VnlcToken& lastToken, std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms) noexcept;
};

#endif // VNLC_TYPE_DECLARATION_NODE_HPP