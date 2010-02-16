#ifndef _ABOUT_DLG_H_
#define _ABOUT_DLG_H_

#include "StaticDialog.h"
#include "PluginInterface.h"


class AboutDlg : public StaticDialog
{

public:
	AboutDlg() : StaticDialog() {};

	void init(HINSTANCE hInst, NppData nppData)
	{
		_nppData = nppData;
		Window::init(hInst, nppData._nppHandle);
	};

	void doDialog();

	virtual void destroy() {
	};


protected :
	virtual BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	NppData			_nppData;
	HWND			_HSource;
};



#endif
