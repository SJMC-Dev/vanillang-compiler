#ifndef VNLC_EXPORT_LIST_PARSING_RESULT_HPP
#define VNLC_EXPORT_LIST_PARSING_RESULT_HPP

#include "ast/declaration/VnlcExportDeclarationItem.hpp"
#include <vector>

struct VnlcExportListParsingResult {
    std::vector<VnlcExportDeclarationItem> items;
};

#endif // VNLC_EXPORT_LIST_PARSING_RESULT_HPP
