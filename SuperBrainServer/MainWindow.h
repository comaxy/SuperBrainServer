#include "UIlib.h"


class MainWindow 
	: public DuiLib::CWindowWnd
	, public DuiLib::INotifyUI
	, public DuiLib::IDialogBuilderCallback
{
public:
	virtual void Notify(DuiLib::TNotifyUI& msg) override;
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass) override;

protected:
	virtual LPCTSTR GetWindowClassName() const override { return TEXT("MainWindow"); }
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnMsgCreate(WPARAM wParam, LPARAM lParam);
	void OnMsgSocket(WPARAM wParam, LPARAM lParam);

private:
	bool OnNotifyWindowInit();

private:
	DuiLib::CPaintManagerUI m_pm;
};