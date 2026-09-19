#ifndef VNLC_BLOCK_STATEMENT_PARSING_RESULT_HPP
#define VNLC_BLOCK_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/BlockStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct BlockStatementParsingResult {
        std::unique_ptr<BlockStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_BLOCK_STATEMENT_PARSING_RESULT_HPP
