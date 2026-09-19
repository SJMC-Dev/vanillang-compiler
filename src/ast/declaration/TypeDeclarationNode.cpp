#include "TypeDeclarationNode.hpp"

namespace vnlc {
    TypeDeclarationNode::TypeDeclarationNode(const Token& firstToken, const Token& lastToken) noexcept : DeclarationNode(firstToken, lastToken) {}

    TypeDeclarationNode::TypeDeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<DeclarationItem::MetadataTerm>&& metadataTerms) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)) {}
} // namespace vnlc
