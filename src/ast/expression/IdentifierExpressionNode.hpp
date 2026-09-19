#ifndef VNLC_IDENTIFIER_EXPRESSION_NODE_HPP
#define VNLC_IDENTIFIER_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include <memory>

namespace vnlc {
    class IdentifierExpressionNode : public PrimaryExpressionNode {
    private:
        IdentifierExpressionNode() = delete;

        std::unique_ptr<IdentifierNode> name;

    public:
        IdentifierExpressionNode(std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_IDENTIFIER_EXPRESSION_NODE_HPP
