#ifndef VNLC_RELOAD_STATEMENT_PARSING_RESULT_HPP
#define VNLC_RELOAD_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/ReloadStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ReloadStatementParsingResult {
        std::unique_ptr<ReloadStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_RELOAD_STATEMENT_PARSING_RESULT_HPP
