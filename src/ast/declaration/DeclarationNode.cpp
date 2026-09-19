#include "DeclarationNode.hpp"

namespace vnlc {
    DeclarationNode::DeclarationNode(const Token& firstToken, const Token& lastToken) : AstNode(firstToken, lastToken), includeMetadata(false), metadataTerms() {}

    DeclarationNode::DeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<DeclarationItem::MetadataTerm>&& metadataTerms) noexcept
        : AstNode(firstToken, lastToken),
          includeMetadata(true),
          metadataTerms(std::move(metadataTerms)) {}

    const bool DeclarationNode::doesIncludeMetadata() const noexcept {
        return includeMetadata;
    }

    const std::vector<DeclarationItem::MetadataTerm>& DeclarationNode::getMetadataTerms() const noexcept {
        return metadataTerms;
    }
} // namespace vnlc
