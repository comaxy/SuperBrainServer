#include "Logger.h"
#include "log4cplus/configurator.h"
#include <Windows.h>
#include <atlstr.h>

bool Logger::initialize()
{
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CreateDirectory(exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("logs\\"), NULL);

	log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log.properties"));
	m_logger = log4cplus::Logger::getRoot();

	return true;
}

void Logger::uninitialize()
{

}
