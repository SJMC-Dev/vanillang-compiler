#ifndef VNLC_FUNCTION_BODY_PARSING_RESULT_HPP
#define VNLC_FUNCTION_BODY_PARSING_RESULT_HPP

#include "ast/statement/VnlcBlockStatementNode.hpp"
#include <memory>

struct VnlcFunctionBodyParsingResult {
    std::unique_ptr<VnlcBlockStatementNode> body;
};

#endif // VNLC_FUNCTION_BODY_PARSING_RESULT_HPP
