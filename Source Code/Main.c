/*
 * Project 64 - A Nintendo 64 emulator.
 *
 * (c) Copyright 2001 zilmar (zilmar@emulation64.com) and
 * Jabo (jabo@emulation64.com).
 *
 * pj64 homepage: www.pj64.net
 *
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Project64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Project64 or software derived from Project64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <stdio.h>
#include <shellapi.h>
#include "main.h"
#include "cheats.h"
#include "cpu.h"
#include "plugin.h"
#include "Settings.h"
#include "htmlHelp.h"
#include "resource.h"
LARGE_INTEGER Frequency, Frames[8], LastFrame;
BOOL 	AutoStart,
	AutoSleep, DisableRegCaching, UseIni, UseTlb, UseLinking, RomBrowser,
	IgnoreMove, Rercursion, LimitFPS,
	AutoFullScreen, SystemCF, AlwaysOnTop, BasicMode, DelaySI, RememberCheats,
	DelayRSP, DelayRDP, ForceClose;
DWORD CurrentFrame, CPU_Type, SystemCPU_Type, SelfModCheck, SystemSelfModCheck,
	RomsToRemember, RomDirsToRemember;
HWND hMainWindow, hHiddenWin, hStatusWnd;
char CurrentSave[256];
HMENU hMainMenu;
HINSTANCE hInst;
void MenuSetText ( HMENU hMenu, int MenuPos, char * Title, char * ShotCut );
void RomInfo     ( void );
void SetupMenu   ( HWND hWnd );
void UninstallApplication(HWND hWnd);
void SM64settings(HWND hWnd);
void SLOWsettings(HWND hWnd);
void Standardsettings(HWND hWnd);
LRESULT CALLBACK AboutIniBoxProc ( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK Main_Proc       ( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK RomInfoProc     ( HWND, UINT, WPARAM, LPARAM );
void AboutIniBox (void) {
	DialogBox(hInst, MAKEINTRESOURCE(IDD_About_Ini), hMainWindow, (DLGPROC)AboutIniBoxProc);
}
LRESULT CALLBACK AboutIniBoxProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			char * IniFile, String[200],String2[200];
			SetWindowText(hDlg, GS(MENU_ABOUT_INI));
			//Language
			SetDlgItemText(hDlg,IDC_LAN,GS(INI_CURRENT_LANG));
			sprintf(String,"%s: %s",GS(INI_AUTHOR),GS(LANGUAGE_AUTHOR));
			SetDlgItemText(hDlg,IDC_LAN_AUTHOR,String);
			sprintf(String,"%s: %s",GS(INI_VERSION),GS(LANGUAGE_VERSION));
			SetDlgItemText(hDlg,IDC_LAN_VERSION,String);
			sprintf(String,"%s: %s",GS(INI_DATE),GS(LANGUAGE_DATE));
			SetDlgItemText(hDlg,IDC_LAN_DATE,String);
			//RDB
			IniFile = GetIniFileName();
			SetDlgItemText(hDlg,IDC_RDB,GS(INI_CURRENT_RDB));
			_GetPrivateProfileString("Meta","Author","",String,sizeof(String),IniFile);
			if (strlen(String) == 0) {
				EnableWindow(GetDlgItem(hDlg,IDC_RDB),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_RDB_AUTHOR),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_RDB_VERSION),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_RDB_DATE),FALSE);
			}
			sprintf(String2,"%s: %s",GS(INI_AUTHOR),String);
			SetDlgItemText(hDlg,IDC_RDB_AUTHOR,String2);
			_GetPrivateProfileString("Meta","Version","",String,sizeof(String),IniFile);
			sprintf(String2,"%s: %s",GS(INI_VERSION),String);
			SetDlgItemText(hDlg,IDC_RDB_VERSION,String2);
			_GetPrivateProfileString("Meta","Date","",String,sizeof(String),IniFile);
			sprintf(String2,"%s: %s",GS(INI_DATE),String);
			SetDlgItemText(hDlg,IDC_RDB_DATE,String2);
			//Cheat
			SetDlgItemText(hDlg,IDC_CHT,GS(INI_CURRENT_CHT));
			IniFile = GetCheatIniFileName();
			_GetPrivateProfileString("Meta","Author","",String,sizeof(String),IniFile);
			if (strlen(String) == 0) {
				EnableWindow(GetDlgItem(hDlg,IDC_CHT),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_CHT_AUTHOR),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_CHT_VERSION),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_CHT_DATE),FALSE);
			}
			sprintf(String2,"%s: %s",GS(INI_AUTHOR),String);
			SetDlgItemText(hDlg,IDC_CHT_AUTHOR,String2);
			_GetPrivateProfileString("Meta","Version","",String,sizeof(String),IniFile);
			sprintf(String2,"%s: %s",GS(INI_VERSION),String);
			SetDlgItemText(hDlg,IDC_CHT_VERSION,String2);
			_GetPrivateProfileString("Meta","Date","",String,sizeof(String),IniFile);
			sprintf(String2,"%s: %s",GS(INI_DATE),String);
			SetDlgItemText(hDlg,IDC_CHT_DATE,String2);
		}
		break;
	case WM_COMMAND: {
			switch (LOWORD(wParam)) {
			case IDOK:
			case IDCANCEL:
				EndDialog(hDlg,0);
				break;
			}
	}
	default:
		return FALSE;
	}
	return TRUE;
}
void AlwaysOnTopWindow(HWND hWnd) {
	SetWindowPos(hWnd,(AlwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST),0,0,0,0,SWP_NOMOVE|SWP_NOREPOSITION|SWP_NOSIZE);
}
DWORD AsciiToHex (char * HexValue) {
	DWORD Count, Finish, Value = 0;
	Finish = strlen(HexValue);
	if (Finish > 8 ) { Finish = 8; }
	for (Count = 0; Count < Finish; Count++){
		Value = (Value << 4);
		switch( HexValue[Count] ) {
		case '0': break;
		case '1': Value += 1; break;
		case '2': Value += 2; break;
		case '3': Value += 3; break;
		case '4': Value += 4; break;
		case '5': Value += 5; break;
		case '6': Value += 6; break;
		case '7': Value += 7; break;
		case '8': Value += 8; break;
		case '9': Value += 9; break;
		case 'A': case 'a': Value += 10; break;
		case 'B': case 'b': Value += 11; break;
		case 'C': case 'c': Value += 12; break;
		case 'D': case 'd': Value += 13; break;
		case 'E': case 'e': Value += 14; break;
		case 'F': case 'f': Value += 15; break;
		default:
			Value = (Value >> 4);
			Count = Finish;
		}
	}
	return Value;
}
void ChangeWinSize ( HWND hWnd, long width, long height, HWND hStatusBar ) {
	WINDOWPLACEMENT wndpl;
    RECT rc1, swrect;
	wndpl.length = sizeof(wndpl);
	GetWindowPlacement( hWnd, &wndpl);
	if ( hStatusBar != NULL ) {
		GetClientRect( hStatusBar, &swrect );
	    SetRect( &rc1, 0, 0, width, height + swrect.bottom );
	} else {
	    SetRect( &rc1, 0, 0, width, height );
	}
    AdjustWindowRectEx( &rc1,GetWindowLong( hWnd, GWL_STYLE ),
		GetMenu( hWnd ) != NULL, GetWindowLong( hWnd, GWL_EXSTYLE ) );
    MoveWindow( hWnd, wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top,
		rc1.right - rc1.left, rc1.bottom - rc1.top, TRUE );
}
void __cdecl DisplayError (char * Message, ...) {
	char Msg[1000];
	va_list ap;
	if (inFullScreen) { return; }
	va_start( ap, Message );
	vsprintf( Msg, Message, ap );
	va_end( ap );
	MessageBox(NULL,Msg,GS(MSG_MSGBOX_TITLE),MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
	SetActiveWindow(hMainWindow);
}
void __cdecl DisplayErrorFatal (char * Message, ...) {
	char Msg[1000];
	va_list ap;
	if (inFullScreen) { return; }
	va_start( ap, Message );
	vsprintf( Msg, Message, ap );
	va_end( ap );
	MessageBox(NULL,Msg,GS(MSG_MSGBOX_TITLE),MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
	SetActiveWindow(hMainWindow);
}
void DisplayFPS (void) {
	if (CurrentFrame > (8 << 3)) {
		LARGE_INTEGER Total;
		char Message[100];
		int count;
		Total.QuadPart = 0;
		for (count = 0; count < 8; count ++) {
			Total.QuadPart += Frames[count].QuadPart;
		}
		sprintf(Message, "%s: ~ %.0f",GS(FPS_DISPLAY), Frequency.QuadPart/ ((double)Total.QuadPart / (8 << 3)));
		SendMessage( hStatusWnd, SB_SETTEXT, 1, (LPARAM)Message );
	}
}
void FixMenuLang (HMENU hMenu) {
	HMENU hSubMenu;
	MenuSetText(hMenu, 0, GS(MENU_FILE), NULL);
	MenuSetText(hMenu, 1, GS(MENU_SYSTEM), NULL);
	MenuSetText(hMenu, 2, GS(MENU_OPTIONS), NULL);
	MenuSetText(hMenu, 3, GS(MENU_HELP), NULL);
	//File
	hSubMenu = GetSubMenu(hMenu,0);
	MenuSetText(hSubMenu, 0, GS(MENU_OPEN), "Ctrl+O");
	MenuSetText(hSubMenu, 1, GS(MENU_ROM_INFO), NULL);
	MenuSetText(hSubMenu, 3, GS(MENU_START), "F11");
	MenuSetText(hSubMenu, 4, GS(MENU_END), "F12");
	MenuSetText(hSubMenu, 6, GS(MENU_LANGUAGE), NULL);
	MenuSetText(hSubMenu, 8, GS(MENU_CHOOSE_ROM), NULL);
	MenuSetText(hSubMenu, 9, GS(MENU_REFRESH), "F5");
	MenuSetText(hSubMenu, 11, GS(MENU_RECENT_ROM), NULL);
	MenuSetText(hSubMenu, 12, GS(MENU_RECENT_DIR), NULL);
	MenuSetText(hSubMenu, 14, GS(MENU_EXIT), "Alt+F4");
	//System
	hSubMenu = GetSubMenu(hMenu,1);
	MenuSetText(hSubMenu, 0, GS(MENU_RESET),"F1");
	MenuSetText(hSubMenu, 1, GS(CPU_Paused?MENU_RESUME:MENU_PAUSE),"F2/Shift+Caps Lock");
	MenuSetText(hSubMenu, 2, GS(MENU_ADVANCE),"Caps Lock");
        MenuSetText(hSubMenu, 3, GS(MENU_BITMAP),"F3");
	MenuSetText(hSubMenu, 5, GS(MENU_LIMIT_FPS),"F4");
	MenuSetText(hSubMenu, 7, GS(MENU_SAVE),"F5");
	MenuSetText(hSubMenu, 8, GS(MENU_SAVE_AS),"Ctrl+S");
	MenuSetText(hSubMenu, 9, GS(MENU_RESTORE),"F7");
	MenuSetText(hSubMenu, 10, GS(MENU_LOAD),"Ctrl+L");
	MenuSetText(hSubMenu, 12, GS(MENU_CURRENT_SAVE),NULL);
	MenuSetText(hSubMenu, 14, GS(MENU_CHEAT),"Ctrl+C");
	MenuSetText(hSubMenu, 15, GS(MENU_GS_BUTTON),"F9");
	//Options
	hSubMenu = GetSubMenu(hMenu,2);
	MenuSetText(hSubMenu, 0, GS(MENU_FULL_SCREEN), "Esc/Alt+Enter");
	MenuSetText(hSubMenu, 1, GS(MENU_ON_TOP), "Ctrl+A");
	MenuSetText(hSubMenu, 3, GS(MENU_CONFIG_GFX), "Ctrl+V");
	MenuSetText(hSubMenu, 4, GS(MENU_CONFIG_AUDIO), "Ctrl+B");
	MenuSetText(hSubMenu, 5, GS(MENU_CONFIG_CTRL), "Ctrl+D");
	MenuSetText(hSubMenu, 6, GS(MENU_CONFIG_RSP), "Ctrl+R");
	MenuSetText(hSubMenu, 8, GS(REGISTRY), NULL);
	MenuSetText(hSubMenu, 10, GS(MENU_SETTINGS), "Ctrl+T");
	//Registry
	hSubMenu = GetSubMenu(hMenu, 2);
	hSubMenu = GetSubMenu(hSubMenu, 8);
	MenuSetText(hSubMenu, 0, GS(MENU_UNINSTALL), "Shift+F1");
	MenuSetText(hSubMenu, 1, GS(MENU_STANDARD), "Shift+F2");
	MenuSetText(hSubMenu, 2, GS(MENU_SM64), "Shift+F3");
	MenuSetText(hSubMenu, 3, GS(MENU_SLOW), "Shift+F4");
	MenuSetText(hSubMenu, 5, GS(OPEN_REGEDIT), "Shift+R");
	//Help Menu
	hSubMenu = GetSubMenu(hMenu,3);
	MenuSetText(hSubMenu, 0, GS(MENU_USER_GUIDE), NULL);
	MenuSetText(hSubMenu, 1, GS(MENU_ABOUT_INI), NULL);
}
char * GetIniFileName(void) {
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	static char IniFileName[_MAX_PATH];
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath( path_buffer, drive, dir, fname, ext );
	sprintf(IniFileName,"%s%sPJ64DB\\%s",drive,dir,IniName);
	return IniFileName;
}
char * GetLangFileName(void) {
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	static char IniFileName[_MAX_PATH];
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath( path_buffer, drive, dir, fname, ext );
	sprintf(IniFileName,"%s%s%s",drive,dir,LangFileName);
	return IniFileName;
}
int GetStoredWinPos( char * WinName, DWORD * X, DWORD * Y ) {
	long lResult;
	HKEY hKeyResults = 0;
	char String[200];
	sprintf(String,"N64 Software\\%s\\Page Setup",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Value, Bytes = 4;
		sprintf(String,"%s Top",WinName);
		lResult = RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)(&Value),&Bytes);
		if (Type == REG_DWORD && lResult == ERROR_SUCCESS) {
			*Y = Value;
		} else {
			RegCloseKey(hKeyResults);
			return FALSE;
		}
		sprintf(String,"%s Left",WinName);
		lResult = RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)(&Value),&Bytes);
		if (Type == REG_DWORD && lResult == ERROR_SUCCESS) {
			*X = Value;
		} else {
			RegCloseKey(hKeyResults);
			return FALSE;
		}
		RegCloseKey(hKeyResults);
		return TRUE;
	}
	return FALSE;
}
int GetStoredWinSize( char * WinName, DWORD * Width, DWORD * Height ) {
	long lResult;
	HKEY hKeyResults = 0;
	char String[200];
	sprintf(String,"N64 Software\\%s\\Page Setup",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Value, Bytes = 4;
		sprintf(String,"%s Width",WinName);
		lResult = RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)(&Value),&Bytes);
		if (Type == REG_DWORD && lResult == ERROR_SUCCESS) {
			*Width = Value;
		} else {
			RegCloseKey(hKeyResults);
			return FALSE;
		}
		sprintf(String,"%s Height",WinName);
		lResult = RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)(&Value),&Bytes);
		if (Type == REG_DWORD && lResult == ERROR_SUCCESS) {
			*Height = Value;
		} else {
			RegCloseKey(hKeyResults);
			return FALSE;
		}
		RegCloseKey(hKeyResults);
		return TRUE;
	}
	return FALSE;
}
void LoadSettings (void) {
	HKEY hKeyResults;
	char String[200];
	long lResult;
	IgnoreMove = FALSE;
	CPU_Type = Default_CPU;
	SystemCPU_Type = Default_CPU;
	SystemSelfModCheck = Default_SelfModCheck;
	SystemRDRAMsize = Default_RDRAMsize;
	SystemCF = Default_CountPerOp;
	AutoStart = Default_AutoStart;
	AutoSleep = Default_AutoSleep;
	DisableRegCaching = Default_DisableRegCaching;
	UseIni = Default_UseIni;
	AutoFullScreen = FALSE;
	RomsToRemember = Default_RomsToRemember;
	RomDirsToRemember = Default_RomsDirsToRemember;
	LimitFPS = Default_LimitFPS;
	AlwaysOnTop = Default_AlwaysOnTop;
	BasicMode = Default_BasicMode;
	RememberCheats = Default_RememberCheats;
	RomBrowser = Default_UseRB;
	Rercursion = Default_Rercursion;
	sprintf(String,"N64 Software\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"Remember Cheats",0,&Type,(LPBYTE)(&RememberCheats),&Bytes);
		if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { RememberCheats = Default_RememberCheats; }
		lResult = RegQueryValueEx(hKeyResults,"Basic Mode",0,&Type,(LPBYTE)(&BasicMode),&Bytes);
		if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { BasicMode = Default_BasicMode;	}
		lResult = RegQueryValueEx(hKeyResults,"Pause CPU Upon Focus Loss",0,&Type,(BYTE *)(&AutoSleep),&Bytes);
		if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { AutoSleep = Default_AutoSleep; }
		lResult = RegQueryValueEx(hKeyResults,"Enter Fullscreen Mode Upon ROM Opening",0,&Type,(BYTE *)(&AutoFullScreen),&Bytes);
		if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { AutoFullScreen = FALSE; }
		{
			lResult = RegQueryValueEx(hKeyResults,"Limit FPS",0,&Type,(LPBYTE)(&LimitFPS),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { LimitFPS = Default_LimitFPS;	}
			lResult = RegQueryValueEx(hKeyResults,"Max # of ROMs Remembered",0,&Type,(BYTE *)(&RomsToRemember),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { RomsToRemember = Default_RomsToRemember; }
			lResult = RegQueryValueEx(hKeyResults,"Max # of ROM Dirs Remembered",0,&Type,(BYTE *)(&RomDirsToRemember),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { RomDirsToRemember = Default_RomsDirsToRemember; }
			lResult = RegQueryValueEx(hKeyResults,"Start Emulation Upon ROM Opening",0,&Type,(BYTE *)(&AutoStart),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { AutoStart = Default_AutoStart; }
			lResult = RegQueryValueEx(hKeyResults,"ROM Browser",0,&Type,(BYTE *)(&RomBrowser),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { RomBrowser = Default_UseRB; }
			lResult = RegQueryValueEx(hKeyResults,"Directory Recursion",0,&Type,(BYTE *)(&Rercursion),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { Rercursion = Default_Rercursion; }
			lResult = RegQueryValueEx(hKeyResults,"RDB ROM Settings",0,&Type,(BYTE *)(&UseIni),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { UseIni = Default_UseIni; }
			lResult = RegQueryValueEx(hKeyResults,"CPU Core Style",0,&Type,(BYTE *)(&SystemCPU_Type),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { SystemCPU_Type = Default_CPU; }
			lResult = RegQueryValueEx(hKeyResults,"Self-modifying Code Method",0,&Type,(LPBYTE)(&SystemSelfModCheck),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { SystemSelfModCheck = Default_SelfModCheck; }
			lResult = RegQueryValueEx(hKeyResults,"Counter Factor",0,&Type,(LPBYTE)(&SystemCF),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { SystemCF = Default_CountPerOp; }
			lResult = RegQueryValueEx(hKeyResults,"Memory Size",0,&Type,(LPBYTE)(&SystemRDRAMsize),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { SystemRDRAMsize = Default_RDRAMsize; }
			lResult = RegQueryValueEx(hKeyResults,"Always On Top",0,&Type,(LPBYTE)(&AlwaysOnTop),&Bytes);
			if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { AlwaysOnTop = Default_AlwaysOnTop;	}
		}
		RegCloseKey(hKeyResults);
	}
}
int InitalizeApplication ( HINSTANCE hInstance ) {
	INITCOMMONCONTROLSEX IntComStruct;
	HKEY hKeyResults = 0;
	CoInitialize(NULL);
	IntComStruct.dwSize = sizeof(IntComStruct);
	IntComStruct.dwICC = ICC_TREEVIEW_CLASSES;
	InitCommonControls();
	InitCommonControlsEx(&IntComStruct);
	hInst = hInstance;
	{
		char String[200];
		sprintf(String,"N64 Software\\%s",AppName);
		//Language selection if none found
		LoadLanguage(String);
	}
	if (!Allocate_Memory()) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	hPauseMutex = CreateMutex(NULL,FALSE,NULL);
	INITIALIZECPUFlags();
	LoadSettings();
	SetupRegisters(&Registers);
	QueryPerformanceFrequency(&Frequency);
	LoadRomBrowserColoumnInfo ();
	InitilizeInitialCompilerVariable();
	return TRUE;
}
void CheckedMenuItem(UINT uMenuID, BOOL * Flag, char * FlagName) {
	char String[256];
	DWORD Disposition;
	HKEY hKeyResults;
	long lResult;
	UINT uState;
	uState = GetMenuState( hMainMenu, uMenuID, MF_BYCOMMAND);
	hKeyResults = 0;
	Disposition = 0;
	if ( uState & MFS_CHECKED ) {
		CheckMenuItem( hMainMenu, uMenuID, MF_BYCOMMAND | MFS_UNCHECKED );
		*Flag = FALSE;
	} else {
		CheckMenuItem( hMainMenu, uMenuID, MF_BYCOMMAND | MFS_CHECKED );
		*Flag = TRUE;
	}
	sprintf(String,"N64 Software\\%s",AppName);
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER,String,0,"",
		REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueEx(hKeyResults,FlagName,0,REG_DWORD,(BYTE *)Flag,sizeof(DWORD));
	}
	RegCloseKey(hKeyResults);
}
LRESULT CALLBACK Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HMENU hMenu;
	switch (uMsg) {
	case WM_CREATE:
		if ( hHiddenWin ) {
			hStatusWnd = CreateStatusWindow( WS_CHILD | WS_VISIBLE, "", hWnd, 100 );
			SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)"" );
			ChangeWinSize ( hWnd, 640, 480, hStatusWnd );
		}
		break;
	case WM_NCLBUTTONDBLCLK:
		switch (wParam) {
		case HTCAPTION:
			SetRomBrowserMaximized(TRUE);
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		default:
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		case SC_MAXIMIZE:
			SetRomBrowserMaximized(TRUE);
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		default:
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
		break;
	case WM_PAINT:
		if (RomListVisible()) { ValidateRect(hWnd,NULL); break; }
		if (hWnd == hHiddenWin) { ValidateRect(hWnd,NULL); break; }
		__try {
			if (CPU_Paused && DrawScreen != NULL) { DrawScreen(); }
		} __except( r4300i_CPU_MemoryFilter( GetExceptionCode(), GetExceptionInformation()) ) {
			DisplayError(GS(MSG_UNKNOWN_MEM_ACTION));
			ExitThread(0);
		}
		ValidateRect(hWnd,NULL);
		break;
	case WM_MOVE:
		if (IgnoreMove) { break; }
		if (hWnd == hHiddenWin) { break; }
		if (MoveScreen != NULL && CPURunning) {
			MoveScreen((int)(short) LOWORD(lParam), (int)(short) HIWORD(lParam));
		}
		if (IsIconic(hWnd)) {
			if (!CPU_Paused && !inFullScreen) { PauseCpu(); break; }
			break;
		} else {
			if (!ManualPaused && (CPU_Paused || CPU_Action.Pause)) { PauseCpu(); break; }
		}
		if (!RomListVisible() || (RomListVisible() && !IsRomBrowserMaximized())) {
			StoreCurrentWinPos("Main", hWnd );
		}
		break;
	case WM_SIZE:
		if (hWnd == hHiddenWin) { break; }
		{
			RECT clrect, swrect;
			int Parts[2];
			GetClientRect( hWnd, &clrect );
			GetClientRect( hStatusWnd, &swrect );
			Parts[0] = (LOWORD( lParam) - (int)( clrect.right * 0.25 ));
			Parts[1] = LOWORD( lParam);
			SendMessage( hStatusWnd, SB_SETPARTS, 2, (LPARAM)&Parts[0] );
			MoveWindow ( hStatusWnd, 0, clrect.bottom - swrect.bottom,
				LOWORD( lParam ), HIWORD( lParam ), TRUE );
			DisplayFPS();
		}
		ResizeRomListControl(LOWORD(lParam),HIWORD( lParam ));
		if (!IgnoreMove) {
			if (wParam == SIZE_RESTORED && RomListVisible()) {
				SetRomBrowserMaximized(FALSE);
				SetRomBrowserSize(LOWORD(lParam),HIWORD( lParam ));
			}
		}
		break;
	case WM_SETFOCUS:
		if (hWnd == hHiddenWin) { break; }
		if (AutoSleep && !ManualPaused && (CPU_Paused || CPU_Action.Pause)) { PauseCpu(); }
		RomList_SetFocus();
		break;
	case WM_KILLFOCUS:
		if (hWnd == hHiddenWin) { break; }
		if (AutoSleep && !CPU_Paused) { PauseCpu(); }
		break;
	case WM_NOTIFY:
		if (wParam == IDC_ROMLIST) { RomListNotify((LPNMHDR)lParam); }
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
		break;
	case WM_DRAWITEM:
		if (wParam == IDC_ROMLIST) { RomListDrawItem((LPDRAWITEMSTRUCT)lParam); }
		break;
	case WM_MENUSELECT:
		switch (LOWORD(wParam)) {
		case ID_PLAYGAME: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_START)); break;
		case ID_POPUPMENU_ROMINFORMATION: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MSG_RB_INFO)); break;
		case ID_EDITSETTINGS: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_GAME_SETTINGS)); break;
		case ID_EDITCHEATS: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_GAME_CHEATS)); break;
		case ID_FILE_OPEN_ROM: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_OPEN)); break;
		case ID_FILE_ROM_INFO: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_ROM_INFO)); break;
		case ID_FILE_STARTEMULATION: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_START)); break;
		case ID_FILE_ENDEMULATION: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_END)); break;
		case ID_FILE_ROMDIRECTORY: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_CHOOSE_ROM)); break;
		case ID_FILE_REFRESHROMLIST: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_REFRESH)); break;
		case ID_FILE_EXIT: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_EXIT)); break;
		case ID_CPU_RESET: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_RESET)); break;
		case ID_CPU_PAUSE: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_PAUSE)); break;
                case ID_SYSTEM_GENERATEBITMAP: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_BITMAP)); break;
		case ID_SYSTEM_ALTERNATEPAUSE: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENU_ADVANCE_DESC)); break;
		case ID_SYSTEM_LIMITFPS: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_LIMIT_FPS)); break;
		case ID_CPU_SAVE: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_SAVE)); break;
		case ID_CPU_SAVEAS: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_SAVE_AS)); break;
		case ID_CPU_RESTORE: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_RESTORE)); break;
		case ID_CPU_LOAD: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_LOAD)); break;
		case ID_OPTIONS_CHEATS: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_CHEAT)); break;
		case ID_SYSTEM_GSBUTTON: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_GS_BUTTON)); break;
		case ID_OPTIONS_FULLSCREEN: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_FULL_SCREEN)); break;
		case ID_OPTIONS_ALWAYSONTOP: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_ON_TOP)); break;
		case ID_OPTIONS_CONFIG_GFX: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_CONFIG_GFX)); break;
		case ID_OPTIONS_CONFIG_AUDIO: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_CONFIG_AUDIO)); break;
		case ID_OPTIONS_CONFIG_CONTROL: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_CONFIG_CTRL)); break;
		case ID_OPTIONS_CONFIG_RSP: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_CONFIG_RSP)); break;
		case ID_OPTIONS_SETTINGS: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_SETTINGS)); break;
		case ID_HELP_GUIDE: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_USER_GUIDE)); break;
		case ID_REGEDIT: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(REGEDIT_MENUDES)); break;
		case ID_HELP_ABOUTSETTINGFILES: SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_ABOUT_INI)); break;
		case ID_HELP_UNINSTALL: SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_UNINSTALLAPP)); break;
		case ID_HELP_STANDARD: SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_STANDARD)); break;
		case ID_HELP_SM64: SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_SM64)); break;
		case ID_HELP_SLOW: SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_SLOW)); break;
		case ID_CURRENTSAVE_DEFAULT:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_2:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_3:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_4:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_5:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_6:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_7:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_8:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_9:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_10:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_11:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_12:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_13:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_14:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_15:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_16:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_17:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_18:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_19:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_20:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_21:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_22:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_23:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_24:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_25:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_26:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_27:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_28:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_29:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_30:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_31:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_32:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_33:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_34:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_35:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_36:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_37:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_38:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_39:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_40:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_41:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_42:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_43:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_44:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_45:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_46:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_47:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_48:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_49:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		case ID_CURRENTSAVE_50:
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MENUDES_GAME_SLOT));
			break;
		default:
			if (LOWORD(wParam) >= ID_FILE_RECENT_FILE && LOWORD(wParam) <= (ID_FILE_RECENT_FILE + RomsToRemember)) {
				SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_RECENT_ROM));
			} else if (LOWORD(wParam) >= ID_FILE_RECENT_DIR && LOWORD(wParam) <= (ID_FILE_RECENT_DIR + RomDirsToRemember)) {
				SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_RECENT_DIR));
			} else if (LOWORD(wParam) >= ID_LANG_SELECT && LOWORD(wParam) <= (ID_LANG_SELECT + 100)) {
				SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MENUDES_LANGUAGES));
			} else {
				SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)"");
			}
			break;
		}
		break;
	case WM_KEYDOWN:
		if (!CPURunning) { break; }
		if (WM_KeyDown) { WM_KeyDown(wParam, lParam); };
		break;
	case WM_KEYUP:
		if (!CPURunning) { break; }
		if (WM_KeyUp) { WM_KeyUp(wParam, lParam); };
		break;
	case WM_USER + 10:
		if (hWnd == hHiddenWin) { break; }
		if (!wParam) {
			while (ShowCursor(FALSE) >= 0) { Sleep(0); }
		} else {
			while (ShowCursor(TRUE) < 0) { Sleep(0); }
		}
		break;
	case WM_USER + 17:  SetFocus(hMainWindow); break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_PLAYGAME:
			if (strlen(CurrentRBFileName) > 0) {
				DWORD ThreadID;
				strcpy(CurrentFileName,CurrentRBFileName);
				CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)OpenChosenFile,NULL,0, &ThreadID);
			}
			break;
		case ID_EDITSETTINGS: ChangeRomSettings(hWnd); break;
		case ID_EDITCHEATS: ChangeRomCheats(hMainWindow); break;
		case ID_POPUPMENU_ROMINFORMATION:
			{
				char OrigRomName[sizeof(RomName)], OrigFileName[sizeof(CurrentFileName)];
				BYTE OrigByteHeader[sizeof(RomHeader)];
				DWORD OrigFileSize;
				//Make copy of current Game values
				strncpy(OrigRomName,RomName,sizeof(OrigRomName));
				strncpy(OrigFileName,CurrentFileName,sizeof(OrigFileName));
				memcpy(OrigByteHeader,RomHeader,sizeof(RomHeader));
				strncpy(CurrentFileName,CurrentRBFileName,sizeof(CurrentFileName));
				OrigFileSize = RomFileSize;
				//Load header of selected Rom
				LoadRomHeader();
				//Display Information
				RomInfo();
				//Restore settings
				strncpy(RomName,OrigRomName,sizeof(RomName));
				strncpy(CurrentFileName,OrigFileName,sizeof(CurrentFileName));
				memcpy(RomHeader,OrigByteHeader,sizeof(RomHeader));
				RomFileSize = OrigFileSize;
			}
			break;
		case ID_FILE_OPEN_ROM: OpenN64Image(); break;
		case ID_FILE_ROM_INFO: RomInfo(); break;
		case ID_FILE_STARTEMULATION: {
			DWORD ThreadID;
			strcpy(CurrentFileName, LastRoms[0]);
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OpenChosenFile, NULL, 0, &ThreadID);
			}
			break;
		case ID_FILE_ENDEMULATION:
			CloseCpu();
			SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MSG_EMULATION_ENDED));
			hMenu = GetMenu(hMainWindow);
			EnableMenuItem(hMenu,ID_FILE_STARTEMULATION,MFS_ENABLED|MF_BYCOMMAND);
			if (DrawScreen != NULL) { DrawScreen(); }
			CloseCheatWindow();
			CheckRbRefresh();
			break;
		case ID_FILE_ROMDIRECTORY: SelectRomDir(); break;
		case ID_FILE_REFRESHROMLIST: RefreshRomBrowser(); break;
		case ID_FILE_EXIT: DestroyWindow(hWnd);	break;
		case ID_CPU_RESET: {
		char drive[_MAX_DRIVE],dir[_MAX_DIR], fname[_MAX_FNAME],ext[_MAX_EXT];
		char SaveFile[255];
		const char* fixedDir;
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
    		if (osvi.dwMajorVersion >= 6) {
        	// Windows Vista and later
        	fixedDir = "C:\\ProgramData\\";
    		} else {
      		// Windows XP
      		fixedDir = "C:\\Documents and Settings\\All Users\\Application Data\\";
   		}
		_splitpath(SaveFile, drive, dir, fname, ext);
		_makepath(SaveFile, drive, fixedDir, (GS(MSG_EMULATION_STARTED)), "");
		strcpy(LoadFileName,SaveFile);
		CPU_Action.RestoreState = TRUE;
			}
			break;
		case ID_CPU_PAUSE: ManualPaused = TRUE; PauseCpu(); break;
		case ID_CPU_SAVE:
			if (CPU_Paused) {
				if (!Machine_SaveState()) {
					CPU_Action.SaveState = TRUE;
				}
			} else {
				CPU_Action.SaveState = TRUE;
			}
			break;
		case ID_CPU_SAVEAS:
			{
				char drive[_MAX_DRIVE] ,dir[_MAX_DIR], fname[_MAX_FNAME],ext[_MAX_EXT];
				char Directory[255], SaveFile[255];
				OPENFILENAME openfilename;
				memset(&SaveFile, 0, sizeof(SaveFile));
				memset(&openfilename, 0, sizeof(openfilename));
				GetInstantSaveDir( Directory );
				openfilename.lStructSize  = sizeof( openfilename );
				openfilename.hwndOwner    = hMainWindow;
				openfilename.lpstrFilter  = "All files (*.*)\0*\0Slots (*.st)\0*.st\0";
				openfilename.lpstrFile    = SaveFile;
				openfilename.lpstrInitialDir    = Directory;
				openfilename.nMaxFile     = MAX_PATH;
				openfilename.Flags        = OFN_HIDEREADONLY;
				if (GetSaveFileName (&openfilename)) {
					_splitpath( SaveFile, drive, dir, fname, ext );
					if (strcmp(ext, "st") == 0) {
						_makepath( SaveFile, drive, dir, fname, NULL );
					}
					strcpy(SaveAsFileName,SaveFile);
					if (CPU_Paused) {
						if (!Machine_SaveState()) {
							CPU_Action.SaveState = TRUE;
						}
					} else {
						CPU_Action.SaveState = TRUE;
					}
				}
			}
			break;
		case ID_CPU_RESTORE:
			CPU_Action.RestoreState = TRUE;
			break;
		case ID_CPU_LOAD:
			{
				char Directory[255], SaveFile[255];
				OPENFILENAME openfilename;
				memset(&SaveFile, 0, sizeof(SaveFile));
				memset(&openfilename, 0, sizeof(openfilename));
				GetInstantSaveDir( Directory );
				openfilename.lStructSize  = sizeof( openfilename );
				openfilename.hwndOwner    = hMainWindow;
				openfilename.lpstrFilter  = "All files (*.*)\0*\0Slots (*.st)\0*.st\0";
				openfilename.lpstrFile    = SaveFile;
				openfilename.lpstrInitialDir    = Directory;
				openfilename.nMaxFile     = MAX_PATH;
				openfilename.Flags        = OFN_HIDEREADONLY;
				if (GetOpenFileName (&openfilename)) {
					strcpy(LoadFileName,SaveFile);
					CPU_Action.RestoreState = TRUE;
				}
			}
			break;
		case ID_SYSTEM_GENERATEBITMAP:
			if (CaptureScreen) {
				char Directory[255];
				GetSnapShotDir(Directory);
				SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(SCREENSHOT_TAKEN));
				CaptureScreen(Directory);
			}
			break;
		case ID_SYSTEM_LIMITFPS:
			CheckedMenuItem(ID_SYSTEM_LIMITFPS,&LimitFPS,"Limit FPS");
			if (LimitFPS) SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(LIMITFPS_ON)); else SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(LIMITFPS_OFF));
			break;
				case ID_CURRENTSAVE_DEFAULT:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_2:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_3:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_4:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_5:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_6:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_7:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_8:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_9:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_10:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_11:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_12:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_13:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_14:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_15:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_16:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_17:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_18:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_19:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_20:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_21:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_22:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_23:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_24:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_25:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_26:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_27:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_28:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_29:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_30:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_31:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_32:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_33:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_34:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_35:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_36:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_37:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_38:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_39:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_40:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_41:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_42:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_43:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_44:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_45:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_46:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_47:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_48:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_49:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
				case ID_CURRENTSAVE_50:
					SetCurrentSaveState(hWnd, LOWORD(wParam));
					break;
		case ID_SYSTEM_GSBUTTON:
		ApplyGSButton();
		SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(GS_PRESS));
		break;
		case ID_OPTIONS_FULLSCREEN:
			if (CPU_Paused) {
				CPU_Action.ChangeWindow = FALSE;
				if (ChangeWindow) {
					ChangeWindow();
					inFullScreen = !inFullScreen;
					AlwaysOnTopWindow(hMainWindow);
					SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(FULLSCREEN_EXITED));
				}
			} else {
				CPU_Action.ChangeWindow = TRUE;
				SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(FULLSCREEN_ENTERED));
			}
			break;
                case ID_SYSTEM_ALTERNATEPAUSE: {
			if (!CPU_Paused) { PauseCpu (); }
			else { PauseCpu(); Sleep(3); PauseCpu(); }
			}
			break;
		case ID_OPTIONS_ALWAYSONTOP:
			CheckedMenuItem(ID_OPTIONS_ALWAYSONTOP,&AlwaysOnTop,"Always On Top");
			AlwaysOnTopWindow(hWnd);
			break;
		case ID_OPTIONS_CONFIG_GFX: GFXDllConfig(hWnd); break;
		case ID_OPTIONS_CONFIG_AUDIO: AiDllConfig(hWnd); break;
		case ID_OPTIONS_CONFIG_RSP: RSPDllConfig(hWnd); break;
		case ID_OPTIONS_CONFIG_CONTROL: ContConfig(hWnd); break;
		case ID_OPTIONS_SETTINGS: ChangeSettings(hWnd); break;
		case ID_OPTIONS_CHEATS: ManageCheats(NULL); break;
		case ID_HELP_GUIDE:
			{
				char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
				char fname[_MAX_FNAME],ext[_MAX_EXT], HelpFileName[_MAX_PATH];
				GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
				_splitpath( path_buffer, drive, dir, fname, ext );
   				_makepath( HelpFileName, drive, dir, "User Guide", "txt" );
				if (HtmlHelp(hWnd, HelpFileName, HH_DISPLAY_INDEX, 0) == NULL) {
					ShellExecute(hWnd, "open", HelpFileName, NULL, NULL, SW_SHOW);
				}
			}
		break;
		case ID_REGEDIT: ShellExecute(NULL, "open", "regedit", NULL, NULL, SW_SHOWNORMAL); break;
		case ID_HELP_UNINSTALL: UninstallApplication(hWnd); break;
		case ID_HELP_STANDARD: Standardsettings(hWnd); break;
		case ID_HELP_SM64: SM64settings(hWnd); break;
		case ID_HELP_SLOW: SLOWsettings(hWnd); break;
		case ID_HELP_ABOUTSETTINGFILES: AboutIniBox(); break;
		default:
			if (LOWORD(wParam) >= ID_FILE_RECENT_FILE && LOWORD(wParam) <= (ID_FILE_RECENT_FILE + RomsToRemember)) {
				LoadRecentRom(LOWORD(wParam));
			} else if (LOWORD(wParam) >= ID_FILE_RECENT_DIR && LOWORD(wParam) <= (ID_FILE_RECENT_DIR + RomDirsToRemember)) {
				SetRecentRomDir(LOWORD(wParam));
			} else if (LOWORD(wParam) >= ID_LANG_SELECT && LOWORD(wParam) <= (ID_LANG_SELECT + 100)) {
				SelectLangMenuItem(GetMenu(hWnd),LOWORD(wParam));
				FixRomBrowserColoumnLang();
				SetupMenu(hWnd);
			}
		}
		break;
	case WM_DESTROY:
		if (!ForceClose)
			SaveRomBrowserColoumnInfo();
		PostQuitMessage(0);
		break;
	case WM_DROPFILES: {
		HDROP hDrop = (HDROP)wParam;
		DWORD ThreadID;
		DragQueryFile(hDrop, 0, CurrentFileName, sizeof(CurrentFileName));
		DragFinish(hDrop);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OpenChosenFile, NULL, 0, &ThreadID);
		break;
	}
	default:
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return TRUE;
}
void MenuSetText ( HMENU hMenu, int MenuPos, char * Title, char * ShotCut) {
	MENUITEMINFO MenuInfo;
	char String[256];
	if (Title == NULL || strlen(Title) == 0) { return; }
	memset(&MenuInfo, 0, sizeof(MENUITEMINFO));
	MenuInfo.cbSize = sizeof(MENUITEMINFO);
	MenuInfo.fMask = MIIM_TYPE;
	MenuInfo.fType = MFT_STRING;
	MenuInfo.fState = MFS_ENABLED;
	MenuInfo.dwTypeData = String;
	MenuInfo.cch = 256;
	GetMenuItemInfo(hMenu,MenuPos,TRUE,&MenuInfo);
	if (strchr(Title,'\t') != NULL) { *(strchr(Title,'\t')) = '\0'; }
	strcpy(String,Title);
	if (ShotCut) { sprintf(String,"%s\t%s",String,ShotCut); }
	SetMenuItemInfo(hMenu,MenuPos,TRUE,&MenuInfo);
}
void RegisterExtension ( char * Extension, BOOL RegisterWithPj64 ) {
	char ShortAppName[] = { "PJ64" };
	char sKeyValue[] = { "Project 64" };
  	char app_path[_MAX_PATH];
	char String[200];
	DWORD Disposition = 0;
	HKEY hKeyResults = 0;
	long lResult;
	//Find Application name
	GetModuleFileName(NULL,app_path,sizeof(app_path));
	//creates a Root entry for sKeyName
	lResult = RegCreateKeyEx( HKEY_CLASSES_ROOT, ShortAppName,0,"", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	RegSetValueEx(hKeyResults,"",0,REG_SZ,(BYTE *)sKeyValue,sizeof(sKeyValue));
	RegCloseKey(hKeyResults);
	// Set the command line for "MyApp".
	sprintf(String,"%s\\DefaultIcon",ShortAppName);
	lResult = RegCreateKeyEx( HKEY_CLASSES_ROOT, String,0,"", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	sprintf(String,"%s",app_path);
	RegSetValueEx(hKeyResults,"",0,REG_SZ,(BYTE *)String,strlen(String));
	RegCloseKey(hKeyResults);
	//set the icon for the file extension
	sprintf(String,"%s\\shell\\open\\command",ShortAppName);
	lResult = RegCreateKeyEx( HKEY_CLASSES_ROOT, String,0,"", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	sprintf(String,"%s %%1",app_path);
	RegSetValueEx(hKeyResults,"",0,REG_SZ,(BYTE *)String,strlen(String));
	RegCloseKey(hKeyResults);
	// creates a Root entry for passed associated with sKeyName
	lResult = RegCreateKeyEx( HKEY_CLASSES_ROOT, Extension,0,"", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	if (RegisterWithPj64) {
		RegSetValueEx(hKeyResults,"",0,REG_SZ,(BYTE *)ShortAppName,sizeof(ShortAppName));
	} else {
		RegSetValueEx(hKeyResults,"",0,REG_SZ,(BYTE *)"",1);
	}
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
}
int RegisterWinClass ( void ) {
	WNDCLASS wcl;
	wcl.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;
	wcl.hIcon			= LoadIcon(hInst,"ICON");
	wcl.hCursor			= LoadCursor(hInst,IDC_ARROW);
	wcl.hInstance		= hInst;
	wcl.lpfnWndProc		= (WNDPROC)Main_Proc;
	wcl.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcl.lpszMenuName	= MAKEINTRESOURCE(MAIN_MENU);
	wcl.lpszClassName	= AppName;
	if (RegisterClass(&wcl)  == 0) return FALSE;
	wcl.lpfnWndProc		= (WNDPROC)Cheat_Proc;
	wcl.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
	wcl.lpszMenuName	= NULL;
	wcl.lpszClassName	= "PJ64.Cheats";
	if (RegisterClass(&wcl)  == 0) return FALSE;
	return TRUE;
}
void RomInfo (void) {
	DialogBox(hInst, "ROM_INFO_DIALOG", hMainWindow, (DLGPROC)RomInfoProc);
}
LRESULT CALLBACK RomInfoProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char String[255], count;
	char drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowText(hDlg, GS(INFO_TITLE));
		SetDlgItemText(hDlg, IDC_ROM_NAME, GS(INFO_ROM_NAME_TEXT));
		SetDlgItemText(hDlg, IDC_FILE_NAME, GS(INFO_FILE_NAME_TEXT));
		SetDlgItemText(hDlg, IDC_LOCATION, GS(INFO_LOCATION_TEXT));
		SetDlgItemText(hDlg, IDC_ROM_SIZE, GS(INFO_SIZE_TEXT));
		SetDlgItemText(hDlg, IDC_CART_ID, GS(INFO_CART_ID_TEXT));
		SetDlgItemText(hDlg, IDC_CRC1, GS(INFO_CRC1_TEXT));
		SetDlgItemText(hDlg, IDC_CRC2, GS(INFO_CRC2_TEXT));
		SetDlgItemText(hDlg, IDC_CIC_CHIP, GS(INFO_CIC_CHIP_TEXT));
		memcpy(&String[1],(void *)(&RomHeader[0x20]),20);
		for( count = 1 ; count < 21; count += 4 ) {
			String[count] ^= String[count+3];
			String[count + 3] ^= String[count];
			String[count] ^= String[count+3];
			String[count + 1] ^= String[count + 2];
			String[count + 2] ^= String[count + 1];
			String[count + 1] ^= String[count + 2];
		}
		String[0] = ' ';
		String[21] = '\0';
		SetDlgItemText(hDlg,IDC_INFO_ROMNAME,String);
		_splitpath(CurrentFileName, drive, dir, fname, ext);
		strcpy(&String[1],fname);
		strcat(String,ext);
		SetDlgItemText(hDlg,IDC_INFO_FILENAME,String);
		strcpy(&String[1],drive);
		strcat(String,dir);
		SetDlgItemText(hDlg,IDC_INFO_LOCATION,String);
		sprintf(&String[1],"%.1f MBit",(float)RomFileSize/0x20000);
		SetDlgItemText(hDlg,IDC_INFO_ROMSIZE,String);
		String[1] = RomHeader[0x3F];
		String[2] = RomHeader[0x3E];
		String[3] = '\0';
		SetDlgItemText(hDlg,IDC_INFO_CARTID,String);
		sprintf(&String[1],"0x%08X",*(DWORD *)(&RomHeader[0x10]));
		SetDlgItemText(hDlg,IDC_INFO_CRC1,String);
		sprintf(&String[1],"0x%08X",*(DWORD *)(&RomHeader[0x14]));
		SetDlgItemText(hDlg,IDC_INFO_CRC2,String);
		if (GetCicChipID(RomHeader) < 0) {
			sprintf(&String[1],GS(RB_NOT_IN_RDB));
		} else {
			sprintf(&String[1],"CIC-NUS-610%d",GetCicChipID(RomHeader));
		}
		SetDlgItemText(hDlg,IDC_INFO_CIC,String);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			EndDialog(hDlg,0);
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL TestExtensionRegistered ( char * Extension ) {
	char ShortAppName[] = { "PJ64" };
	HKEY hKeyResults = 0;
	char Association[100];
	long lResult;
	DWORD Type, Bytes;
	lResult = RegOpenKey( HKEY_CLASSES_ROOT,Extension,&hKeyResults);
	if (lResult != ERROR_SUCCESS) { return FALSE; }
	Bytes = sizeof(Association);
	lResult = RegQueryValueEx(hKeyResults,"",0,&Type,(LPBYTE)(&Association),&Bytes);
	RegCloseKey(hKeyResults);
	if (lResult != ERROR_SUCCESS) { return FALSE;  }
	if (strcmp(Association,ShortAppName) != 0) { return FALSE; }
	return TRUE;
}
void SetupMenu ( HWND hWnd ) {
	HMENU hMenu = GetMenu(hWnd), hSubMenu;
	int State;
	DestroyMenu(hMenu);
	hMenu = LoadMenu(hInst,MAKEINTRESOURCE(MAIN_MENU));
	FixMenuLang(hMenu);
	CreateLangList(GetSubMenu(hMenu,0),6, ID_LANG_SELECT);
	CreateRecentDirList(hMenu);
	CreateRecentFileList(hMenu);
	CheckMenuItem( hMenu, CurrentSaveSlot, MF_BYCOMMAND | MFS_CHECKED );
	if (LimitFPS) {
		CheckMenuItem( hMenu, ID_SYSTEM_LIMITFPS, MF_BYCOMMAND | MFS_CHECKED );
	}
	if (AlwaysOnTop) {
		CheckMenuItem( hMenu, ID_OPTIONS_ALWAYSONTOP, MF_BYCOMMAND | MFS_CHECKED );
	}
	EnableMenuItem(hMenu, ID_OPTIONS_ALWAYSONTOP, MF_BYCOMMAND);
	EnableMenuItem(hMenu, ID_OPTIONS_CONFIG_AUDIO, MF_BYCOMMAND | (AiDllConfig == NULL?MF_GRAYED:MF_ENABLED));
	EnableMenuItem(hMenu, ID_OPTIONS_CONFIG_GFX, MF_BYCOMMAND | (GFXDllConfig == NULL?MF_GRAYED:MF_ENABLED));
	EnableMenuItem(hMenu, ID_OPTIONS_CONFIG_RSP, MF_BYCOMMAND | (RSPDllConfig == NULL?MF_GRAYED:MF_ENABLED));
	EnableMenuItem(hMenu, ID_OPTIONS_CONFIG_CONTROL, MF_BYCOMMAND | (ContConfig == NULL?MF_GRAYED:MF_ENABLED));
	if (strlen(RomName) > 0) {
		EnableMenuItem(hMenu,ID_FILE_ROM_INFO,MFS_ENABLED|MF_BYCOMMAND);
		EnableMenuItem(hMenu,ID_FILE_STARTEMULATION,MFS_ENABLED|MF_BYCOMMAND);
		EnableMenuItem(hMenu,ID_SYSTEM_GSBUTTON,MFS_ENABLED|MF_BYCOMMAND);						//added by Witten on 10/03/2002
	}
	//Enable if cpu is running
	State = CPURunning?MFS_ENABLED:MFS_DISABLED;
	EnableMenuItem(hMenu,ID_FILE_ENDEMULATION,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_OPTIONS_FULLSCREEN,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_CPU_RESET,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_CPU_PAUSE,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_SYSTEM_ALTERNATEPAUSE,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_OPTIONS_CHEATS,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_CPU_SAVE,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_CPU_SAVEAS,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_CPU_RESTORE,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_CPU_LOAD,State|MF_BYCOMMAND);
	if (CPURunning && CaptureScreen != NULL) {
		EnableMenuItem(hMenu,ID_SYSTEM_GENERATEBITMAP,MFS_ENABLED|MF_BYCOMMAND);
	} else {
		EnableMenuItem(hMenu,ID_SYSTEM_GENERATEBITMAP,MFS_DISABLED|MF_BYCOMMAND);
	}
	hSubMenu = GetSubMenu(hMenu,1); 	//System
	EnableMenuItem(hSubMenu,12,State|MF_BYPOSITION);  //Save State
	//Disable if cpu is running
	State = CPURunning?MFS_DISABLED:MFS_ENABLED;
	EnableMenuItem(hMenu,ID_FILE_ROMDIRECTORY,State|MF_BYCOMMAND);
	EnableMenuItem(hMenu,ID_FILE_REFRESHROMLIST,State|MF_BYCOMMAND);
	if (State == MFS_DISABLED) { EnableMenuItem(hMenu,ID_FILE_STARTEMULATION,State|MF_BYCOMMAND); }
	SetMenu(hWnd, hMenu);
	DrawMenuBar(hWnd);
	hMainMenu = hMenu;
	hSubMenu = GetSubMenu(hMenu, 0);
	if (!RomBrowser) { DeleteMenu(hSubMenu, 9, MF_BYPOSITION); }
}
void SetCurrentSaveState (HWND hWnd, int State) {
	char String[256];
	HMENU hMenu;
	hMenu = GetMenu(hWnd);
	if (!CPURunning)
		State = ID_CURRENTSAVE_DEFAULT;
	CheckMenuItem(hMenu, ID_CURRENTSAVE_DEFAULT, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_2, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_3, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_4, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_5, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_6, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_7, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_8, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_9, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_10, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_11, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_12, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_13, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_14, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_15, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_16, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_17, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_18, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_19, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_20, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_21, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_22, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_23, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_24, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_25, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_26, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_27, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_28, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_29, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_30, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_31, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_32, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_33, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_34, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_35, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_36, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_37, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_38, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_39, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_40, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_41, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_42, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_43, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_44, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_45, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_46, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_47, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_48, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_49, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, ID_CURRENTSAVE_50, MF_BYCOMMAND | MFS_UNCHECKED);
	CheckMenuItem(hMenu, State, MF_BYCOMMAND | MFS_CHECKED);
	if (strlen(RomName) == 0) { return; }
	strcpy(CurrentSave,RomName);
	switch (State) {
	case ID_CURRENTSAVE_DEFAULT:
		strcat(CurrentSave, " 01.st");
		break;
	case ID_CURRENTSAVE_2:
		strcat(CurrentSave, " 02.st");
		break;
	case ID_CURRENTSAVE_3:
		strcat(CurrentSave, " 03.st");
		break;
	case ID_CURRENTSAVE_4:
		strcat(CurrentSave, " 04.st");
		break;
	case ID_CURRENTSAVE_5:
		strcat(CurrentSave, " 05.st");
		break;
	case ID_CURRENTSAVE_6:
		strcat(CurrentSave, " 06.st");
		break;
	case ID_CURRENTSAVE_7:
		strcat(CurrentSave, " 07.st");
		break;
	case ID_CURRENTSAVE_8:
		strcat(CurrentSave, " 08.st");
		break;
	case ID_CURRENTSAVE_9:
		strcat(CurrentSave, " 09.st");
		break;
	case ID_CURRENTSAVE_10:
		strcat(CurrentSave, " 10.st");
		break;
	case ID_CURRENTSAVE_11:
		strcat(CurrentSave, " 11.st");
		break;
	case ID_CURRENTSAVE_12:
		strcat(CurrentSave, " 12.st");
		break;
	case ID_CURRENTSAVE_13:
		strcat(CurrentSave, " 13.st");
		break;
	case ID_CURRENTSAVE_14:
		strcat(CurrentSave, " 14.st");
		break;
	case ID_CURRENTSAVE_15:
		strcat(CurrentSave, " 15.st");
		break;
	case ID_CURRENTSAVE_16:
		strcat(CurrentSave, " 16.st");
		break;
	case ID_CURRENTSAVE_17:
		strcat(CurrentSave, " 17.st");
		break;
	case ID_CURRENTSAVE_18:
		strcat(CurrentSave, " 18.st");
		break;
	case ID_CURRENTSAVE_19:
		strcat(CurrentSave, " 19.st");
		break;
	case ID_CURRENTSAVE_20:
		strcat(CurrentSave, " 20.st");
		break;
	case ID_CURRENTSAVE_21:
		strcat(CurrentSave, " 21.st");
		break;
	case ID_CURRENTSAVE_22:
		strcat(CurrentSave, " 22.st");
		break;
	case ID_CURRENTSAVE_23:
		strcat(CurrentSave, " 23.st");
		break;
	case ID_CURRENTSAVE_24:
		strcat(CurrentSave, " 24.st");
		break;
	case ID_CURRENTSAVE_25:
		strcat(CurrentSave, " 25.st");
		break;
	case ID_CURRENTSAVE_26:
		strcat(CurrentSave, " 26.st");
		break;
	case ID_CURRENTSAVE_27:
		strcat(CurrentSave, " 27.st");
		break;
	case ID_CURRENTSAVE_28:
		strcat(CurrentSave, " 28.st");
		break;
	case ID_CURRENTSAVE_29:
		strcat(CurrentSave, " 29.st");
		break;
	case ID_CURRENTSAVE_30:
		strcat(CurrentSave, " 30.st");
		break;
	case ID_CURRENTSAVE_31:
		strcat(CurrentSave, " 31.st");
		break;
	case ID_CURRENTSAVE_32:
		strcat(CurrentSave, " 32.st");
		break;
	case ID_CURRENTSAVE_33:
		strcat(CurrentSave, " 33.st");
		break;
	case ID_CURRENTSAVE_34:
		strcat(CurrentSave, " 34.st");
		break;
	case ID_CURRENTSAVE_35:
		strcat(CurrentSave, " 35.st");
		break;
	case ID_CURRENTSAVE_36:
		strcat(CurrentSave, " 36.st");
		break;
	case ID_CURRENTSAVE_37:
		strcat(CurrentSave, " 37.st");
		break;
	case ID_CURRENTSAVE_38:
		strcat(CurrentSave, " 38.st");
		break;
	case ID_CURRENTSAVE_39:
		strcat(CurrentSave, " 39.st");
		break;
	case ID_CURRENTSAVE_40:
		strcat(CurrentSave, " 40.st");
		break;
	case ID_CURRENTSAVE_41:
		strcat(CurrentSave, " 41.st");
		break;
	case ID_CURRENTSAVE_42:
		strcat(CurrentSave, " 42.st");
		break;
	case ID_CURRENTSAVE_43:
		strcat(CurrentSave, " 43.st");
		break;
	case ID_CURRENTSAVE_44:
		strcat(CurrentSave, " 44.st");
		break;
	case ID_CURRENTSAVE_45:
		strcat(CurrentSave, " 45.st");
		break;
	case ID_CURRENTSAVE_46:
		strcat(CurrentSave, " 46.st");
		break;
	case ID_CURRENTSAVE_47:
		strcat(CurrentSave, " 47.st");
		break;
	case ID_CURRENTSAVE_48:
		strcat(CurrentSave, " 48.st");
		break;
	case ID_CURRENTSAVE_49:
		strcat(CurrentSave, " 49.st");
		break;
	case ID_CURRENTSAVE_50:
		strcat(CurrentSave, " 50.st");
		break;
	}
	sprintf(String,"%s",CurrentSave);
	SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)String );
	CurrentSaveSlot = State;
}
void UninstallApplication(HWND hWnd) {
	if (MessageBox(NULL, GS(MSG_CONFIRMATION_UNINSTALL), AppName, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND) ==  IDOK) {
		char path_buffer[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR];
		char fname[_MAX_FNAME], ext[_MAX_EXT], HelpFileName[_MAX_PATH];
		GetModuleFileName(NULL, path_buffer, sizeof(path_buffer));
		_splitpath(path_buffer, drive, dir, fname, ext);
		_makepath(HelpFileName, drive, dir, "Registry Factory Reset", "reg");
		if (HtmlHelp(hWnd, HelpFileName, HH_DISPLAY_INDEX, 0) == NULL) {
			ShellExecute(hWnd, "open", HelpFileName, NULL, NULL, SW_SHOW);
		}
		DestroyWindow(hWnd);
	}
}
void SM64settings(HWND hWnd) {
	if (MessageBox(NULL, GS(SM64_WARN), AppName, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND) ==  IDOK) {
		char path_buffer[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR];
		char fname[_MAX_FNAME], ext[_MAX_EXT], HelpFileName[_MAX_PATH];
		GetModuleFileName(NULL, path_buffer, sizeof(path_buffer));
		_splitpath(path_buffer, drive, dir, fname, ext);
		_makepath(HelpFileName, drive, dir, "Registry SM64 ROMhacks Mode", "reg");
		if (HtmlHelp(hWnd, HelpFileName, HH_DISPLAY_INDEX, 0) == NULL) {
			ShellExecute(hWnd, "open", HelpFileName, NULL, NULL, SW_SHOW);
		}
		DestroyWindow(hWnd);
	}
}
void SLOWsettings(HWND hWnd) {
	if (MessageBox(NULL, GS(SLOW_WARN), AppName, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND) ==  IDOK) {
		char path_buffer[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR];
		char fname[_MAX_FNAME], ext[_MAX_EXT], HelpFileName[_MAX_PATH];
		GetModuleFileName(NULL, path_buffer, sizeof(path_buffer));
		_splitpath(path_buffer, drive, dir, fname, ext);
		_makepath(HelpFileName, drive, dir, "Registry LLE Slow Mode (Experimental)", "reg");
		if (HtmlHelp(hWnd, HelpFileName, HH_DISPLAY_INDEX, 0) == NULL) {
			ShellExecute(hWnd, "open", HelpFileName, NULL, NULL, SW_SHOW);
		}
		DestroyWindow(hWnd);
	}
}
void Standardsettings(HWND hWnd) {
	if (MessageBox(NULL, GS(STANDARD_WARN), AppName, MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND) ==  IDOK) {
		char path_buffer[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR];
		char fname[_MAX_FNAME], ext[_MAX_EXT], HelpFileName[_MAX_PATH];
		GetModuleFileName(NULL, path_buffer, sizeof(path_buffer));
		_splitpath(path_buffer, drive, dir, fname, ext);
		_makepath(HelpFileName, drive, dir, "Registry Standard Mode", "reg");
		if (HtmlHelp(hWnd, HelpFileName, HH_DISPLAY_INDEX, 0) == NULL) {
			ShellExecute(hWnd, "open", HelpFileName, NULL, NULL, SW_SHOW);
		}
		DestroyWindow(hWnd);
	}
}
void ShutdownApplication ( void ) {
	CloseCpu();
	if (TargetInfo != NULL) { VirtualFree(TargetInfo,0,MEM_RELEASE); }
	FreeRomBrowser();
	ShutdownPlugins();
	if (!ForceClose)
		SaveRecentFiles();
	Release_Memory();
	CloseHandle(hPauseMutex);
	CoUninitialize();
}
void StoreCurrentWinPos (  char * WinName, HWND hWnd ) {
	long lResult;
	HKEY hKeyResults = 0;
	DWORD Disposition = 0;
	RECT WinRect;
	char String[200];
	GetWindowRect(hWnd, &WinRect );
	sprintf(String,"N64 Software\\%s\\Page Setup",AppName);
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		sprintf(String,"%s Top",WinName);
		RegSetValueEx(hKeyResults,String,0, REG_DWORD,(CONST BYTE *)(&WinRect.top),
			sizeof(DWORD));
		sprintf(String,"%s Left",WinName);
		RegSetValueEx(hKeyResults,String,0, REG_DWORD,(CONST BYTE *)(&WinRect.left),
			sizeof(DWORD));
	}
	RegCloseKey(hKeyResults);
}
void StoreCurrentWinSize (  char * WinName, HWND hWnd ) {
	long lResult;
	HKEY hKeyResults = 0;
	DWORD Disposition = 0;
	RECT WinRect;
	char String[200];
	GetWindowRect(hWnd, &WinRect );
	WinRect.bottom -= WinRect.top;
	WinRect.right -= WinRect.left;
	sprintf(String,"N64 Software\\%s\\Page Setup",AppName);
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		sprintf(String,"%s Height",WinName);
		RegSetValueEx(hKeyResults,String,0, REG_DWORD,(CONST BYTE *)(&WinRect.bottom),
			sizeof(DWORD));
		sprintf(String,"%s Width",WinName);
		RegSetValueEx(hKeyResults,String,0, REG_DWORD,(CONST BYTE *)(&WinRect.right),
			sizeof(DWORD));
	}
	RegCloseKey(hKeyResults);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgs, int nWinMode) {
#define WindowWidth  640
#define WindowHeight 480
	HANDLE hJob = CreateJobObject(NULL, NULL);
	JOBOBJECT_BASIC_UI_RESTRICTIONS jbur = { 0 };
	jbur.UIRestrictionsClass = JOB_OBJECT_UILIMIT_DESKTOP |
		JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
		JOB_OBJECT_UILIMIT_EXITWINDOWS |
		JOB_OBJECT_UILIMIT_GLOBALATOMS |
		JOB_OBJECT_UILIMIT_HANDLES |
		JOB_OBJECT_UILIMIT_READCLIPBOARD |
		JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |
		JOB_OBJECT_UILIMIT_WRITECLIPBOARD;
	if (!SetInformationJobObject(hJob, JobObjectBasicUIRestrictions, &jbur, sizeof(jbur))) CloseHandle(hJob);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS |
		JOB_OBJECT_LIMIT_AFFINITY |
		JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION |
		JOB_OBJECT_LIMIT_JOB_MEMORY |
		JOB_OBJECT_LIMIT_PRIORITY_CLASS |
		JOB_OBJECT_LIMIT_PROCESS_MEMORY |
		JOB_OBJECT_LIMIT_WORKINGSET;
	jeli.BasicLimitInformation.ActiveProcessLimit = 1;
	jeli.JobMemoryLimit = 100 * 1024 * 1024;
	jeli.ProcessMemoryLimit = 50 * 1024 * 1024;
	jeli.BasicLimitInformation.MinimumWorkingSetSize = 10 * 1024 * 1024;
	jeli.BasicLimitInformation.MaximumWorkingSetSize = 50 * 1024 * 1024;
	jeli.BasicLimitInformation.Affinity = 1;
	jeli.BasicLimitInformation.PriorityClass = NORMAL_PRIORITY_CLASS;
	if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) CloseHandle(hJob);
	JOBOBJECT_SECURITY_LIMIT_INFORMATION jsl = { 0 };
	jsl.SecurityLimitFlags = JOB_OBJECT_SECURITY_NO_ADMIN |
		JOB_OBJECT_SECURITY_RESTRICTED_TOKEN |
		JOB_OBJECT_SECURITY_ONLY_TOKEN |
		JOB_OBJECT_SECURITY_FILTER_TOKENS;
	if (!SetInformationJobObject(hJob, JobObjectSecurityLimitInformation, &jsl, sizeof(jsl))) CloseHandle(hJob);
	if (!AssignProcessToJobObject(hJob, GetCurrentProcess())) CloseHandle(hJob);
	HACCEL AccelWinMode, AccelCPURunning, AccelRomBrowser;
	DWORD X, Y;
	MSG msg;
	if ( !InitalizeApplication (hInstance) ) { return FALSE; }
	if ( !RegisterWinClass () ) { return FALSE; }
	if ( !GetStoredWinPos( "Main", &X, &Y ) ) {
  		X = (GetSystemMetrics( SM_CXSCREEN ) - WindowWidth) / 2;
		Y = (GetSystemMetrics( SM_CYSCREEN ) - WindowHeight) / 2;
	}
	AccelWinMode    = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_WINDOWMODE));
	AccelCPURunning = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_CPURUNNING));
	AccelRomBrowser = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_ROMBROWSER));
	hHiddenWin = CreateWindow( AppName, AppName, WS_OVERLAPPED | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX,X,Y,WindowWidth,WindowHeight,
		NULL,NULL,hInst,NULL
	);
	if ( !hHiddenWin ) { return FALSE; }
	hMainWindow = CreateWindow( AppName, AppName, WS_OVERLAPPED | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX,X,Y,WindowWidth,WindowHeight,
		NULL,NULL,hInst,NULL
	);
        DragAcceptFiles(hMainWindow, TRUE);
	if ( !hMainWindow ) { return FALSE; }
	if (__argc > 1) {
		DWORD ThreadID;
		SetupPlugins(hMainWindow);
		SetupMenu(hMainWindow);
		ShowWindow(hMainWindow, nWinMode);
		strcpy(CurrentFileName, __argv[1]);
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)OpenChosenFile,NULL,0, &ThreadID);
	} else {
		if (RomBrowser) {
			ShowRomList(hMainWindow);
			RefreshRomBrowser();
		} else {
			SetupPlugins(hMainWindow);
			SetupMenu(hMainWindow);
			ShowWindow(hMainWindow, nWinMode);
		}
		if (!LimitFPS) SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(LIMITFPS_OFF));
	}
	while (GetMessage(&msg,NULL,0,0)) {
		if (!CPURunning && TranslateAccelerator(hMainWindow,AccelRomBrowser,&msg)) { continue; }
		if (CPURunning) {
			if (TranslateAccelerator(hMainWindow,AccelCPURunning,&msg)) { continue; }
			if (!inFullScreen && TranslateAccelerator(hMainWindow,AccelWinMode,&msg)) { continue; }
		}
		if (IsDialogMessage( hManageWindow,&msg)) { continue; }
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
	}
	ShutdownApplication ();
	return msg.wParam;
	CloseHandle(hJob);
}