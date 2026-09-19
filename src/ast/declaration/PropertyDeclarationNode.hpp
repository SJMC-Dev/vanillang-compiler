#ifndef VNLC_PROPERTY_DECLARATION_NODE_HPP
#define VNLC_PROPERTY_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/PropertyDeclarationType.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/type/TypeNode.hpp"

namespace vnlc {
    class PropertyDeclarationNode : public DeclarationNode {
    private:
        PropertyDeclarationNode() = delete;

        PropertyDeclarationType::AccessModifier accessModifier;
        PropertyDeclarationType::Binding binding;
        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeNode> type;
        std::optional<std::unique_ptr<ExpressionNode>> initializer;

    public:
        PropertyDeclarationNode(
            PropertyDeclarationType::AccessModifier accessModifier,
            PropertyDeclarationType::Binding binding,
            std::unique_ptr<IdentifierNode>&& name,
            std::unique_ptr<TypeNode>&& type,
            std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        PropertyDeclarationNode(
            PropertyDeclarationType::AccessModifier accessModifier,
            PropertyDeclarationType::Binding binding,
            std::unique_ptr<IdentifierNode>&& name,
            std::unique_ptr<TypeNode>&& type,
            std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const PropertyDeclarationType::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const PropertyDeclarationType::Binding getBinding() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const TypeNode& getType() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<ExpressionNode>>& getInitializer() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_PROPERTY_DECLARATION_NODE_HPP