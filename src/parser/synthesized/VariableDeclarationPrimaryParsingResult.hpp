#ifndef VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationType.hpp"
#include "ast/type/TypeNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    struct VariableDeclarationPrimaryParsingResult {
        ValueDeclarationType::Kind kind;
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeNode>> type;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP