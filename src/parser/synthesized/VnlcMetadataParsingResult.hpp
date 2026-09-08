#ifndef VNLC_METADATA_PARSING_RESULT_HPP
#define VNLC_METADATA_PARSING_RESULT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcMetadataParsingResult {
    std::vector<VnlcDeclarationItem::MetadataTerm> metadata;
};

#endif // VNLC_METADATA_PARSING_RESULT_HPP
