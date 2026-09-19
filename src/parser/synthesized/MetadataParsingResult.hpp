#ifndef VNLC_METADATA_PARSING_RESULT_HPP
#define VNLC_METADATA_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct MetadataParsingResult {
        std::vector<DeclarationItem::MetadataTerm> metadata;
    };
} // namespace vnlc

#endif // VNLC_METADATA_PARSING_RESULT_HPP
