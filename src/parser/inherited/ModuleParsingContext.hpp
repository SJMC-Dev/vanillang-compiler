#ifndef VNLC_MODULE_PARSING_CONTEXT_HPP
#define VNLC_MODULE_PARSING_CONTEXT_HPP

#include "config/Config.hpp"

namespace vnlc {
    struct ModuleParsingContext {
        const Config& config;
    };
} // namespace vnlc

#endif // VNLC_MODULE_PARSING_CONTEXT_HPP