#ifndef VNLC_METADATA_TERM_PARSING_RESULT_HPP
#define VNLC_METADATA_TERM_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationItem.hpp"

namespace vnlc {
    struct MetadataTermParsingResult {
        DeclarationItem::MetadataTerm term;
    };
} // namespace vnlc

#endif // VNLC_METADATA_TERM_PARSING_RESULT_HPP
