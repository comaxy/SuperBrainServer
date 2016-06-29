#include "DuiManager.h"
#include "UIlib.h"
#include <atlstr.h>

bool DuiManager::initialize(HINSTANCE hInstance)
{
	// 设置资源路径
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CString resourcePath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("skin\\");
	DuiLib::CPaintManagerUI::SetResourcePath(static_cast<LPCTSTR>(resourcePath));

	return true;
}

void DuiManager::uninitialize()
{

}
