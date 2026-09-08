#ifndef VNLC_TYPE_DECLARATION_PARSING_RESULT_HPP
#define VNLC_TYPE_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcTypeDeclarationNode.hpp"
#include <memory>

struct VnlcTypeDeclarationParsingResult {
    std::unique_ptr<VnlcTypeDeclarationNode> declaration;
};

#endif // VNLC_TYPE_DECLARATION_PARSING_RESULT_HPP
