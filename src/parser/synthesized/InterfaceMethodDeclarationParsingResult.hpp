#ifndef VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP
#define VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct InterfaceMethodDeclarationParsingResult {
        std::unique_ptr<DeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP
