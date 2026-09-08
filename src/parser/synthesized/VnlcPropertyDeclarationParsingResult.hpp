#ifndef VNLC_PROPERTY_DECLARATION_PARSING_RESULT_HPP
#define VNLC_PROPERTY_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcPropertyDeclarationNode.hpp"
#include <memory>

struct VnlcPropertyDeclarationParsingResult {
    std::unique_ptr<VnlcPropertyDeclarationNode> declaration;
};

#endif // VNLC_PROPERTY_DECLARATION_PARSING_RESULT_HPP
