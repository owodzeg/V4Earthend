#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 

#include <filesystem>
#include <sstream>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "Engine/CoreManager.h"

namespace fs = std::filesystem;

#ifdef __linux__
#include <X11/Xlib.h>
void setupPlatform() {
    XInitThreads();
}
#else
void setupPlatform() {}
#endif

int main(int argc, char *argv[])
{
    // we store the args in a simple vector
    std::vector<std::string> cmd_args;

    // fetch cmdline arguments
    if (argc > 1)
    {
        // read them from argv
        for(int i=0; i<argc; i++)
            cmd_args.emplace_back(argv[i]);
    }

    // Init spdlog and set base log level
    spdlog::cfg::load_env_levels();
    spdlog::set_level(spdlog::level::trace);

    // get current date for log file (there's probably a better way for that)
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");

    // Create a multi-sink logger to output for console and the logfile
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    // Combine them and set as default
    auto combined_logger = std::make_shared<spdlog::logger>("patalogger", begin(sinks), end(sinks));

    // Setting custom log pattern, example:
    // [30-07-2021 16:01:24] [info] [main.cpp: main:42] log message
    combined_logger->set_pattern("[%d-%m-%Y %H:%M:%S] [%^%l%$] [%s: %!:%#] %v");
    combined_logger->set_level(spdlog::level::trace);

    // Set the logger as default logger
    spdlog::register_logger(combined_logger);
    spdlog::set_default_logger(combined_logger);

    // note i'm using SPDLOG_x macros, its because they allow to log cpp file, function and line (regular spdlog::x doesnt do that)
    SPDLOG_INFO("Starting V4Earthend v{}.", std::string(PATAFOUR_VERSION));
    SPDLOG_INFO("SFML version: {}.{}.{}", SFML_VERSION_MAJOR, SFML_VERSION_MINOR, SFML_VERSION_PATCH);

    // additional initialization for other platforms
    setupPlatform();

    // change working directory to where the executable is
    auto executable_path = weakly_canonical(fs::path(argv[0])).parent_path();
    fs::current_path(executable_path);
    SPDLOG_INFO("Executable directory: {}", executable_path.string());

    // initialize CoreManager and the main V4Earthend class
    CoreManager::getInstance().init();
    CoreManager::getInstance().getCore()->init(cmd_args);

    return 0;
}