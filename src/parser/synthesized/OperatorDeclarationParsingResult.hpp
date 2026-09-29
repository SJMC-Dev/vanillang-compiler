#ifndef VNLC_OPERATOR_DECLARATION_PARSING_RESULT_HPP
#define VNLC_OPERATOR_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/OperatorDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct OperatorDeclarationParsingResult {
        std::unique_ptr<OperatorDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_OPERATOR_DECLARATION_PARSING_RESULT_HPP
