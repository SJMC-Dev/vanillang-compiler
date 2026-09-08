#ifndef VNLC_MODULE_PARSING_RESULT_HPP
#define VNLC_MODULE_PARSING_RESULT_HPP

#include "ast/module/VnlcModuleNode.hpp"
#include <memory>

struct VnlcModuleParsingResult {
    std::unique_ptr<VnlcModuleNode> moduleNode;
};

#endif // VNLC_MODULE_PARSING_RESULT_HPP