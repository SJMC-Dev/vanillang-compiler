#ifndef VNLC_WHILE_STATEMENT_PARSING_CONTEXT_HPP
#define VNLC_WHILE_STATEMENT_PARSING_CONTEXT_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>
#include <optional>

struct VnlcWhileStatementParsingContext {
    std::optional<std::unique_ptr<VnlcIdentifierNode>> label;
};

#endif // VNLC_WHILE_STATEMENT_PARSING_CONTEXT_HPP