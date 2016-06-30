#pragma once

#include "UIlib.h"

class MainWindow 
	: public DuiLib::CWindowWnd
	, public DuiLib::INotifyUI
	, public DuiLib::IDialogBuilderCallback
{
public:
	MainWindow();
	virtual void Notify(DuiLib::TNotifyUI& msg) override;
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass) override;

protected:
	virtual LPCTSTR GetWindowClassName() const override { return TEXT("MainWindow"); }
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnMsgCreate(WPARAM wParam, LPARAM lParam);
	void OnMsgSocket(WPARAM wParam, LPARAM lParam);
	void OnMsgSocketAccept(SOCKET sock);
	void OnMsgSocketRead(SOCKET sock);
	void OnMsgSocketClose(SOCKET sock);

private:
	bool OnNotifyWindowInit();

private:
	DuiLib::CPaintManagerUI m_pm;
	SOCKET m_hLstnSock;
};