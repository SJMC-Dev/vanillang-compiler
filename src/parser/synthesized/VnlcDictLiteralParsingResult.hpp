#ifndef VNLC_DICT_LITERAL_PARSING_RESULT_HPP
#define VNLC_DICT_LITERAL_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcDictLiteralParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_DICT_LITERAL_PARSING_RESULT_HPP
