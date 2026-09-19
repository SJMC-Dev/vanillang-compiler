#ifndef VNLC_MODULE_PARSING_RESULT_HPP
#define VNLC_MODULE_PARSING_RESULT_HPP

#include "ast/module/ModuleNode.hpp"
#include <memory>

namespace vnlc {
    struct ModuleParsingResult {
        std::unique_ptr<ModuleNode> moduleNode;
    };
} // namespace vnlc

#endif // VNLC_MODULE_PARSING_RESULT_HPP