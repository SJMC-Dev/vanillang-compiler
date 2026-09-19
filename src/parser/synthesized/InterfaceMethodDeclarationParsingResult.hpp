#ifndef VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP
#define VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/FunctionDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct InterfaceMethodDeclarationParsingResult {
        std::unique_ptr<FunctionDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP
