
#include "PluginDefinition.h"
#include <stdio.h>
#include "OptionDlg.h"
#include "resource.h"



void OptionDlg::doDialog()
{
	if (!isCreated())
	{
		create(DLG_OPTIONS);
	}

	// 将配置信息绑定到控件
	TCHAR strTabSize[3];
#ifdef UNICODE
	swprintf(strTabSize, 3, TEXT("%d"), tabSize);
#else
	sprintf_s(strTabSize, 3, TEXT("%d"), tabSize);
#endif
	::SetWindowText(::GetDlgItem(_hSelf, TXT_TABSIZE), strTabSize);

	::SendDlgItemMessage(_hSelf, CHK_REPTAB, BM_SETCHECK, replaceTabWithSpace ? TRUE : FALSE, 0);

	for (int i = 0; i < codeStyleCount; ++i)	// 防止在同一时刻有多个radiobutton被选中
		::SendDlgItemMessage(_hSelf, RDO_NONE + i, BM_SETCHECK, (LPARAM)FALSE, 0);
	UINT rdoCodeStyle = RDO_NONE + curCodeStyle;
	::SendDlgItemMessage(_hSelf, rdoCodeStyle, BM_SETCHECK, (LPARAM)TRUE, 0);
	::SetWindowText(::GetDlgItem(_hSelf, TXT_SAMPLE), codeStyleSamples[curCodeStyle]);

	if (rdoCodeStyle == RDO_NONE)
	{
		EnableWindow(GetDlgItem(_hSelf, CHK_REPTAB), FALSE);
		::SendDlgItemMessage(_hSelf, TXT_TABSIZE, EM_SETREADONLY, TRUE, 0);
	}
	else
	{
		EnableWindow(GetDlgItem(_hSelf, CHK_REPTAB), TRUE);
		BOOL enabled = ::SendDlgItemMessage(_hSelf, CHK_REPTAB, BM_GETCHECK, 0, 0) == BST_CHECKED 
			? TRUE 
			: FALSE;
		::SendDlgItemMessage(_hSelf, TXT_TABSIZE, EM_SETREADONLY, !enabled, 0);
	}

	goToCenter();
}


BOOL CALLBACK OptionDlg::run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
	case WM_INITDIALOG :
		{
			return TRUE;
		}
	case WM_COMMAND : 
		{
			// 如果当前没有选中任何样式,使Tab Settings不可用
			if (RDO_NONE <= wParam && wParam < RDO_NONE + codeStyleCount)
			{
				if (RDO_NONE == wParam)
				{
					EnableWindow(::GetDlgItem(_hSelf, CHK_REPTAB), FALSE);
					::SendDlgItemMessage(_hSelf, TXT_TABSIZE, EM_SETREADONLY, (LPARAM)TRUE, 0);
				}
				else
				{
					EnableWindow(::GetDlgItem(_hSelf, CHK_REPTAB), TRUE);
					BOOL enabled = ::SendDlgItemMessage(_hSelf, CHK_REPTAB, BM_GETCHECK, 0, 0) == BST_CHECKED 
						? TRUE 
						: FALSE;
					
					::SendDlgItemMessage(_hSelf, TXT_TABSIZE, EM_SETREADONLY, (LPARAM)!enabled, 0);
				}
			}

			switch (wParam)
			{
			case IDOK :
				replaceTabWithSpace = 
					::SendDlgItemMessage(_hSelf, CHK_REPTAB, BM_GETCHECK, 0, 0) == BST_CHECKED 
						? 1 
						: 0;

				TCHAR strTabSize[4];
				if (::GetDlgItemText(_hSelf, TXT_TABSIZE, strTabSize, 4))
				{
					tabSize = t_atoi(strTabSize);
					if (tabSize < 2)
					{
						MessageBox(NULL, TEXT("Tag size must greater/equal than 2"), TEXT("NppAStyle Message"), 0);
						break;
					}
					else if (tabSize > 20)
					{
						MessageBox(NULL, TEXT("Tag size must less/equal than 20"), TEXT("NppAStyle Message"), 0);
						break;
					}
				}
				else
				{
					tabSize = 4;
				}

				for (int i = 0; i < codeStyleCount; i++)
				{
					if (::SendDlgItemMessage(_hSelf, RDO_NONE + i, BM_GETCHECK, 0, 0) == BST_CHECKED )
					{
						curCodeStyle = i;
						break;
					}
				}

				saveConfigInfo();

				display(FALSE);
				return TRUE;

			case IDCANCEL :
				display(FALSE);
				return TRUE;

			case CHK_REPTAB:
				{
					BOOL enabled = ::SendDlgItemMessage(_hSelf, CHK_REPTAB, BM_GETCHECK, 0, 0) == BST_CHECKED 
						? TRUE 
						: FALSE;
					::SendDlgItemMessage(_hSelf, TXT_TABSIZE, EM_SETREADONLY, (LPARAM)!enabled, 0);
				}
				return TRUE;

			case RDO_NONE:
			case RDO_ANSI:
			case RDO_JAVA:
			case RDO_KR:
			case RDO_LINUX:
			case RDO_GNU:
			case RDO_STROUSTRUP:
			case RDO_BANNER:
			case RDO_HORSTMANN:
			case RDO_WHITESMITH:
			case RDO_LTBS:
				::SetWindowText(::GetDlgItem(_hSelf, TXT_SAMPLE), codeStyleSamples[wParam - RDO_NONE]);
				return TRUE;

			default :
				break;
			}
		}
	}
	return FALSE;
}

