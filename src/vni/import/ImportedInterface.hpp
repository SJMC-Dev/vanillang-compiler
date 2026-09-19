#ifndef VNLC_IMPORTED_INTERFACE_HPP
#define VNLC_IMPORTED_INTERFACE_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedMethod.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class ImportedInterface : public ImportedIdentifier {
    private:
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;

    public:
        ImportedInterface(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedInterface(std::string_view name, std::vector<std::string>&& genericParameters, std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods);

        [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>& getMethods() const;

        [[nodiscard]] const ImportedMethod* getMethodByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_INTERFACE_HPP
