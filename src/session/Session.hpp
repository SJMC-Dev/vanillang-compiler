#ifndef VNLC_SESSION_HPP
#define VNLC_SESSION_HPP

#include "ast/module/ModuleNode.hpp"
#include "config/Config.hpp"
#include "diagnostic/Diagnostic.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class Session {
    private:
        const Config config;
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        std::vector<Diagnostic> collectionErrors;
        std::unique_ptr<ModuleNode> ast;

    public:
        Session(Config&& config);

        void run();
    };
} // namespace vnlc

#endif // VNLC_SESSION_HPP
