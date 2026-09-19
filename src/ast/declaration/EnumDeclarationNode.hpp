#ifndef VNLC_ENUM_DECLARATION_NODE_HPP
#define VNLC_ENUM_DECLARATION_NODE_HPP

#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"

namespace vnlc {
    class EnumDeclarationNode : public TypeDeclarationNode {
    private:
        EnumDeclarationNode() = delete;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> memberDeclarations;

    public:
        EnumDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        EnumDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<EnumMemberDeclarationNode>>& getMemberDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameterNames() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_ENUM_DECLARATION_NODE_HPP