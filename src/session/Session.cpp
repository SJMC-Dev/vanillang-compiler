#include "Session.hpp"
#include "config/Config.hpp"
#include "lexer/Lexer.hpp"
#include "log/Logger.hpp"
#include "parser/Parser.hpp"
#include <fstream>

namespace vnlc {
    Session::Session(Config&& config) : config(config), ast(nullptr), imports() {}

    void Session::run() {
        VNLC_LOG_INFO("Session started.");

        std::ifstream input(config.inputFilePath);
        Lexer lexer(input);
        Parser parser(std::move(lexer));
        ast = parser.parse(config);
    }
} // namespace vnlc
