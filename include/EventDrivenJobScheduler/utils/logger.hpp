#pragma once

#include <memory>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class Logger {
public:
    static void Init()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs.txt", true);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        s_logger = std::make_shared<spdlog::logger>("MainLogger", begin(sinks), end(sinks));
        spdlog::register_logger(s_logger);
        s_logger->set_level(spdlog::level::debug);
        s_logger->flush_on(spdlog::level::info);
    }

    static std::shared_ptr<spdlog::logger>& Get() {return s_logger;};

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};