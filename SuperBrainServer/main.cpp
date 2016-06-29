#include <Windows.h>
#include <WinSock.h>
#include <atlstr.h>
#include "UIlib.h"
#include "MainWindow.h"
#include "sqlite/sqlite3.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ��ʼ�� DuiLib
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CString resourcePath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("skin\\");
	DuiLib::CPaintManagerUI::SetResourcePath(static_cast<LPCTSTR>(resourcePath));

	//TCHAR szFileName[MAX_PATH] = { 0 };
	//GetModuleFileName(NULL, szFileName, MAX_PATH);
	//TCHAR resourcePath[MAX_PATH] = { 0 };
	//_tcsncpy_s(resourcePath, MAX_PATH, szFileName, 
	//	_tcsrchr(szFileName, TEXT('\\')) - szFileName + 1);
	//_tcscat_s(resourcePath, MAX_PATH, TEXT("skin\\"));
	//DuiLib::CPaintManagerUI::SetResourcePath(resourcePath);

	// ��ʼ�����ݿ�
	//sqlite3_open()

	// ����������
	MainWindow mainWindow;
	mainWindow.Create(NULL, TEXT("Super Brain"), WS_OVERLAPPED | WS_SYSMENU, 0);
	mainWindow.ShowWindow();
	
	MSG msg = { 0 };
	BOOL bRet;
	while ((bRet = GetMessage(&msg, mainWindow, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			return -1;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
