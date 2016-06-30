#pragma once

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

class Logger
{
public:
	bool initialize();
	void uninitialize();

	template <typename... Args>
	void fatal(const Args&... args)
	{
		std::wostringstream strout;
		LOG4CPLUS_FATAL(m_logger, make(strout, args...).str());
	}

	template <typename... Args>
	void error(const Args&... args)
	{
		std::wostringstream strout;
		LOG4CPLUS_ERROR(m_logger, make(strout, args...).str());
	}

	template <typename... Args>
	void warn(const Args&... args)
	{
		std::wostringstream strout;
		LOG4CPLUS_WARN(m_logger, make(strout, args...).str());
	}

	template <typename... Args>
	void info(const Args&... args)
	{
		std::wostringstream strout;
		LOG4CPLUS_INFO(m_logger, make(strout, args...).str());
	}

	template <typename... Args>
	void debug(const Args&... args)
	{
		std::wostringstream strout;
		LOG4CPLUS_DEBUG(m_logger, make(strout, args...).str());
	}

	template <typename... Args>
	void trace(const Args&... args)
	{
		std::wostringstream strout;
		LOG4CPLUS_TRACE(m_logger, make(strout, args...).str());
	}

private:
	template <typename T, typename... Args>
	static std::wostringstream& make(std::wostringstream& strout, const T& t, const Args&... args)
	{
		strout << t;
		return make(strout, args...);
	}

	template <typename T>
	static std::wostringstream& make(std::wostringstream& strout, const T& t)
	{
		strout << t;
		return strout;
	}

private:
	log4cplus::Logger m_logger;
};
