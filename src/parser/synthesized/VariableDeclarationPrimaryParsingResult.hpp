#ifndef VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationKind.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    struct VariableDeclarationPrimaryParsingResult {
        ValueDeclarationKind::Kind kind;
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeReferenceNode>> type;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP