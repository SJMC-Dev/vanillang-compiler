#ifndef VNLC_SCOPE_KIND_HPP
#define VNLC_SCOPE_KIND_HPP

enum class VnlcScopeKind {
    MODULE,
    CLASS,
    INTERFACE,
    ENUM,
    ENUM_MEMBER,
    TYPE_ALIAS,
    FUNCTION,
    BLOCK,
    LOOP,
    SWITCH,
};

#endif // VNLC_SCOPE_KIND_HPP