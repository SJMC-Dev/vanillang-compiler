#ifndef VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationType.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <optional>

struct VnlcVariableDeclarationPrimaryParsingResult {
    VnlcValueDeclarationType::Kind kind;
    std::unique_ptr<VnlcIdentifierNode> name;
    std::optional<std::unique_ptr<VnlcTypeNode>> type;
};

#endif // VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP