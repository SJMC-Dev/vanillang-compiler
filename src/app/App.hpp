#ifndef VNLC_APP_HPP
#define VNLC_APP_HPP

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

namespace vnlc {
    class App {
    private:
        CLI::App app;

        int argc;
        char** argv;

        App() = delete;
        App(const App&) = delete;
        App(App&&) noexcept = delete;

        App& operator=(const App&) = delete;

    public:
        App(int argc, char** argv);

        void run();
    };
} // namespace vnlc

#endif // VNLC_APP_HPP