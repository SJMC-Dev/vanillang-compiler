#ifndef VNLC_DICT_ENTRY_PARSING_RESULT_HPP
#define VNLC_DICT_ENTRY_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>
#include <string>

struct VnlcDictEntryParsingResult {
    std::string key;
    std::unique_ptr<VnlcExpressionNode> value;
};

#endif // VNLC_DICT_ENTRY_PARSING_RESULT_HPP
