#ifndef VNLC_IMPORT_PATH_LIST_PARSING_RESULT_HPP
#define VNLC_IMPORT_PATH_LIST_PARSING_RESULT_HPP

#include "ast/declaration/VnlcImportDeclarationItem.hpp"
#include <memory>
#include <vector>

struct VnlcImportPathListParsingResult {
    std::vector<std::unique_ptr<VnlcImportDeclarationItem>> paths;
};

#endif // VNLC_IMPORT_PATH_LIST_PARSING_RESULT_HPP
