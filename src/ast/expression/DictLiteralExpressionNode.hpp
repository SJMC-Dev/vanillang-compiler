#ifndef VNLC_DICT_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_DICT_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/LiteralExpressionNode.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class DictLiteralExpressionNode : public LiteralExpressionNode {
    private:
        DictLiteralExpressionNode() = delete;

        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> entries;

    public:
        DictLiteralExpressionNode(std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& entries, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& getEntries() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_DICT_LITERAL_EXPRESSION_NODE_HPP
