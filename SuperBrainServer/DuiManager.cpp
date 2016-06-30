#include "DuiManager.h"
#include "UIlib.h"
#include "LoggerDef.h"
#include <atlstr.h>

bool DuiManager::initialize(HINSTANCE hInstance)
{
	appLogger()->trace("Initializing DUI manager.");

	// 设置资源路径
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CString resourcePath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("skin\\");
	DuiLib::CPaintManagerUI::SetResourcePath(static_cast<LPCTSTR>(resourcePath));

	appLogger()->trace("Initialize DUI manager done.");

	return true;
}

void DuiManager::uninitialize()
{
	appLogger()->trace("Uninitializing DUI manager.");

	appLogger()->trace("Uninitialie DUI manager done.");
}
