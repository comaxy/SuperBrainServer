#include "LoggerDef.h"

std::shared_ptr<Logger> appLogger()
{
	return Application::sharedInstance()->logger();
}
