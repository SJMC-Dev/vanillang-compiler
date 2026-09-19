#ifndef VNLC_CONSTRUCTOR_PARSING_RESULT_HPP
#define VNLC_CONSTRUCTOR_PARSING_RESULT_HPP

#include "ast/declaration/FunctionDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ConstructorParsingResult {
        std::unique_ptr<FunctionDeclarationNode> constructor;
    };
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_PARSING_RESULT_HPP