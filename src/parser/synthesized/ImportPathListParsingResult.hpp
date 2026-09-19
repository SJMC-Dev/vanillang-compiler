#ifndef VNLC_IMPORT_PATH_LIST_PARSING_RESULT_HPP
#define VNLC_IMPORT_PATH_LIST_PARSING_RESULT_HPP

#include "ast/declaration/ImportDeclarationItem.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ImportPathListParsingResult {
        std::vector<std::unique_ptr<ImportDeclarationItem>> paths;
    };
} // namespace vnlc

#endif // VNLC_IMPORT_PATH_LIST_PARSING_RESULT_HPP
