#ifndef VNLC_DECLARATION_NODE_HPP
#define VNLC_DECLARATION_NODE_HPP

#include "ast/AstNode.hpp"
#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    class DeclarationNode : public AstNode {
    private:
        DeclarationNode() = delete;

        bool includeMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;

    protected:
        DeclarationNode(const Token& firstToken, const Token& lastToken);
        DeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<DeclarationItem::MetadataTerm>&& metadataTerms) noexcept;

    public:
        [[nodiscard]] const bool doesIncludeMetadata() const noexcept;
        [[nodiscard]] const std::vector<DeclarationItem::MetadataTerm>& getMetadataTerms() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_DECLARATION_NODE_HPP