#ifndef VNLC_DICT_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_DICT_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcLiteralExpressionNode.hpp"
#include <memory>
#include <unordered_map>

class VnlcDictLiteralExpressionNode : public VnlcLiteralExpressionNode {
private:
    VnlcDictLiteralExpressionNode() = delete;

    std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>> entries;

public:
    VnlcDictLiteralExpressionNode(std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>>&& entries, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>>& getEntries() const noexcept;
};

#endif // VNLC_DICT_LITERAL_EXPRESSION_NODE_HPP
