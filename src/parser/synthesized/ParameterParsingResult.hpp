#ifndef VNLC_PARAMETER_PARSING_RESULT_HPP
#define VNLC_PARAMETER_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ParameterParsingResult {
        std::unique_ptr<ValueDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_PARAMETER_PARSING_RESULT_HPP
