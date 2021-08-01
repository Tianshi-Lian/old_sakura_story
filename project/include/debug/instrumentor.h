#pragma once

// Credit: TheCherno
// https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Debug/Instrumentor.h

#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

#include "logger.h"

struct Profile_Result {
	std::string name;

	std::chrono::duration<double, std::micro> start;
	std::chrono::microseconds elapsedTime;
	std::thread::id threadID;
};

struct Instrumentation_Session {
	std::string name;
};

class Instrumentor {
public:
	Instrumentor(const Instrumentor&) = delete;
	Instrumentor(Instrumentor&&) = delete;

	void beginSession(const std::string& name, const std::string& filepath = "results.json") {
		std::lock_guard lock(m_mutex);
		if (m_currentSession) {
			Log::error("Instrumentor::beginSession('{0}') when session '{1}' already open.", name, m_currentSession->name);
			internalEndSession();
		}
		m_outputStream.open(filepath);

		if (m_outputStream.is_open()) {
			m_currentSession = new Instrumentation_Session({name});
			writeHeader();
		}
		else {
				Log::error("Instrumentor could not open results file '{0}'.", filepath);
		}
	}

	void endSession() {
		std::lock_guard lock(m_mutex);
		internalEndSession();
	}

	void writeProfile(const Profile_Result& result) {
		std::stringstream json;

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.elapsedTime.count()) << ',';
		json << "\"name\":\"" << result.name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.threadID << ",";
		json << "\"ts\":" << result.start.count();
		json << "}";

		std::lock_guard lock(m_mutex);
		if (m_currentSession) {
			m_outputStream << json.str();
			m_outputStream.flush();
		}
	}

	static Instrumentor& get() {
		static Instrumentor instance;
		return instance;
	}

private:
	Instrumentor()
		: m_currentSession(nullptr) {
	}

	~Instrumentor() {
		endSession();
	}		

	void writeHeader() {
		m_outputStream << R"({"otherData": {},"traceEvents":[{})";
		m_outputStream.flush();
	}

	void writeFooter() {
		m_outputStream << "]}";
		m_outputStream.flush();
	}

	// Note: you must already own lock on m_mutex before
	// calling internalEndSession()
	void internalEndSession() {
		if (m_currentSession) {
			writeFooter();
			m_outputStream.close();
			delete m_currentSession;
			m_currentSession = nullptr;
		}
	}

	std::mutex m_mutex;
	Instrumentation_Session* m_currentSession;
	std::ofstream m_outputStream;
};

class Instrumentation_Timer
{
public:
	Instrumentation_Timer(const char* name) 
		: m_mame(name), m_Stopped(false) {
		m_startTimepoint = std::chrono::steady_clock::now();
	}

	~Instrumentation_Timer() {
		if (!m_Stopped) {
			Stop();
		}
	}

	void Stop() {
		auto endTimepoint = std::chrono::steady_clock::now();
		auto highResStart = std::chrono::duration<double, std::micro>{ m_startTimepoint.time_since_epoch() };
		auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch();

		Instrumentor::get().writeProfile({ m_mame, highResStart, elapsedTime, std::this_thread::get_id() });

		m_Stopped = true;
	}

private:
	const char* m_mame;
	std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
	bool m_Stopped;
};

template <size_t N>
struct Change_Result {
	char Data[N];
};

template <size_t N, size_t K>
constexpr auto cleanupOutputString(const char(&expr)[N], const char(&remove)[K]){
	Change_Result<N> result = {};

	size_t srcIndex = 0;
	size_t dstIndex = 0;
	while (srcIndex < N) {
		size_t matchIndex = 0;
		while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
			matchIndex++;
		if (matchIndex == K - 1)
			srcIndex += matchIndex;
		result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
		srcIndex++;
	}
	return result;
}

#if PS_DEBUG
	// Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define PS_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define PS_FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define PS_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define PS_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define PS_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define PS_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define PS_FUNC_SIG __func__
	#else
		#define PS_FUNC_SIG "HZ_FUNC_SIG unknown!"
	#endif

	#define PROFILE_BEGIN_SESSION(name, filepath) ::Instrumentor::get().beginSession(name, filepath)
	#define PROFILE_END_SESSION() ::Instrumentor::get().endSession()
	#define PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::cleanupOutputString(name, "__cdecl ");\
											   ::Instrumentation_Timer timer##line(fixedName##line.Data)
	#define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE2(name, line)
	#define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
	#define PROFILE_FUNCTION() PROFILE_SCOPE(PS_FUNC_SIG)
#else
	#define PROFILE_BEGIN_SESSION(name, filepath)
	#define PROFILE_END_SESSION()
	#define PROFILE_SCOPE(name)
	#define PROFILE_FUNCTION()
#endif