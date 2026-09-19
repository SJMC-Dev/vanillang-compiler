#ifndef VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP
#define VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    struct ForStatementParsingContext {
        std::optional<std::unique_ptr<IdentifierNode>> label;
    };
} // namespace vnlc

#endif // VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP