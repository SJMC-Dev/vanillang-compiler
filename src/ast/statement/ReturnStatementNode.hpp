#ifndef VNLC_RETURN_STATEMENT_NODE_HPP
#define VNLC_RETURN_STATEMENT_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class ReturnStatementNode : public ControlFlowStatementNode {
    private:
        ReturnStatementNode() = delete;

        std::optional<std::unique_ptr<ExpressionNode>> returnValue;

    public:
        ReturnStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        ReturnStatementNode(std::unique_ptr<ExpressionNode>&& returnValue, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::optional<std::unique_ptr<ExpressionNode>>& getReturnValue() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_RETURN_STATEMENT_NODE_HPP