#ifndef VNLC_SESSION_HPP
#define VNLC_SESSION_HPP

#include "ast/module/ModuleNode.hpp"
#include "collector/CollectionResult.hpp"
#include "config/Config.hpp"
#include "diagnostic/Diagnostic.hpp"
#include "semantic/SemanticResult.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class Session {
    private:
        const Config config;

        std::optional<CollectionResult> collectionResult;
        std::unique_ptr<ModuleNode> ast;
        std::optional<SemanticResult> semanticAnalysisResult;

    public:
        Session(Config&& config);

        void run();
    };
} // namespace vnlc

#endif // VNLC_SESSION_HPP
