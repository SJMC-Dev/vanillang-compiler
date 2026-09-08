#ifndef VNLC_EXPORT_DECLARATION_PARSING_RESULT_HPP
#define VNLC_EXPORT_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcExportDeclarationNode.hpp"
#include <memory>

struct VnlcExportDeclarationParsingResult {
    std::unique_ptr<VnlcExportDeclarationNode> declaration;
};

#endif // VNLC_EXPORT_DECLARATION_PARSING_RESULT_HPP
