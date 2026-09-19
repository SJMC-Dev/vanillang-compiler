#include "app/App.hpp"
#include "log/Logger.hpp"

int main(int argc, char** argv) {
    try {
        vnlc::App app{ argc, argv };
        app.run();
    } catch (const std::exception& e) {
        VNLC_LOG_FATAL(e.what());
    }

    return 0;
}