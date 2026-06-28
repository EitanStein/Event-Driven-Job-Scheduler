#pragma once

#include "logger.hpp"

void INIT_LOGGER(std::string filename="") {
    [[maybe_unused]] static bool initialized = [&]() {
        Logger::Init(std::move(filename));
        return true;
    }();
}

#define LOG_TRACE(...) Logger::Get()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::Get()->debug(__VA_ARGS__)
#define LOG_INFO(...)  Logger::Get()->info(__VA_ARGS__)
#define LOG_WARN(...)  Logger::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Get()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::Get()->critical(__VA_ARGS__)