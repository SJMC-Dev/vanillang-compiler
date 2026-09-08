#ifndef VNLC_IMPORT_PATH_PARSING_RESULT_HPP
#define VNLC_IMPORT_PATH_PARSING_RESULT_HPP

#include "ast/declaration/VnlcImportDeclarationItem.hpp"
#include <memory>

struct VnlcImportPathParsingResult {
    std::unique_ptr<VnlcImportDeclarationItem> paths;
};

#endif // VNLC_IMPORT_PATH_PARSING_RESULT_HPP
