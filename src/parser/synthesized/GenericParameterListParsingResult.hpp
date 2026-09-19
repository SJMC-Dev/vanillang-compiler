#ifndef VNLC_GENERIC_PARAMETER_LIST_PARSING_RESULT_HPP
#define VNLC_GENERIC_PARAMETER_LIST_PARSING_RESULT_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct GenericParameterListParsingResult {
        std::vector<std::unique_ptr<IdentifierNode>> parameters;
    };
} // namespace vnlc

#endif // VNLC_GENERIC_PARAMETER_LIST_PARSING_RESULT_HPP
