#include "common/debug/logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>

std::shared_ptr<spdlog::logger> Log::coreLogger;

void Log::initialize() {
	// NOTE(rob): Create a basic console logger and a file logger
	std::vector<spdlog::sink_ptr> logSinks;
	logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/app.log", true));

	logSinks[0]->set_pattern("%^[%T.%e] [%l] %v%$");
	logSinks[1]->set_pattern("[%T.%e] [%l] %v");
	
	coreLogger = std::make_shared<spdlog::logger>("coreLogger", std::begin(logSinks), std::end(logSinks));
	coreLogger->set_level(spdlog::level::trace);
	coreLogger->flush_on(spdlog::level::trace);
	coreLogger->set_error_handler(errorHandler);
	spdlog::register_logger(coreLogger);
}

void Log::errorHandler(const std::string& message) {
	std::cout << "[spdlog] Error: " << message << std::endl;
}
