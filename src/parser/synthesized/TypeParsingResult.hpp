#ifndef VNLC_TYPE_PARSING_RESULT_HPP
#define VNLC_TYPE_PARSING_RESULT_HPP

#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeParsingResult {
        std::unique_ptr<TypeReferenceNode> type;
    };
} // namespace vnlc

#endif // VNLC_TYPE_PARSING_RESULT_HPP
