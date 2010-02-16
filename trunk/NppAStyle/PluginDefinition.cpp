//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdio.h>
#include <Shlwapi.h>
#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "AboutDlg.h"
#include "OptionDlg.h"
#include "resource.h"
#include "astyle.h"

//#pragma warning(disable: C4748)
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "AStyleLib.lib")

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;
HINSTANCE hModule;

// ¥∞ø⁄
AboutDlg aboutDlg;
OptionDlg optionDlg;

// ≈‰÷√–≈œ¢
TCHAR configDir[MAX_PATH];	// 
//TCHAR astyleCmd[MAX_PATH * 3];
//LPCTSTR astyleCmdFormat = TEXT("\"%s\\AStyle.exe\" --indent=%s --style=%s < \"%s\" > \"%s\\%s\"");
TCHAR iniFile[MAX_PATH];
TCHAR iniFileName[] = TEXT("NppAStyle.ini");
TCHAR tempFileName[] = TEXT("NppAStyle.txt");
//LPCTSTR codeStyles[codeStyleCount] = {
//	TEXT("none"),
//	TEXT("ansi"),
//	TEXT("java"),
//	TEXT("k&r"),
//	TEXT("linux"),
//	TEXT("gnu"),
//	TEXT("stroustrup"),
//	TEXT("banner"),
//	TEXT("horstmann"),
//	TEXT("whitesmith"),
//	TEXT("ltbs")
//};

LPCTSTR codeStyleSamples[codeStyleCount] = {
	TEXT(""),

	// ansi
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{\r\n")
	TEXT("    if (isBar)\r\n")
	TEXT("    {\r\n")
	TEXT("        bar();\r\n")
	TEXT("        return 1;\r\n")
	TEXT("    }\r\n")
	TEXT("    else\r\n")
	TEXT("        return 0;\r\n")
	TEXT("}"),

	// java
	TEXT("int Foo(bool isBar) {\r\n")
	TEXT("    if (isBar) {\r\n")
	TEXT("        bar();\r\n")
	TEXT("        return 1;\r\n")
	TEXT("    } else\r\n")
	TEXT("        return 0;\r\n")
	TEXT("}"),

	// k&r
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{\r\n")
	TEXT("    if (isBar) {\r\n")
	TEXT("        bar();\r\n")
	TEXT("        return 1;\r\n")
	TEXT("    } else\r\n")
	TEXT("        return 0;\r\n")
	TEXT("}"),

	// linux
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{\r\n")
	TEXT("        if (isBar) {\r\n")
	TEXT("                bar();\r\n")
	TEXT("                return 1;\r\n")
	TEXT("        } else\r\n")
	TEXT("                return 0;\r\n")
	TEXT("}"),

	// gnu
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{\r\n")
	TEXT("  if (isBar)\r\n")
	TEXT("    {\r\n")
	TEXT("      bar();\r\n")
	TEXT("      return 1;\r\n")
	TEXT("    }\r\n")
	TEXT("  else\r\n")
	TEXT("    return 0;\r\n")
	TEXT("}"),

	// stroustrup
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{\r\n")
	TEXT("     if (isBar) {\r\n")
	TEXT("          bar();\r\n")
	TEXT("          return 1;\r\n")
	TEXT("     } else\r\n")
	TEXT("          return 0;\r\n")
	TEXT("}"),

	// banner
	TEXT("int Foo(bool isBar) {\r\n")
	TEXT("    if (isBar) {\r\n")
	TEXT("        bar();\r\n")
	TEXT("        return 1;\r\n")
	TEXT("	      }\r\n")
	TEXT("    else\r\n")
	TEXT("        return 0;\r\n")
	TEXT("    }"),

	// horstmann
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{  if (isBar)\r\n")
	TEXT("   {  bar();\r\n")
	TEXT("      return 1;\r\n")
	TEXT("	 } else\r\n")
	TEXT("      return 0;\r\n")
	TEXT("}"),

	// whitesmith
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("    {\r\n")
	TEXT("    if (isBar)\r\n")
	TEXT("        {")
	TEXT("        bar();\r\n")
	TEXT("        return 1;\r\n")
	TEXT("	      }\r\n")
	TEXT("    else\r\n")
	TEXT("        return 0;\r\n")
	TEXT("    }"),

	// ltbs
	TEXT("int Foo(bool isBar)\r\n")
	TEXT("{\r\n")
	TEXT("    if (isBar) {\r\n")
	TEXT("        bar();\r\n")
	TEXT("        return 1;\r\n")
	TEXT("    } else {\r\n")
	TEXT("        return 0;\r\n")
	TEXT("    }\r\n")
	TEXT("}")
};


INT curCodeStyle = 0;	// 0 <= n < codeStyleCount
INT replaceTabWithSpace = 0;
INT tabSize = defaultTabSize;

TCHAR keySectionName[] = TEXT("NppAStyle");
TCHAR keyCurCodeStyle[] = TEXT("CodeStyle");
TCHAR keyReplaceTabWithSpace[] = TEXT("ReplaceTab");
TCHAR keyTabSize[] = TEXT("TabSize");

void loadConfigInfo()
{
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)configDir);
	if (::PathFileExists(configDir) == FALSE)
	{
		::CreateDirectory(configDir, NULL);
	}

#ifdef UNICODE
	swprintf(iniFile, MAX_PATH, TEXT("%s\\%s"), configDir, iniFileName);
#else
	sprintf(iniFile, TEXT("%s\\%s"), configDir, iniFileName);
#endif

	curCodeStyle = ::GetPrivateProfileInt(keySectionName, keyCurCodeStyle, 0, iniFile);
	replaceTabWithSpace = ::GetPrivateProfileInt(keySectionName, keyReplaceTabWithSpace, 0, iniFile);
	tabSize = ::GetPrivateProfileInt(keySectionName, keyTabSize, defaultTabSize, iniFile);
}

void saveConfigInfo()
{
	if (curCodeStyle >= codeStyleCount)
		curCodeStyle = codeStyleCount - 1;
	if (curCodeStyle < 0)
		curCodeStyle = 0;

	if (tabSize > 20)
		tabSize = 20;
	if (tabSize < 2)
		tabSize = 2;

	const int bufferSize = 3;
	TCHAR buffer[bufferSize];

#ifdef UNICODE
	swprintf(buffer, bufferSize, TEXT("%d"), curCodeStyle);
	::WritePrivateProfileString(keySectionName, keyCurCodeStyle, buffer, iniFile);
	swprintf(buffer, bufferSize, TEXT("%d"), tabSize);
	::WritePrivateProfileString(keySectionName, keyTabSize, buffer, iniFile);
#else
	sprintf(buffer, TEXT("%d"), curCodeStyle);
	::WritePrivateProfileString(keySectionName, keyCurCodeStyle, buffer, iniFile);
	sprintf(buffer, TEXT("%d"), tabSize);
	::WritePrivateProfileString(keySectionName, keyTabSize, buffer, iniFile);
#endif
	::WritePrivateProfileString(keySectionName, keyReplaceTabWithSpace, replaceTabWithSpace ? TEXT("1") : TEXT("0"), iniFile);
}



//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	::hModule = (HINSTANCE)hModule;
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );

	loadConfigInfo();

	extern void formatCode();
	extern void openOptionDlg();
	extern void openAboutDlg();

	// Shortcut :
	// Following code makes the first command
	// bind to the shortcut Alt-F
	ShortcutKey *shKey = new ShortcutKey;
	shKey->_isAlt = true;
	shKey->_isCtrl = false;
	shKey->_isShift = false;
	shKey->_key = 0x46; //VK_F

    setCommand(0, TEXT("Format Code"), formatCode, shKey, false);
	setCommand(1, TEXT("Options ..."), openOptionDlg, NULL, false);
	setCommand(2, TEXT("---"), NULL, NULL, false);
	setCommand(3, TEXT("About ..."), openAboutDlg, NULL, false);

	aboutDlg.init((HINSTANCE)hModule, nppData);
	optionDlg.init((HINSTANCE)hModule, nppData);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
	delete funcItem[0]._pShKey;
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

static void STDCALL formatFailedCallback(int, char *errStr)
{
	::MessageBoxA(NULL, errStr, "NppAStyle Message", 0);
}

static char * STDCALL allocMem(unsigned long size)
{
	return (char *)::malloc(size);
}

static char *codeStylesA[codeStyleCount] = {
	"none",
	"ansi",
	"java",
	"k&r",
	"linux",
	"gnu",
	"stroustrup",
	"banner",
	"horstmann",
	"whitesmith",
	"otbs"
};

void formatCode()
{
	if (curCodeStyle == 0)
		return;

	int which = -1;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return;
	HWND curScintilla = (which == 0)
		? nppData._scintillaMainHandle 
		: nppData._scintillaSecondHandle;

	int langType;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&langType);
	if (!(langType == L_JAVA || langType == L_CS || langType == L_C || langType == L_CPP))
	{
		MessageBox(NULL, TEXT("Astyle only support C, C++, C# and Java"), TEXT("NppAStyle Message"), 0);
		return;
	}

	int textSize = (int)::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0);
	textSize += 1;
	char *textBuffer = (char *)::malloc(textSize);
	::SendMessage(curScintilla, SCI_GETTEXT, (WPARAM)textSize, (LPARAM)textBuffer);
	
	char *styleFmtUseTab = "indent=tab style=%s";
	char *styleFmtUseSpace = "indent=spaces=%d style=%s";
	char option[50];
	if (replaceTabWithSpace)
		sprintf(option, styleFmtUseSpace, tabSize, codeStylesA[curCodeStyle]);
	else
		sprintf(option, styleFmtUseTab, codeStylesA[curCodeStyle]);
	char *formattedCode = AStyleMain(textBuffer, option, formatFailedCallback, allocMem);
	::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)formattedCode);
	
	::free(textBuffer);
	::free(formattedCode);
}

void openOptionDlg()
{
	optionDlg.doDialog();
}

void openAboutDlg()
{
	aboutDlg.doDialog();
}