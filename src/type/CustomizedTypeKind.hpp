#ifndef VNLC_CUSTOMIZED_TYPE_KIND_HPP
#define VNLC_CUSTOMIZED_TYPE_KIND_HPP

namespace vnlc {
    enum class CustomizedTypeKind {
        CLASS,
        INTERFACE,
        ENUM,
        ENUM_MEMBER,
        TYPE_ALIAS,
        GENERIC_PARAMETER,
    };
}

#endif // VNLC_CUSTOMIZED_TYPE_KIND_HPP
