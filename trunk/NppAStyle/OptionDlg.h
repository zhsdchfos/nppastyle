#ifndef _OPTION_DLG_H_
#define _OPTION_DLG_H_

#include "StaticDialog.h"
#include "PluginInterface.h"


class OptionDlg : public StaticDialog
{

public:
	OptionDlg() : StaticDialog() {};

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
	/* Handles */
	NppData			_nppData;
	HWND			_HSource;
};



#endif
