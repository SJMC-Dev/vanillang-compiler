#ifndef VNLC_INTERFACE_DECLARATION_PARSING_RESULT_HPP
#define VNLC_INTERFACE_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct InterfaceDeclarationParsingResult {
        std::unique_ptr<InterfaceDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_DECLARATION_PARSING_RESULT_HPP
