#ifndef VNLC_DICT_LITERAL_PARSING_RESULT_HPP
#define VNLC_DICT_LITERAL_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct DictLiteralParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_DICT_LITERAL_PARSING_RESULT_HPP
