#ifndef VNLC_TYPE_REFERENCE_NODE_UTIL_HPP
#define VNLC_TYPE_REFERENCE_NODE_UTIL_HPP

#include "ast/typeref/TypeReferenceNode.hpp"
#include <string>

namespace vnlc {
    namespace TypeReferenceNodeUtil {
        [[nodiscard]] std::string generateNamespaceIdFromTypeName(const TypeReferenceNode& typeNode);
    } // namespace TypeReferenceNodeUtil
} // namespace vnlc

#endif // VNLC_TYPE_REFERENCE_NODE_UTIL_HPP
