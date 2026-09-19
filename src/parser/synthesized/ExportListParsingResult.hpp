#ifndef VNLC_EXPORT_LIST_PARSING_RESULT_HPP
#define VNLC_EXPORT_LIST_PARSING_RESULT_HPP

#include "ast/declaration/ExportDeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct ExportListParsingResult {
        std::vector<ExportDeclarationItem> items;
    };
} // namespace vnlc

#endif // VNLC_EXPORT_LIST_PARSING_RESULT_HPP
