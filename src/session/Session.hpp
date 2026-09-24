#ifndef VNLC_SESSION_HPP
#define VNLC_SESSION_HPP

#include "ast/AstNode.hpp"
#include "config/Config.hpp"
#include "vni/import/ImportedItem.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vnlc {
    class Session {
    private:
        const Config config;
        std::unique_ptr<AstNode> ast;
        std::unordered_map<std::string, std::unique_ptr<ImportedItem>> imports;

    public:
        Session(Config&& config);

        void run();
    };
} // namespace vnlc

#endif // VNLC_SESSION_HPP