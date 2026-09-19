#ifndef VNLC_DICT_ENTRY_PARSING_RESULT_HPP
#define VNLC_DICT_ENTRY_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <string>

namespace vnlc {
    struct DictEntryParsingResult {
        std::string key;
        std::unique_ptr<ExpressionNode> value;
    };
} // namespace vnlc

#endif // VNLC_DICT_ENTRY_PARSING_RESULT_HPP
