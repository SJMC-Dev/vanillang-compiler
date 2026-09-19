#ifndef VNLC_EXPORT_DECLARATION_PARSING_RESULT_HPP
#define VNLC_EXPORT_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/ExportDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ExportDeclarationParsingResult {
        std::unique_ptr<ExportDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_EXPORT_DECLARATION_PARSING_RESULT_HPP
