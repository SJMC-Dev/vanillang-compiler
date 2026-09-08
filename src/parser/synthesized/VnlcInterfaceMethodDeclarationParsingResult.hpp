#ifndef VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP
#define VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include <memory>

struct VnlcInterfaceMethodDeclarationParsingResult {
    std::unique_ptr<VnlcFunctionDeclarationNode> declaration;
};

#endif // VNLC_INTERFACE_METHOD_DECLARATION_PARSING_RESULT_HPP
