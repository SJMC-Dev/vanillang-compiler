#ifndef VNLC_CLASS_DECLARATION_PARSING_RESULT_HPP
#define VNLC_CLASS_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ClassDeclarationParsingResult {
        std::unique_ptr<ClassDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_PARSING_RESULT_HPP
