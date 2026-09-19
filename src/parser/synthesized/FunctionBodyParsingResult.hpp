#ifndef VNLC_FUNCTION_BODY_PARSING_RESULT_HPP
#define VNLC_FUNCTION_BODY_PARSING_RESULT_HPP

#include "ast/statement/BlockStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct FunctionBodyParsingResult {
        std::unique_ptr<BlockStatementNode> body;
    };
} // namespace vnlc

#endif // VNLC_FUNCTION_BODY_PARSING_RESULT_HPP
