#ifndef VNLC_TYPE_DECLARATION_NODE_HPP
#define VNLC_TYPE_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"

namespace vnlc {
    class TypeDeclarationNode : public DeclarationNode {
    private:
        TypeDeclarationNode() = delete;

    protected:
        TypeDeclarationNode(const Token& firstToken, const Token& lastToken) noexcept;
        TypeDeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<DeclarationItem::MetadataTerm>&& metadataTerms) noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_DECLARATION_NODE_HPP