#ifndef VNLC_IMPORT_DECLARATION_PARSING_RESULT_HPP
#define VNLC_IMPORT_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/ImportDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ImportDeclarationParsingResult {
        std::unique_ptr<ImportDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_IMPORT_DECLARATION_PARSING_RESULT_HPP
