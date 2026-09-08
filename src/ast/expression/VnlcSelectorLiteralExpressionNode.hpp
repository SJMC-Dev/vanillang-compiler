#ifndef VNLC_SELECTOR_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_SELECTOR_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcLiteralExpressionNode.hpp"
#include "ast/expression/VnlcSelectorLiteralExpressionType.hpp"
#include <memory>
#include <unordered_map>

class VnlcSelectorLiteralExpressionNode : public VnlcLiteralExpressionNode {
private:
    VnlcSelectorLiteralExpressionNode() = delete;

    VnlcSelectorLiteralExpressionType type;
    std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>> arguments;

public:
    VnlcSelectorLiteralExpressionNode(
        VnlcSelectorLiteralExpressionType type,
        std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>>&& arguments,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcSelectorLiteralExpressionType getType() const noexcept;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>>& getArguments() const noexcept;
};

#endif // VNLC_SELECTOR_LITERAL_EXPRESSION_NODE_HPP