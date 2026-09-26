#ifndef VNLC_IDENTIFIER_EXPRESSION_NODE_HPP
#define VNLC_IDENTIFIER_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class IdentifierLikeExpressionNode : public PrimaryExpressionNode {
    private:
        IdentifierLikeExpressionNode() = delete;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<IdentifierLikeExpressionNode>> genericArguments;

    public:
        IdentifierLikeExpressionNode(std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierLikeExpressionNode>>& getGenericArguments() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_IDENTIFIER_EXPRESSION_NODE_HPP
