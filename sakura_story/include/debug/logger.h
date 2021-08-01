#pragma once

#include <spdlog/logger.h>

class Log {

public:

	static void initialize();

    template<typename FormatString, typename... Args>
    static void debug(const FormatString &fmt, Args&&...args)
    {
        coreLogger->debug(fmt, std::forward<Args>(args)...);
    }
    template<typename FormatString, typename... Args>
    static void info(const FormatString &fmt, Args&&...args)
    {
        coreLogger->info(fmt, std::forward<Args>(args)...);
    }
    template<typename FormatString, typename... Args>
    static void warn(const FormatString &fmt, Args&&...args)
    {
        coreLogger->warn(fmt, std::forward<Args>(args)...);
    }
    template<typename FormatString, typename... Args>
    static void error(const FormatString &fmt, Args&&...args)
    {
        coreLogger->error(fmt, std::forward<Args>(args)...);
    }
    template<typename FormatString, typename... Args>
    static void critical(const FormatString &fmt, Args&&...args)
    {
        coreLogger->critical(fmt, std::forward<Args>(args)...);
    }

	template<typename T>
	static void debug(const T &msg) {
	    coreLogger->debug(msg);
	}
	template<typename T>
	static void info(const T &msg) {
	    coreLogger->info(msg);
	}
	template<typename T>
	static void warn(const T &msg) {
	    coreLogger->warn(msg);
	}
	template<typename T>
	static void error(const T &msg) {
	    coreLogger->error(msg);
	}
	template<typename T>
	static void critical(const T &msg) {
	    coreLogger->critical(msg);
	}

private:
    static std::shared_ptr<spdlog::logger> coreLogger;
};
