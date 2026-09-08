#ifndef VNLC_CLASS_DECLARATION_PARSING_RESULT_HPP
#define VNLC_CLASS_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcClassDeclarationNode.hpp"
#include <memory>

struct VnlcClassDeclarationParsingResult {
    std::unique_ptr<VnlcClassDeclarationNode> declaration;
};

#endif // VNLC_CLASS_DECLARATION_PARSING_RESULT_HPP
