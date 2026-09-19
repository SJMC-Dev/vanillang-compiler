#include "App.hpp"
#include "config/Config.hpp"
#include "error/IllegalInputError.hpp"
#include "error/PackageNameConflictError.hpp"
#include "log/Logger.hpp"
#include "session/Session.hpp"
#include "util/RunningModeUtil.hpp"
#include <CLI/CLI.hpp>
#include <cctype>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <vector>

namespace vnlc {
    App::App(int argc, char** argv) : argc{ argc }, argv{ argv }, app{ "Vanillang Compiler" } {}

    void App::run() {
        std::string_view mode;
        std::string vanillangVersion;
        std::string minecraftVersion;

        bool verbose = false;

        std::string packageRootPathString;
        std::string inputFilePathString;
        std::string outputDirectoryString;
        std::string moduleInterfaceOutputDirectoryString;
        std::vector<std::string> dependencyPackageRootPathStrings;

        int optimizationLevel = 0;

        app.require_subcommand(1);

        app.add_option("-s,--standard", vanillangVersion, "Vanillang standard version")
            ->default_val("1.0")
            ->check(
                CLI::IsMember(
                    {
                        "1.0",
                    }
                )
            );
        app.add_option("-t,--target", minecraftVersion, "Target Minecraft version")
            ->default_val("26.1.2")
            ->check(
                CLI::IsMember(
                    {
                        "26.1",
                        "26.1.1",
                        "26.1.2",
                    }
                )
            );
        app.add_flag("--verbose", verbose, "Enable verbose output")->default_val(false);

        CLI::App* compileApp = app.add_subcommand("compile", "Compile a Vanillang source file to one or more mcfunction files");
        CLI::App* checkApp = app.add_subcommand("check", "Check a Vanillang source file for errors without generating any output");
        CLI::App* lintApp = app.add_subcommand("lint", "Check a Vanillang source file for style issues without generating any output");
        CLI::App* formatApp = app.add_subcommand("format", "Format a Vanillang source file according to recommended style guidelines");
        CLI::App* dumpAstApp = app.add_subcommand("dump-ast", "Parse a Vanillang source file and generate a JSON representation of the resulting abstract syntax tree");

        auto addCommonOptions = [&](CLI::App* subApp, bool requireOutput = false) {
            subApp->add_option("-i,--input", inputFilePathString, "Path to the input Vanillang source file")->required()->check(CLI::ExistingFile);
            subApp->add_option("-p,--package", packageRootPathString, "Path to the root package")->default_val(std::filesystem::current_path().string())->check(CLI::ExistingDirectory);
            subApp->add_option("-d,--dependencies", dependencyPackageRootPathStrings, "Paths to root packages of dependencies your source code require")->check(CLI::ExistingDirectory);

            if (requireOutput) {
                subApp->add_option("-o,--output", outputDirectoryString, "Path to the output directory")->required()->check(CLI::ExistingDirectory);
            }
            subApp->add_option("-m,--module-interface-output", moduleInterfaceOutputDirectoryString, "Path to the module interface output directory")->check(CLI::ExistingDirectory);
        };

        addCommonOptions(compileApp, true);
        compileApp->add_option("--optimization-level", optimizationLevel, "Optimization level that compiler uses, including 0, 1, 2 and 3")
            ->default_val(0)
            ->check(CLI::IsMember({ 0, 1, 2, 3 }));

        addCommonOptions(checkApp);
        addCommonOptions(lintApp);
        addCommonOptions(formatApp);
        addCommonOptions(dumpAstApp, true);

        compileApp->callback([&]() { mode = "compile"; });
        checkApp->callback([&]() { mode = "check"; });
        lintApp->callback([&]() { mode = "lint"; });
        formatApp->callback([&]() { mode = "format"; });
        dumpAstApp->callback([&]() { mode = "dump-ast"; });

        try {
            app.parse(argc, argv);
        } catch (const CLI::ParseError& e) {
            std::exit(app.exit(e));
        }
        Logger::setVerbose(verbose);

        VNLC_LOG_INFO(fmt::format("Vanillang Compiler started in {} mode.", mode));

        std::filesystem::path inputFilePath = std::filesystem::canonical(inputFilePathString);
        std::filesystem::path outputDirectory = std::filesystem::canonical(outputDirectoryString);
        std::filesystem::path packageRootPath = std::filesystem::canonical(packageRootPathString);

        std::vector<std::filesystem::path> dependencyPackageRootPaths;
        for (auto& dependencyPackageRootPathString : dependencyPackageRootPathStrings) {
            dependencyPackageRootPaths.emplace_back(std::filesystem::canonical(dependencyPackageRootPathString));
        }

        if (!packageRootPath.has_parent_path()) {
            throw IllegalInputError("Package root path cannot be the root path of the file system.");
        }

        if (!inputFilePath.string().starts_with(packageRootPath.string())) {
            throw IllegalInputError("Input file must be within the package root directory.");
        }

        std::string extension = inputFilePath.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
        if (extension == ".vnl") {
            throw IllegalInputError("Input file must end with '.vnl'.");
        }

        Config config{
            .mode = RunningModeUtil::getRunningMode(mode),
            .vanillangVersion{ std::move(vanillangVersion) },
            .minecraftVersion{ std::move(minecraftVersion) },
            .packageRootPath{ std::move(packageRootPath) },
            .inputFilePath{ std::move(inputFilePath) },
            .outputDirectory{ outputDirectory.empty() ? std::nullopt : std::make_optional(std::move(outputDirectory)) },
            .moduleInterfaceOutputDirectory{ moduleInterfaceOutputDirectoryString.empty() ? std::nullopt
                                                                                          : std::make_optional(std::filesystem::canonical(moduleInterfaceOutputDirectoryString)) },
            .dependencyPackageRootPaths{},
            .optimizationLevel = (mode == "compile") ? std::make_optional(optimizationLevel) : std::nullopt,
        };

        std::string packageName = config.packageRootPath.filename().string();
        for (auto& dependencyPackageRootPath : dependencyPackageRootPaths) {
            std::string dependencyName = dependencyPackageRootPath.filename().string();

            if (dependencyName == packageName || config.dependencyPackageRootPaths.contains(dependencyName)) {
                throw PackageNameConflictError(std::move(dependencyName));
            }

            config.dependencyPackageRootPaths.emplace(std::move(dependencyName), std::move(dependencyPackageRootPath));
        }

        Session session{ std::move(config) };
        session.run();
    }
} // namespace vnlc
