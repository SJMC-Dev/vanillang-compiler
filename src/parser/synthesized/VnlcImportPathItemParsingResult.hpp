#ifndef VNLC_IMPORT_PATH_ITEM_PARSING_RESULT_HPP
#define VNLC_IMPORT_PATH_ITEM_PARSING_RESULT_HPP

#include "ast/declaration/VnlcImportDeclarationItem.hpp"
#include <memory>

struct VnlcImportPathItemParsingResult {
    std::unique_ptr<VnlcImportDeclarationItem> paths;
};

#endif // VNLC_IMPORT_PATH_ITEM_PARSING_RESULT_HPP
