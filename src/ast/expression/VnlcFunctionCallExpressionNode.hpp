#ifndef VNLC_FUNCTION_CALL_EXPRESSION_NODE_HPP
#define VNLC_FUNCTION_CALL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>
#include <optional>
#include <vector>

class VnlcFunctionCallExpressionNode : public VnlcExpressionNode {
private:
    VnlcFunctionCallExpressionNode() = delete;

    std::unique_ptr<VnlcExpressionNode> callee;
    std::vector<std::unique_ptr<VnlcExpressionNode>> arguments;
    std::optional<std::unique_ptr<VnlcExpressionNode>> context;

public:
    VnlcFunctionCallExpressionNode(
        std::unique_ptr<VnlcExpressionNode>&& callee,
        std::vector<std::unique_ptr<VnlcExpressionNode>>&& arguments,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcFunctionCallExpressionNode(
        std::unique_ptr<VnlcExpressionNode>&& callee,
        std::vector<std::unique_ptr<VnlcExpressionNode>>&& arguments,
        std::unique_ptr<VnlcExpressionNode>&& context,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcExpressionNode& getCallee() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcExpressionNode>>& getArguments() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcExpressionNode>>& getContext() const noexcept;
};

#endif // VNLC_FUNCTION_CALL_EXPRESSION_NODE_HPP
