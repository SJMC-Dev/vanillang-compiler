#ifndef VNLC_ENUM_DECLARATION_PARSING_RESULT_HPP
#define VNLC_ENUM_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcEnumDeclarationNode.hpp"
#include <memory>

struct VnlcEnumDeclarationParsingResult {
    std::unique_ptr<VnlcEnumDeclarationNode> declaration;
};

#endif // VNLC_ENUM_DECLARATION_PARSING_RESULT_HPP
