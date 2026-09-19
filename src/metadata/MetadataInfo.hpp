#ifndef VNLC_METADATA_INFO_HPP
#define VNLC_METADATA_INFO_HPP

namespace vnlc {
    struct MetadataInfo {
        static const MetadataInfo DEFAULT;
        bool noWarnings;
        bool deprecated;
    };
} // namespace vnlc

#endif // VNLC_METADATA_INFO_HPP