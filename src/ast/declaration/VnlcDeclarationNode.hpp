#ifndef VNLC_DECLARATION_NODE_HPP
#define VNLC_DECLARATION_NODE_HPP

#include "ast/VnlcAstNode.hpp"
#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

class VnlcDeclarationNode : public VnlcAstNode {
private:
    VnlcDeclarationNode() = delete;

    bool includeMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;

protected:
    VnlcDeclarationNode(const VnlcToken& firstToken, const VnlcToken& lastToken);
    VnlcDeclarationNode(const VnlcToken& firstToken, const VnlcToken& lastToken, std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms) noexcept;

public:
    [[nodiscard]] const bool doesIncludeMetadata() const noexcept;
    [[nodiscard]] const std::vector<VnlcDeclarationItem::MetadataTerm>& getMetadataTerms() const noexcept;
};

#endif // VNLC_DECLARATION_NODE_HPP