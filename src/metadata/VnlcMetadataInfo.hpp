#ifndef VNLC_METADATA_INFO_HPP
#define VNLC_METADATA_INFO_HPP

struct VnlcMetadataInfo {
    static const VnlcMetadataInfo DEFAULT;
    bool noWarnings;
    bool deprecated;
};

#endif // VNLC_METADATA_INFO_HPP