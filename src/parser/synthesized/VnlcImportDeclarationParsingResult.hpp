#ifndef VNLC_IMPORT_DECLARATION_PARSING_RESULT_HPP
#define VNLC_IMPORT_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcImportDeclarationNode.hpp"
#include <memory>

struct VnlcImportDeclarationParsingResult {
    std::unique_ptr<VnlcImportDeclarationNode> declaration;
};

#endif // VNLC_IMPORT_DECLARATION_PARSING_RESULT_HPP
