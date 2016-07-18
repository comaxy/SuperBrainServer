#pragma once

#include "Application.h"
#include "Logger.h"

std::shared_ptr<Logger> appLogger();

#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)

#define LOG_FATAL(...) appLogger()->fatal(__FILENAME__, "(", __LINE__, ") ", __VA_ARGS__)
#define LOG_ERROR(...) appLogger()->error(__FILENAME__, "(", __LINE__, ") ", __VA_ARGS__)
#define LOG_WARN(...) appLogger()->warn(__FILENAME__, "(", __LINE__, ") ", __VA_ARGS__)
#define LOG_DEBUG(...) appLogger()->debug(__FILENAME__, "(", __LINE__, ") ", __VA_ARGS__)
#define LOG_TRACE(...) appLogger()->trace(__FILENAME__, "(", __LINE__, ") ", __VA_ARGS__)
