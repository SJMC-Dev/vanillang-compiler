#ifndef VNLC_INTERFACE_DECLARATION_PARSING_RESULT_HPP
#define VNLC_INTERFACE_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcInterfaceDeclarationNode.hpp"
#include <memory>

struct VnlcInterfaceDeclarationParsingResult {
    std::unique_ptr<VnlcInterfaceDeclarationNode> declaration;
};

#endif // VNLC_INTERFACE_DECLARATION_PARSING_RESULT_HPP
