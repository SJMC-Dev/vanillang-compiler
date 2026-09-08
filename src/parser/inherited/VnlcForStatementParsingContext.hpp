#ifndef VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP
#define VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>
#include <optional>

struct VnlcForStatementParsingContext {
    std::optional<std::unique_ptr<VnlcIdentifierNode>> label;
};

#endif // VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP