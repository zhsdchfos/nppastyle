#include "PluginDefinition.h"
#include "AboutDlg.h"
#include "resource.h"


void AboutDlg::doDialog()
{
	if (!isCreated())
		create(DLG_ABOUT);

	goToCenter();
}


BOOL CALLBACK AboutDlg::run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
	case WM_INITDIALOG :
		{
			return TRUE;
		}
	case WM_COMMAND : 
		{
			switch (wParam)
			{
			case IDOK :
			case IDCANCEL :
				display(FALSE);
				return TRUE;

			default :
				break;
			}
		}
	}
	return FALSE;
}

