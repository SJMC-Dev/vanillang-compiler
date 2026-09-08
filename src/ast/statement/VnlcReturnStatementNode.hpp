#ifndef VNLC_RETURN_STATEMENT_NODE_HPP
#define VNLC_RETURN_STATEMENT_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/statement/VnlcControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

class VnlcReturnStatementNode : public VnlcControlFlowStatementNode {
private:
    VnlcReturnStatementNode() = delete;

    std::optional<std::unique_ptr<VnlcExpressionNode>> returnValue;

public:
    VnlcReturnStatementNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
    VnlcReturnStatementNode(std::unique_ptr<VnlcExpressionNode>&& returnValue, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const std::optional<std::unique_ptr<VnlcExpressionNode>>& getReturnValue() const noexcept;
};

#endif // VNLC_RETURN_STATEMENT_NODE_HPP