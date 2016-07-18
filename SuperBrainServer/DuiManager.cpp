#include "DuiManager.h"
#include "UIlib.h"
#include "LoggerDef.h"
#include <atlstr.h>

bool DuiManager::initialize(HINSTANCE hInstance)
{
	LOG_TRACE("Initializing DUI manager.");

	// 设置资源路径
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CString resourcePath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("skin\\");
	DuiLib::CPaintManagerUI::SetResourcePath(static_cast<LPCTSTR>(resourcePath));

	LOG_TRACE("Initialize DUI manager done.");

	return true;
}

void DuiManager::uninitialize()
{
	LOG_TRACE("Uninitializing DUI manager.");

	LOG_TRACE("Uninitialie DUI manager done.");
}
