#ifndef VNLC_IMPORT_PATH_ITEM_PARSING_RESULT_HPP
#define VNLC_IMPORT_PATH_ITEM_PARSING_RESULT_HPP

#include "ast/declaration/ImportDeclarationItem.hpp"
#include <memory>

namespace vnlc {
    struct ImportPathItemParsingResult {
        std::unique_ptr<ImportDeclarationItem> paths;
    };
} // namespace vnlc

#endif // VNLC_IMPORT_PATH_ITEM_PARSING_RESULT_HPP
