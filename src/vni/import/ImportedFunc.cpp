#include "ImportedFunc.hpp"

namespace vnlc {
    ImportedFunc::ImportedFunc(
        std::string_view name,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
        bool native,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          returnType(returnType),
          parameters(std::move(parameters)),
          native(native) {}

    ImportedFunc::ImportedFunc(std::string_view name, std::string_view returnType, std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters, bool native)
        : ImportedIdentifier(name),
          returnType(returnType),
          parameters(std::move(parameters)),
          native(native) {}

    std::string_view ImportedFunc::getReturnType() const {
        return returnType;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& ImportedFunc::getParameters() const {
        return parameters;
    }

    bool ImportedFunc::isNative() const {
        return native;
    }

    const ImportedParameter* ImportedFunc::getParameterByName(std::string_view name) const {
        auto it = parameters.find(std::string(name));
        if (it != parameters.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
