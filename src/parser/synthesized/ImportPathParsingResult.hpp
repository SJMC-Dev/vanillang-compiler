#ifndef VNLC_IMPORT_PATH_PARSING_RESULT_HPP
#define VNLC_IMPORT_PATH_PARSING_RESULT_HPP

#include "ast/declaration/ImportDeclarationItem.hpp"
#include <memory>

namespace vnlc {
    struct ImportPathParsingResult {
        std::unique_ptr<ImportDeclarationItem> paths;
    };
} // namespace vnlc

#endif // VNLC_IMPORT_PATH_PARSING_RESULT_HPP
