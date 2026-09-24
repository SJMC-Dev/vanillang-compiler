#ifndef VNLC_GENERIC_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_GENERIC_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct GenericArgumentListParsingResult {
        std::vector<std::unique_ptr<TypeReferenceNode>> arguments;
    };
} // namespace vnlc

#endif // VNLC_GENERIC_ARGUMENT_LIST_PARSING_RESULT_HPP
