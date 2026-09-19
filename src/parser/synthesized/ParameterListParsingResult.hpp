#ifndef VNLC_PARAMETER_LIST_PARSING_RESULT_HPP
#define VNLC_PARAMETER_LIST_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ParameterListParsingResult {
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
    };
} // namespace vnlc

#endif // VNLC_PARAMETER_LIST_PARSING_RESULT_HPP
