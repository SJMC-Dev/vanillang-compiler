#ifndef VNLC_ENUM_MEMBER_DECLARATION_NODE_HPP
#define VNLC_ENUM_MEMBER_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include <vector>

namespace vnlc {
    class EnumMemberDeclarationNode : public DeclarationNode {
    private:
        EnumMemberDeclarationNode() = delete;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;

    public:
        EnumMemberDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        EnumMemberDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getAssociatedValues() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_ENUM_MEMBER_DECLARATION_NODE_HPP