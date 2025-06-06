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
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include "main.h"
#include "cpu.h"
#include "plugin.h"
#include "resource.h"
BOOL CALLBACK DefaultOptionsProc   ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK GeneralOptionsProc   ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK DirSelectProc        ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK PluginSelectProc     ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK RomBrowserProc       ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK RomSettingsProc      ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK RomNotesProc         ( HWND, UINT, WPARAM, LPARAM );
typedef struct {
	int     LanguageID;
	WORD    TemplateID;
	DLGPROC pfnDlgProc;
} SETTINGS_TAB;
SETTINGS_TAB SettingsTabs[] = {
	{ TAB_PLUGIN,          IDD_Settings_PlugSel,   PluginSelectProc     },
	{ TAB_DIRECTORY,       IDD_Settings_Directory, DirSelectProc        },
	{ MENU_OPTIONS,         IDD_Settings_General,   GeneralOptionsProc   },
	{ TAB_ROMSELECTION,    IDD_Settings_RomBrowser,RomBrowserProc       },
	{ TAB_ADVANCED,        IDD_Settings_Options,   DefaultOptionsProc   },
	{ TAB_ROMSETTINGS,     IDD_Settings_Rom,       RomSettingsProc      },
	{ TAB_ROMNOTES,        IDD_Settings_RomNotes,  RomNotesProc         },
};
SETTINGS_TAB SettingsTabsBasic[] = {
	{ TAB_PLUGIN, IDD_Settings_PlugSel,PluginSelectProc               },
	{ TAB_DIRECTORY,IDD_Settings_Directory,DirSelectProc              },
	{ MENU_OPTIONS,IDD_Settings_General,GeneralOptionsProc             },
	{ TAB_ROMSELECTION,IDD_Settings_RomBrowser,RomBrowserProc         },
	{ TAB_ROMNOTES,IDD_Settings_RomNotes,RomNotesProc                 },
};
SETTINGS_TAB SettingsTabsRom[] = {
	{ TAB_ROMNOTES,    IDD_Settings_RomNotes, RomNotesProc     },
};
void ChangeRomSettings(HWND hwndOwner) {
	char OrigRomName[sizeof(RomName)], OrigFileName[sizeof(CurrentFileName)];
    PROPSHEETPAGE psp[sizeof(SettingsTabsRom) / sizeof(SETTINGS_TAB)];
	BYTE OrigByteHeader[sizeof(RomHeader)];
    PROPSHEETHEADER psh;
	DWORD OrigFileSize, count;
	strncpy(OrigRomName,RomName,sizeof(OrigRomName));
	strncpy(OrigFileName,CurrentFileName,sizeof(OrigFileName));
	memcpy(OrigByteHeader,RomHeader,sizeof(RomHeader));
	strncpy(CurrentFileName,CurrentRBFileName,sizeof(CurrentFileName));
	OrigFileSize = RomFileSize;
	LoadRomHeader();
	for (count = 0; count < (sizeof(SettingsTabsRom) / sizeof(SETTINGS_TAB)); count ++) {
		psp[count].dwSize = sizeof(PROPSHEETPAGE);
		psp[count].dwFlags = PSP_USETITLE;
		psp[count].hInstance = hInst;
		psp[count].pszTemplate = MAKEINTRESOURCE(SettingsTabsRom[count].TemplateID);
		psp[count].pfnDlgProc = SettingsTabsRom[count].pfnDlgProc;
		psp[count].pszTitle = GS(SettingsTabsRom[count].LanguageID);
		psp[count].lParam = 0;
		psp[count].pfnCallback = NULL;
	}
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInst;
    psh.pszCaption = (LPSTR)GS(MENU_SETTINGS);
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;
	PropertySheet(&psh);
	strncpy(RomName,OrigRomName,sizeof(RomName));
	strncpy(CurrentFileName,OrigFileName,sizeof(CurrentFileName));
	memcpy(RomHeader,OrigByteHeader,sizeof(RomHeader));
	RomFileSize = OrigFileSize;
}
void ChangeSettings(HWND hwndOwner) {
    PROPSHEETPAGE psp[sizeof(SettingsTabs) / sizeof(SETTINGS_TAB)];
    PROPSHEETPAGE BasicPsp[sizeof(SettingsTabsBasic) / sizeof(SETTINGS_TAB)];
    PROPSHEETHEADER psh;
	int count;
	for (count = 0; count < (sizeof(SettingsTabs) / sizeof(SETTINGS_TAB)); count ++) {
		psp[count].dwSize = sizeof(PROPSHEETPAGE);
		psp[count].dwFlags = PSP_USETITLE;
		psp[count].hInstance = hInst;
		psp[count].pszTemplate = MAKEINTRESOURCE(SettingsTabs[count].TemplateID);
		psp[count].pfnDlgProc = SettingsTabs[count].pfnDlgProc;
		psp[count].pszTitle = GS(SettingsTabs[count].LanguageID);
		psp[count].lParam = 0;
		psp[count].pfnCallback = NULL;
	}
	for (count = 0; count < (sizeof(SettingsTabsBasic) / sizeof(SETTINGS_TAB)); count ++) {
		BasicPsp[count].dwSize = sizeof(PROPSHEETPAGE);
		BasicPsp[count].dwFlags = PSP_USETITLE;
		BasicPsp[count].hInstance = hInst;
		BasicPsp[count].pszTemplate = MAKEINTRESOURCE(SettingsTabsBasic[count].TemplateID);
		BasicPsp[count].pfnDlgProc = SettingsTabsBasic[count].pfnDlgProc;
		BasicPsp[count].pszTitle = GS(SettingsTabsBasic[count].LanguageID);
		BasicPsp[count].lParam = 0;
		BasicPsp[count].pfnCallback = NULL;
	}
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInst;
    psh.pszCaption = (LPSTR)GS(MENU_SETTINGS);
    psh.nPages = (BasicMode?sizeof(BasicPsp):sizeof(psp)) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = BasicMode?(LPCPROPSHEETPAGE) &BasicPsp:(LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;
	PropertySheet(&psh);
	LoadSettings();
	if (!AutoSleep && !ManualPaused && (CPU_Paused || CPU_Action.Pause)) {
		PauseCpu();
		Timer_Start();
	}
	return;
}
void SetFlagControl (HWND hDlg, BOOL * Flag, WORD CtrlID, int StringID) {
	SetDlgItemText(hDlg,CtrlID,GS(StringID));
	if (*Flag) { SendMessage(GetDlgItem(hDlg,CtrlID),BM_SETCHECK, BST_CHECKED,0); }
}
BOOL CALLBACK GeneralOptionsProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		SetFlagControl(hDlg, &AutoSleep,      IDC_AUTOSLEEP,       OPTION_AUTO_SLEEP);
		SetFlagControl(hDlg, &AutoFullScreen, IDC_LOAD_FULLSCREEN, OPTION_AUTO_FULLSCREEN);
		SetFlagControl(hDlg, &BasicMode,      IDC_BASIC_MODE,      OPTION_BASIC_MODE);
		SetFlagControl(hDlg, &RememberCheats, IDC_REMEMBER_CHEAT,  OPTION_REMEMBER_CHEAT);
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code == PSN_APPLY) {
			long lResult;
			HKEY hKeyResults = 0;
			DWORD Disposition = 0;
			char String[200];
			sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
			lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
			if (lResult == ERROR_SUCCESS) {
				AutoFullScreen = SendMessage(GetDlgItem(hDlg,IDC_LOAD_FULLSCREEN),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Enter Fullscreen Mode Upon ROM Opening",0,REG_DWORD,(BYTE *)&AutoFullScreen,sizeof(DWORD));
				BasicMode = SendMessage(GetDlgItem(hDlg,IDC_BASIC_MODE),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Basic Mode",0,REG_DWORD,(BYTE *)&BasicMode,sizeof(DWORD));
				RememberCheats = SendMessage(GetDlgItem(hDlg,IDC_REMEMBER_CHEAT),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Remember Cheats",0,REG_DWORD,(BYTE *)&RememberCheats,sizeof(DWORD));
				AutoSleep = SendMessage(GetDlgItem(hDlg,IDC_AUTOSLEEP),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Pause CPU Upon Focus Loss",0,REG_DWORD,(BYTE *)&AutoSleep,sizeof(DWORD));
			}
			RegCloseKey(hKeyResults);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
void AddDropDownItem (HWND hDlg, WORD CtrlID, int StringID, int ItemData, int * Variable) {
	HWND hCtrl = GetDlgItem(hDlg,CtrlID);
	int indx;
	indx = SendMessage(hCtrl,CB_ADDSTRING,0,(LPARAM)GS(StringID));
	SendMessage(hCtrl,CB_SETITEMDATA,indx,ItemData);
	if (*Variable == ItemData) { SendMessage(hCtrl,CB_SETCURSEL,indx,0); }
	if (SendMessage(hCtrl,CB_GETCOUNT,0,0) == 0) { SendMessage(hCtrl,CB_SETCURSEL,0,0); }
}
BOOL CALLBACK DefaultOptionsProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int indx;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDC_CORE_DEFAULTS,GS(ADVANCE_DEFAULTS));
		SetDlgItemText(hDlg,IDC_TEXT2,GS(ROM_CPU_STYLE));
		SetDlgItemText(hDlg,IDC_TEXT3,GS(ROM_SMCM));
		SetDlgItemText(hDlg,IDC_TEXT4,GS(ROM_MEM_SIZE));
		SetDlgItemText(hDlg,IDC_TEXT5,GS(ROM_COUNTER_FACTOR));
		SetFlagControl(hDlg, &ForceDisableTLB, IDC_ForceDisableTLB, FORCE_DISABLE_TLB);
		SetFlagControl(hDlg, &ForceEnableDMA, IDC_ForceEnableDMA, FORCE_ENABLE_DMA);
		SetFlagControl(hDlg, &ForceEnableCaching, IDC_ForceEnableCaching, FORCE_ENABLE_REGISTERCACHING);
		SetFlagControl(hDlg, &ForceEnableDelayRDP, IDC_ForceEnableDelayRDP, FORCE_ENABLE_DELAYRDP);
		SetFlagControl(hDlg, &ForceAuto16, IDC_ForceAuto16, FORCE_AUTO16);
		AddDropDownItem(hDlg,IDC_CPU_TYPE,CORE_INTERPRETER,CPU_Interpreter,&SystemCPU_Type);
		AddDropDownItem(hDlg,IDC_CPU_TYPE,CORE_RECOMPILER,CPU_Recompiler,&SystemCPU_Type);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_NONE,ModCode_None,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_CACHE,ModCode_Cache,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_PROTECTED,ModCode_ProtectedMemory,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_CHECK_MEM,ModCode_CheckMemoryCache,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_CHANGE_MEM,ModCode_ChangeMemory,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_CHECK_ADV,ModCode_CheckMemoryAdvance,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_CHECK_RETURN,ModCode_CheckMemoryReturn,&SystemSelfModCheck);
		AddDropDownItem(hDlg,IDC_RDRAM_SIZE,RDRAM_4MB,0x400000,&SystemRDRAMsize);
		AddDropDownItem(hDlg,IDC_RDRAM_SIZE,RDRAM_8MB,0x800000,&SystemRDRAMsize);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_1,1,&SystemCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_2,2,&SystemCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_3,3,&SystemCF);
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code == PSN_APPLY) {
			long lResult;
			HKEY hKeyResults = 0;
			DWORD Disposition = 0;
			char String[200];
			sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
			lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
			if (lResult == ERROR_SUCCESS) {
				ForceDisableTLB = SendMessage(GetDlgItem(hDlg, IDC_ForceDisableTLB), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RegSetValueEx(hKeyResults, "Always Disable TLB", 0, REG_DWORD, (BYTE*)&ForceDisableTLB, sizeof(DWORD));
				ForceEnableDMA = SendMessage(GetDlgItem(hDlg, IDC_ForceEnableDMA), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RegSetValueEx(hKeyResults, "Always Enable Align DMA", 0, REG_DWORD, (BYTE*)&ForceEnableDMA, sizeof(DWORD));
				ForceEnableCaching = SendMessage(GetDlgItem(hDlg, IDC_ForceEnableCaching), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RegSetValueEx(hKeyResults, "Always Enable Register Caching", 0, REG_DWORD, (BYTE*)&ForceEnableCaching, sizeof(DWORD));
				ForceEnableDelayRDP = SendMessage(GetDlgItem(hDlg, IDC_ForceEnableDelayRDP), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RegSetValueEx(hKeyResults, "Always Enable Delay RDP", 0, REG_DWORD, (BYTE*)&ForceEnableDelayRDP, sizeof(DWORD));
				ForceAuto16 = SendMessage(GetDlgItem(hDlg, IDC_ForceAuto16), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RegSetValueEx(hKeyResults, "Always Autodetect With 16kbit", 0, REG_DWORD, (BYTE*)&ForceAuto16, sizeof(DWORD));
				indx = SendMessage(GetDlgItem(hDlg,IDC_CPU_TYPE),CB_GETCURSEL,0,0);
				SystemCPU_Type = SendMessage(GetDlgItem(hDlg,IDC_CPU_TYPE),CB_GETITEMDATA,indx,0);
				RegSetValueEx(hKeyResults,"CPU Core Style",0,REG_DWORD,(BYTE *)&SystemCPU_Type,sizeof(DWORD));
				indx = SendMessage(GetDlgItem(hDlg,IDC_SELFMOD),CB_GETCURSEL,0,0);
				SystemSelfModCheck = SendMessage(GetDlgItem(hDlg,IDC_SELFMOD),CB_GETITEMDATA,indx,0);
				RegSetValueEx(hKeyResults,"Self-modifying Code Method",0,REG_DWORD,(BYTE *)&SystemSelfModCheck,sizeof(DWORD));
				indx = SendMessage(GetDlgItem(hDlg,IDC_RDRAM_SIZE),CB_GETCURSEL,0,0);
				SystemRDRAMsize = SendMessage(GetDlgItem(hDlg,IDC_RDRAM_SIZE),CB_GETITEMDATA,indx,0);
				RegSetValueEx(hKeyResults,"Memory Size",0,REG_DWORD,(BYTE *)&SystemRDRAMsize,sizeof(DWORD));
				indx = SendMessage(GetDlgItem(hDlg,IDC_CF),CB_GETCURSEL,0,0);
				SystemCF = SendMessage(GetDlgItem(hDlg,IDC_CF),CB_GETITEMDATA,indx,0);
				RegSetValueEx(hKeyResults,"Counter Factor",0,REG_DWORD,(BYTE *)&SystemCF,sizeof(DWORD));
			}
			RegCloseKey(hKeyResults);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
int CALLBACK SelectDirCallBack(HWND hwnd,DWORD uMsg,DWORD lp, DWORD lpData) {
  switch(uMsg)
  {
    case BFFM_INITIALIZED:
      // WParam is TRUE since you are passing a path.
      // It would be FALSE if you were passing a pidl.
      if (lpData)
      {
        SendMessage((HWND)hwnd,BFFM_SETSELECTION,TRUE,lpData);
      }
      break;
  }
  return 0;
}
BOOL CALLBACK DirSelectProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			HKEY hKeyResults = 0;
			char String[256];
			char Directory[255];
			long lResult;
			sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
			lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults);
			if (lResult == ERROR_SUCCESS) {
				DWORD Type, Value, Bytes = 4;
				lResult = RegQueryValueEx(hKeyResults,"AppPath ROMs",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { Value = FALSE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_ROM_DEFAULT:IDC_ROM_OTHER),BM_SETCHECK, BST_CHECKED,0);
				lResult = RegQueryValueEx(hKeyResults,"AppPath Save Data",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { Value = TRUE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_AUTO_DEFAULT:IDC_AUTO_OTHER),BM_SETCHECK, BST_CHECKED,0);
				lResult = RegQueryValueEx(hKeyResults,"AppPath Screenshots",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { Value = TRUE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_SNAP_DEFAULT:IDC_SNAP_OTHER),BM_SETCHECK, BST_CHECKED,0);
				lResult = RegQueryValueEx(hKeyResults,"AppPath Savestates",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { Value = TRUE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_INSTANT_DEFAULT:IDC_INSTANT_OTHER),BM_SETCHECK, BST_CHECKED,0);
				Bytes = sizeof(Directory);
				lResult = RegQueryValueEx(hKeyResults,"CustomPath Savestates",0,&Type,(LPBYTE)Directory,&Bytes);
				if (lResult != ERROR_SUCCESS) { GetInstantSaveDir(Directory); }
				SetDlgItemText(hDlg,IDC_INSTANT_DIR,Directory);
				Bytes = sizeof(Directory);
				lResult = RegQueryValueEx(hKeyResults,"CustomPath Save Data",0,&Type,(LPBYTE)Directory,&Bytes);
				if (lResult != ERROR_SUCCESS) { GetAutoSaveDir(Directory); }
				SetDlgItemText(hDlg,IDC_AUTO_DIR,Directory);
				Bytes = sizeof(Directory);
				lResult = RegQueryValueEx(hKeyResults,"CustomPath Screenshots",0,&Type,(LPBYTE)Directory,&Bytes);
				if (lResult != ERROR_SUCCESS) { GetSnapShotDir(Directory); }
				SetDlgItemText(hDlg,IDC_SNAP_DIR,Directory);
			} else {
				SendMessage(GetDlgItem(hDlg,IDC_ROM_DEFAULT),BM_SETCHECK, BST_CHECKED,0);
				SendMessage(GetDlgItem(hDlg,IDC_AUTO_DEFAULT),BM_SETCHECK, BST_CHECKED,0);
				SendMessage(GetDlgItem(hDlg,IDC_INSTANT_DEFAULT),BM_SETCHECK, BST_CHECKED,0);
				SendMessage(GetDlgItem(hDlg,IDC_SNAP_DEFAULT),BM_SETCHECK, BST_CHECKED,0);
				GetInstantSaveDir(Directory);
				SetDlgItemText(hDlg,IDC_INSTANT_DIR,Directory);
				GetAutoSaveDir(Directory);
				SetDlgItemText(hDlg,IDC_AUTO_DIR,Directory);
				GetSnapShotDir(Directory);
				SetDlgItemText(hDlg,IDC_SNAP_DIR,Directory);
			}
			GetRomDirectory( Directory );
			SetDlgItemText(hDlg,IDC_ROM_DIR,Directory);
			SetDlgItemText(hDlg,IDC_DIR_FRAME2,GS(RB_ROMS));
			SetDlgItemText(hDlg,IDC_DIR_FRAME3,GS(DIR_AUTO_SAVE));
			SetDlgItemText(hDlg,IDC_DIR_FRAME4,GS(DIR_INSTANT_SAVE));
			SetDlgItemText(hDlg,IDC_DIR_FRAME5,GS(DIR_SCREEN_SHOT));
			SetDlgItemText(hDlg,IDC_ROM_DEFAULT,GS(DIR_ROM_DEFAULT));
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_SELECT_ROM_DIR:
		case IDC_SELECT_INSTANT_DIR:
		case IDC_SELECT_AUTO_DIR:
		case IDC_SELECT_SNAP_DIR:
			{
				char Buffer[MAX_PATH], Directory[255], Title[255];
				LPITEMIDLIST pidl;
				BROWSEINFO bi;
				switch (LOWORD(wParam)) {
				case IDC_SELECT_ROM_DIR:
					GetRomDirectory(Directory);
					strcpy(Title,GS(DIR_SELECT_ROM));
					break;
				case IDC_SELECT_AUTO_DIR:
					GetAutoSaveDir(Directory);
					strcpy(Title,GS(DIR_SELECT_AUTO));
					break;
				case IDC_SELECT_INSTANT_DIR:
					GetInstantSaveDir(Directory);
					strcpy(Title,GS(DIR_SELECT_INSTANT));
					break;
				case IDC_SELECT_SNAP_DIR:
					GetSnapShotDir(Directory);
					strcpy(Title,GS(DIR_SELECT_SCREEN));
					break;
				}
				bi.hwndOwner = hDlg;
				bi.pidlRoot = NULL;
				bi.pszDisplayName = Buffer;
				bi.lpszTitle = Title;
				bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
				bi.lpfn = (BFFCALLBACK)SelectDirCallBack;
				bi.lParam = (DWORD)Directory;
				if ((pidl = SHBrowseForFolder(&bi)) != NULL) {
					if (SHGetPathFromIDList(pidl, Directory)) {
						int len = strlen(Directory);
						if (Directory[len - 1] != '\\') { strcat(Directory,"\\"); }
						switch (LOWORD(wParam)) {
						case IDC_SELECT_ROM_DIR:
							SetDlgItemText(hDlg,IDC_ROM_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_ROM_DEFAULT),BM_SETCHECK, BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_ROM_OTHER),BM_SETCHECK, BST_CHECKED,0);
							break;
						case IDC_SELECT_INSTANT_DIR:
							SetDlgItemText(hDlg,IDC_INSTANT_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_INSTANT_DEFAULT),BM_SETCHECK, BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_INSTANT_OTHER),BM_SETCHECK, BST_CHECKED,0);
							break;
						case IDC_SELECT_AUTO_DIR:
							SetDlgItemText(hDlg,IDC_AUTO_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_AUTO_DEFAULT),BM_SETCHECK, BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_AUTO_OTHER),BM_SETCHECK, BST_CHECKED,0);
							break;
						case IDC_SELECT_SNAP_DIR:
							SetDlgItemText(hDlg,IDC_SNAP_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_SNAP_DEFAULT),BM_SETCHECK, BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_SNAP_OTHER),BM_SETCHECK, BST_CHECKED,0);
							break;
						}
					}
				}
			}
			break;
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code == PSN_APPLY) {
			long lResult;
			HKEY hKeyResults = 0;
			DWORD Disposition = 0;
			char String[200];
			sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
			lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
			if (lResult == ERROR_SUCCESS) {
				DWORD Value;
				Value = SendMessage(GetDlgItem(hDlg,IDC_ROM_DEFAULT),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath ROMs",0,REG_DWORD,(BYTE *)&Value,sizeof(DWORD));
				if (Value == FALSE) {
					GetDlgItemText(hDlg,IDC_ROM_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath ROMs",0,REG_SZ,(CONST BYTE *)String,strlen(String));
				}
				Value = SendMessage(GetDlgItem(hDlg,IDC_AUTO_DEFAULT),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath Save Data",0,REG_DWORD,(BYTE *)&Value,sizeof(DWORD));
				if (Value == FALSE) {
					GetDlgItemText(hDlg,IDC_AUTO_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath Save Data",0,REG_SZ,(CONST BYTE *)String,strlen(String));
				}
				Value = SendMessage(GetDlgItem(hDlg,IDC_INSTANT_DEFAULT),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath Savestates",0,REG_DWORD,(BYTE *)&Value,sizeof(DWORD));
				if (Value == FALSE) {
					GetDlgItemText(hDlg,IDC_INSTANT_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath Savestates",0,REG_SZ,(CONST BYTE *)String,strlen(String));
                                }
				Value = SendMessage(GetDlgItem(hDlg,IDC_SNAP_DEFAULT),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath Screenshots",0,REG_DWORD,(BYTE *)&Value,sizeof(DWORD));
				if (Value == FALSE) {
					GetDlgItemText(hDlg,IDC_SNAP_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath Screenshots",0,REG_SZ,(CONST BYTE *)String,strlen(String));
				}
			}
			RegCloseKey(hKeyResults);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL PluginsChanged ( HWND hDlg ) {
	if (CPURunning) SetDlgItemText(GetParent(hDlg), IDOK, GS(OKPOSTSWAP));
	DWORD index;
	index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETCURSEL,0,0);
	index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index >= 0) {
		if(_stricmp(RspDLL,PluginNames[index]) != 0) { return TRUE; }
	}
	index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETCURSEL,0,0);
	index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index >= 0) {
		if(_stricmp(GfxDLL,PluginNames[index]) != 0) { return TRUE; }
	}
	index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETCURSEL,0,0);
	index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index >= 0) {
		if(_stricmp(AudioDLL,PluginNames[index]) != 0) { return TRUE; }
	}
	index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETCURSEL,0,0);
	index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index >= 0) {
		if(_stricmp(ControllerDLL,PluginNames[index]) != 0) { return TRUE; }
	}
	return FALSE;
}
void FreePluginList() {
	unsigned int count;
	for (count = 0; count <	PluginCount; count ++ ) {
		free(PluginNames[count]);
	}
	PluginCount = 0;
}
BOOL CALLBACK PluginSelectProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char Plugin[300];
	HANDLE hLib;
	DWORD index;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetupPluginScreen(hDlg);
		if (CPURunning) SetDlgItemText(GetParent(hDlg), IDOK, GS(OKPRESWAP));
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case RSP_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETCURSEL,0,0);
				if (index == CB_ERR) { break; } // *** Add in Build 53
				index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib = LoadLibrary(Plugin);
				if (hLib == NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				RSPDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,RSP_ABOUT),RSPDllAbout != NULL ? TRUE:FALSE);
			}
			break;
		case GFX_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETCURSEL,0,0);
				if (index == CB_ERR) { break; } // *** Add in Build 53
				index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib = LoadLibrary(Plugin);
				if (hLib == NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				GFXDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,GFX_ABOUT),GFXDllAbout != NULL ? TRUE:FALSE);
			}
			break;
		case AUDIO_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETCURSEL,0,0);
				if (index == CB_ERR) { break; } // *** Add in Build 53
				index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib = LoadLibrary(Plugin);
				if (hLib == NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				AiDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,GFX_ABOUT),GFXDllAbout != NULL ? TRUE:FALSE);
			}
			break;
		case CONT_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETCURSEL,0,0);
				if (index == CB_ERR) { break; } // *** Add in Build 53
				index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib = LoadLibrary(Plugin);
				if (hLib == NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				ContDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hLib, "DllAbout" );
				EnableWindow(GetDlgItem(hDlg,CONT_ABOUT),ContDllAbout != NULL ? TRUE:FALSE);
			}
			break;
		case RSP_ABOUT: RSPDllAbout(hDlg); break;
		case GFX_ABOUT: GFXDllAbout(hDlg); break;
		case CONT_ABOUT: ContDllAbout(hDlg); break;
		case AUDIO_ABOUT: AiDllAbout(hDlg); break;
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code == PSN_APPLY) {
			long lResult;
			HKEY hKeyResults = 0;
			DWORD Disposition = 0;
			char String[200];
			if (PluginsChanged(hDlg) == FALSE) { FreePluginList(); break; }
			sprintf(String,"PJ64 V 1.6.2\\%s\\Dll",AppName);
			lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
			if (lResult == ERROR_SUCCESS) {
				DWORD index;
				index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETCURSEL,0,0);
				index = SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"RSP Dll",0,REG_SZ,(CONST BYTE *)String,
					strlen(String));
				index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETCURSEL,0,0);
				index = SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"Graphics Dll",0,REG_SZ,(CONST BYTE *)String,
					strlen(String));
				index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETCURSEL,0,0);
				index = SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"Audio Dll",0,REG_SZ,(CONST BYTE *)String,
					strlen(String));
				index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETCURSEL,0,0);
				index = SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"Controller Dll",0,REG_SZ,(CONST BYTE *)String,
					strlen(String));
			}
			RegCloseKey(hKeyResults);
			if (CPURunning) {
				DestroyWindow(hDlg);
					char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
					char SaveFile[255], Directory[255];
					GetInstantSaveDir(Directory);
					_splitpath(SaveFile, drive, dir, fname, ext);
					_makepath(SaveFile, drive, Directory, (GS(HOT_SWAP_COMPLETE)), "");
					strcpy(SaveAsFileName, SaveFile);
					CPU_Action.SaveState = TRUE;
					CloseCpu();
					DWORD ThreadID;
					memset(&CPU_Action, 0, sizeof(CPU_Action));
					LoadRomOptions();
					if (strcmp(GfxDLL, "Icepir8sLegacyLLE.dll") == 0) GetCurrentDlls();
					if (strcmp(GfxDLL, "Icepir8sLegacyLLE.dll") != 0) HideRomBrowser();
					SetupPlugins(hMainWindow);
					/*if (strcmp(GfxDLL, "Icepir8sLegacyLLE.dll") == 0) {
						ShowRomList(hMainWindow);
						NotUsuallyonTopWindow(hMainWindow);
					}*/
					switch (CPU_Type) {
					case CPU_Interpreter: hCPU = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartInterpreterCPU, NULL, 0, &ThreadID); break;
					case CPU_Recompiler: hCPU = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRecompilerCPU, NULL, 0, &ThreadID); break;
					}
					/*if (strcmp(GfxDLL, "Icepir8sLegacyLLE.dll") == 0) {
						CPURunning = FALSE;
						timeBeginPeriod(16);
						Sleep(10);
						timeEndPeriod(16);
						HideRomBrowser();
						CPURunning = TRUE;
						SetupMenu(hMainWindow);
					}*/
					_splitpath(SaveFile, drive, dir, fname, ext);
					_makepath(SaveFile, drive, dir, (GS(HOT_SWAP_COMPLETE)), "");
					strcpy(LoadFileName, SaveFile);
					CPU_Action.RestoreState = TRUE;
					if (AutoSleep) {
						PauseCpu();
						Timer_Start();
					} else Timer_Start();
			} else SetupPlugins(hHiddenWin);
			FreePluginList();
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
void RomAddFieldToList (HWND hDlg, char * Name, int Pos, int ID) {
	int listCount, index;
	if (Pos < 0) {
		index = SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_ADDSTRING,0,(LPARAM)Name);
		SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_SETITEMDATA,index,ID);
		return;
	}
	listCount = SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
	if (Pos > listCount) { Pos = listCount; }
	index = SendDlgItemMessage(hDlg,IDC_USING,LB_INSERTSTRING,Pos,(LPARAM)Name);
	SendDlgItemMessage(hDlg,IDC_USING,LB_SETITEMDATA,index,ID);
}
BOOL CALLBACK RomBrowserProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		if (Rercursion) { SendMessage(GetDlgItem(hDlg,IDC_RECURSION),BM_SETCHECK, BST_CHECKED,0); }
		{
			int count;
			for (count = 0; count < NoOfFields; count ++) {
				RomAddFieldToList(hDlg,GS(RomBrowserFields[count].LangID),RomBrowserFields[count].Pos, count);
			}
		}
		{
			char String[256];
			sprintf(String,"%d",RomsToRemember);
			SetDlgItemText(hDlg,IDC_REMEMBER,String);
			sprintf(String,"%d",RomDirsToRemember);
			SetDlgItemText(hDlg,IDC_REMEMBERDIR,String);
			SetDlgItemText(hDlg,IDC_ROMSEL_TEXT1,GS(RB_MAX_ROMS));
			SetDlgItemText(hDlg,IDC_ROMSEL_TEXT2,GS(RB_ROMS));
			SetDlgItemText(hDlg,IDC_ROMSEL_TEXT3,GS(RB_MAX_DIRS));
			SetDlgItemText(hDlg,IDC_ROMSEL_TEXT4,GS(RB_DIRS));
			SetDlgItemText(hDlg,IDC_RECURSION,GS(RB_DIR_RECURSION));
			SetDlgItemText(hDlg,IDC_ROMSEL_TEXT5,GS(RB_AVAILABLE_FIELDS));
			SetDlgItemText(hDlg,IDC_ROMSEL_TEXT6,GS(RB_SHOW_FIELDS));
			SetDlgItemText(hDlg,IDC_ADD,GS(RB_ADD));
			SetDlgItemText(hDlg,IDC_REMOVE,GS(RB_REMOVE));
			SetDlgItemText(hDlg,IDC_UP,GS(RB_UP));
			SetDlgItemText(hDlg,IDC_DOWN,GS(RB_DOWN));
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ADD:
			{
				char String[100];
				int index, listCount, Data;
				index = SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETCURSEL,0,0);
				if (index < 0) { break; }
				SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETTEXT,index,(LPARAM)String);
				Data = SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETITEMDATA,index,0);
				SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_DELETESTRING,index,0);
				listCount = SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_GETCOUNT,0,0);
				if (index >= listCount) { index -= 1;}
				SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_SETCURSEL,index,0);
				index = SendDlgItemMessage(hDlg,IDC_USING,LB_ADDSTRING,0,(LPARAM)String);
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETITEMDATA,index,Data);
			}
			break;
		case IDC_REMOVE:
			{
				char String[100];
				int index, listCount, Data;
				index = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETCURSEL,0,0);
				if (index < 0) { break; }
				SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETTEXT,index,(LPARAM)String);
				Data = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETITEMDATA,index,0);
				SendDlgItemMessage(hDlg,IDC_USING,LB_DELETESTRING,index,0);
				listCount = SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
				if (index >= listCount) { index -= 1;}
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETCURSEL,index,0);
				index = SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_ADDSTRING,0,(LPARAM)String);
				SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_SETITEMDATA,index,Data);
			}
			break;
		case IDC_UP:
			{
				char String[100];
				int index, Data;
				index = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETCURSEL,0,0);
				if (index <= 0) { break; }
				SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETTEXT,index,(LPARAM)String);
				Data = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETITEMDATA,index,0);
				SendDlgItemMessage(hDlg,IDC_USING,LB_DELETESTRING,index,0);
				index = SendDlgItemMessage(hDlg,IDC_USING,LB_INSERTSTRING,index - 1,(LPARAM)String);
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETCURSEL,index,0);
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETITEMDATA,index,Data);
			}
			break;
		case IDC_DOWN:
			{
				char String[100];
				int index,listCount,Data;
				index = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETCURSEL,0,0);
				listCount = SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
				if ((index + 1) == listCount) { break; }
				SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETTEXT,index,(LPARAM)String);
				Data = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETITEMDATA,index,0);
				SendDlgItemMessage(hDlg,IDC_USING,LB_DELETESTRING,index,0);
				index = SendDlgItemMessage(hDlg,IDC_USING,LB_INSERTSTRING,index + 1,(LPARAM)String);
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETCURSEL,index,0);
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETITEMDATA,index,Data);
			}
			break;
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code == PSN_APPLY) {
			char String[200], szIndex[10];
			int index, listCount, Pos;
			DWORD Disposition = 0;
			HKEY hKeyResults = 0;
			long lResult;
			Rercursion = SendMessage(GetDlgItem(hDlg,IDC_RECURSION),BM_GETSTATE, 0,0) == BST_CHECKED?TRUE:FALSE;
			sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
			lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
			if (lResult == ERROR_SUCCESS) RegSetValueEx(hKeyResults,"Directory Recursion",0,REG_DWORD,(BYTE *)&Rercursion,sizeof(DWORD));
			SaveRomBrowserColoumnInfo(); // Any coloumn width changes get saved
			listCount = SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
			for (Pos = 0; Pos < listCount; Pos ++ ){
				index = SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETITEMDATA,Pos,0);
				SaveRomBrowserColoumnPosition(index,Pos);
			}
			listCount = SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_GETCOUNT,0,0);
			strcpy(szIndex,"-1");
			for (Pos = 0; Pos < listCount; Pos ++ ){
				index = SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETITEMDATA,Pos,0);
				SaveRomBrowserColoumnPosition(index,-1);
			}
			LoadRomBrowserColoumnInfo();
			ResetRomBrowserColomuns();
			RemoveRecentList(hMainWindow);
			RomsToRemember = GetDlgItemInt(hDlg,IDC_REMEMBER,NULL,FALSE);
			if (RomsToRemember < 0) { RomsToRemember = 0; }
			if (RomsToRemember > 10) { RomsToRemember = 10; }
			RegSetValueEx(hKeyResults,"Max # of ROMs Remembered",0,REG_DWORD,(BYTE *)&RomsToRemember,sizeof(DWORD));
			RemoveRecentList(hMainWindow);
			RomDirsToRemember = GetDlgItemInt(hDlg,IDC_REMEMBERDIR,NULL,FALSE);
			if (RomDirsToRemember < 0) { RomDirsToRemember = 0; }
			if (RomDirsToRemember > 10) { RomDirsToRemember = 10; }
			RegSetValueEx(hKeyResults,"Max # of ROM Dirs Remembered",0,REG_DWORD,(BYTE *)&RomDirsToRemember,sizeof(DWORD));
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK RomNotesProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDC_STATUS_TEXT,GS(NOTE_STATUS));
		SetDlgItemText(hDlg,IDC_CORE,GS(NOTE_CORE));
		SetDlgItemText(hDlg,IDC_PLUGIN,GS(NOTE_PLUGIN));
		{
			char Identifier[100], *IniFile;
			sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
			IniFile = GetIniFileName();
			{
				char String[0x3000], RomStatus[100], Status[200], *p;
				int len, index;
				_GetPrivateProfileString(Identifier,"Status",GS(RB_NOT_IN_RDB),RomStatus,sizeof(RomStatus),IniFile);
				GetPrivateProfileSection("ROM Status",String,sizeof(String), IniFile);
				for (p = String; strlen(p) > 0; p += strlen(p) + 1) {
					strncpy(Status,p,sizeof(Status));
					if (strrchr(Status,'=') == NULL) { continue; }
					*(strrchr(Status,'=')) = 0;
					len = strlen(Status);
					if (len > 4 && _strnicmp(&Status[len-4],".Sel",4) == 0) { continue; }
					if (len > 8 && _strnicmp(&Status[len-8],".Seltext",8) == 0) { continue; }
					index = SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_ADDSTRING,0,(LPARAM)Status);
					if (strcmp(Status,RomStatus) == 0) { SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_SETCURSEL,index,0); }
					if (SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_GETCOUNT,0,0) == 0) { SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_SETCURSEL,0,0); }
				}
			}
			{
				char CoreNotes[800];
				_GetPrivateProfileString(Identifier,"Core","",CoreNotes,sizeof(CoreNotes),IniFile);
				SetDlgItemText(hDlg,IDC_CORE_NOTES,CoreNotes);
			}
			{
				char PluginNotes[800];
				_GetPrivateProfileString(Identifier,"Plugin","",PluginNotes,sizeof(PluginNotes),IniFile);
				SetDlgItemText(hDlg,IDC_PLUGIN_NOTE,PluginNotes);
			}
		}
		if (!CPURunning) {
			EnableWindow(GetDlgItem(hDlg,IDC_STATUS_TEXT),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_STATUS),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CORE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CORE_NOTES),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PLUGIN),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PLUGIN_NOTE),FALSE);
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code == PSN_APPLY) {
			char Identifier[100], string[200], *IniFile;
			sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
			IniFile = GetIniFileName();
			GetWindowText(GetDlgItem(hDlg, IDC_STATUS), string, sizeof(string));
			if (strlen(string) == 0) { strcpy(string, GS(RB_NOT_IN_RDB)); }
			_WritePrivateProfileString(Identifier,"Status",string,IniFile);
			GetWindowText(GetDlgItem(hDlg, IDC_CORE_NOTES), string, sizeof(string));
			_WritePrivateProfileString(Identifier,"Core",string,IniFile);
			GetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_NOTE), string, sizeof(string));
			_WritePrivateProfileString(Identifier,"Plugin",string,IniFile);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK RomSettingsProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int indx;
	switch (uMsg) {
	case WM_INITDIALOG:
		ReadRomOptions();
		if (strlen(RomName) != 0) {
			SetDlgItemText(hDlg, IDC_ROMSETTWARN, GS(ROMSETTWARN));
			SetDlgItemText(hDlg, IDC_INFO, GS(ADVANCE_INFO));
		}
		SetDlgItemText(hDlg,IDC_CPU_TYPE_TEXT,GS(ROM_CPU_STYLE));
		SetDlgItemText(hDlg,IDC_SELFMOD_TEXT,GS(ROM_SMCM));
		SetDlgItemText(hDlg,IDC_MEMORY_SIZE_TEXT,GS(ROM_MEM_SIZE));
		SetDlgItemText(hDlg,IDC_SAVE_TYPE_TEXT,GS(ROM_SAVE_TYPE));
		SetDlgItemText(hDlg,IDC_COUNTFACT_TEXT,GS(ROM_COUNTER_FACTOR));
		AddDropDownItem(hDlg,IDC_CPU_TYPE,ADVANCE_DEFAULTS,CPU_Default,&RomCPUType);
		AddDropDownItem(hDlg,IDC_CPU_TYPE,CORE_INTERPRETER,CPU_Interpreter,&RomCPUType);
		AddDropDownItem(hDlg,IDC_CPU_TYPE,CORE_RECOMPILER,CPU_Recompiler,&RomCPUType);
		AddDropDownItem(hDlg,IDC_SELFMOD,ADVANCE_DEFAULTS,ModCode_Default,&RomSelfMod);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_CACHE,ModCode_Cache,&RomSelfMod);
		AddDropDownItem(hDlg, IDC_SELFMOD, SMCM_CHANGE_MEM, ModCode_ChangeMemory, &RomSelfMod);
		AddDropDownItem(hDlg, IDC_SELFMOD, SMCM_CHECK_MEM, ModCode_CheckMemoryCache, &RomSelfMod);
		AddDropDownItem(hDlg, IDC_SELFMOD, SMCM_CHECK_RETURN, ModCode_CheckMemoryReturn, &RomSelfMod);
		AddDropDownItem(hDlg, IDC_SELFMOD, SMCM_CHECK_ADV, ModCode_CheckMemoryAdvance, &RomSelfMod);
		AddDropDownItem(hDlg, IDC_SELFMOD, SMCM_NONE, ModCode_None, &RomSelfMod);
		AddDropDownItem(hDlg,IDC_SELFMOD,SMCM_PROTECTED,ModCode_ProtectedMemory,&RomSelfMod);
		AddDropDownItem(hDlg,IDC_RDRAM_SIZE,ADVANCE_DEFAULTS,-1,&ROMRAMsize);
		AddDropDownItem(hDlg,IDC_RDRAM_SIZE,RDRAM_4MB,0x400000,&ROMRAMsize);
		AddDropDownItem(hDlg,IDC_RDRAM_SIZE,RDRAM_8MB,0x800000,&ROMRAMsize);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,ROM_SAVE,Auto,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_4K_eepROM,eepROM_4K,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_16K_eepROM,eepROM_16K,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_SRAM,SRAM,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_FlashRAM,FlashRAM,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_COUNTFACT,ADVANCE_DEFAULTS,-1,&RomCF);
		AddDropDownItem(hDlg,IDC_COUNTFACT,NUMBER_1,1,&RomCF);
		AddDropDownItem(hDlg,IDC_COUNTFACT,NUMBER_2,2,&RomCF);
		AddDropDownItem(hDlg,IDC_COUNTFACT,NUMBER_3,3,&RomCF);
		SetFlagControl(hDlg,&RomAudioSignal,IDC_AUDIO_SIGNAL,ROM_AUDIO_SIGNAL);
		SetFlagControl(hDlg,&RomUseLargeBuffer, IDC_LARGE_COMPILE_BUFFER, ROM_LARGE_BUFFER);
		SetFlagControl(hDlg,&RomUseTlb, IDC_USE_TLB, ROM_USE_TLB);
		SetFlagControl(hDlg,&RomUseCache, IDC_ROM_REGCACHE, ROM_REG_CACHE);
		SetFlagControl(hDlg,&RomDelaySI, IDC_DELAY_SI, ROM_DELAY_SI);
		SetFlagControl(hDlg,&RomDelayRDP, IDC_DELAY_RDP, ROM_DELAY_RDP);
		SetFlagControl(hDlg,&RomDelayRSP, IDC_DELAY_RSP, ROM_DELAY_RSP);
		SetFlagControl(hDlg,&RomAlignDMA, IDC_ALIGN_DMA, ROM_ALIGN_DMA);
		SetFlagControl(hDlg,&RomSPHack, IDC_ROM_SPHACK, ROM_SP_HACK);
		if (strlen(RomName) == 0) {
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE_TYPE_TEXT),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE_TYPE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_DELAY_SI),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_DELAY_RDP),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_DELAY_RSP),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_ROM_SPHACK),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_ROM_REGCACHE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_AUDIO_SIGNAL),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_LARGE_COMPILE_BUFFER),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_NOTES),FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CPU_TYPE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_RDRAM_SIZE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_COUNTFACT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_SELFMOD), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CPU_TYPE_TEXT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_MEMORY_SIZE_TEXT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_COUNTFACT_TEXT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_SELFMOD_TEXT), FALSE);
		}
		if (strlen(RomName) == 0 || ForceDisableTLB) EnableWindow(GetDlgItem(hDlg, IDC_USE_TLB), FALSE);
		if (strlen(RomName) == 0 || ForceEnableDMA) EnableWindow(GetDlgItem(hDlg, IDC_ALIGN_DMA), FALSE);
		if (strlen(RomName) == 0 || ForceEnableCaching) EnableWindow(GetDlgItem(hDlg, IDC_ForceEnableCaching), FALSE);
		if (strlen(RomName) == 0 || ForceEnableDelayRDP) EnableWindow(GetDlgItem(hDlg, IDC_ForceEnableDelayRDP), FALSE);
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code == PSN_APPLY) {
			if (strlen(RomName) == 0) break;
			{
				indx = SendMessage(GetDlgItem(hDlg, IDC_RDRAM_SIZE), CB_GETCURSEL, 0, 0);
				ROMRAMsize = SendMessage(GetDlgItem(hDlg, IDC_RDRAM_SIZE), CB_GETITEMDATA, indx, 0);
				indx = SendMessage(GetDlgItem(hDlg, IDC_SAVE_TYPE), CB_GETCURSEL, 0, 0);
				RomSaveUsing = SendMessage(GetDlgItem(hDlg, IDC_SAVE_TYPE), CB_GETITEMDATA, indx, 0);
				indx = SendMessage(GetDlgItem(hDlg, IDC_COUNTFACT), CB_GETCURSEL, 0, 0);
				RomCF = SendMessage(GetDlgItem(hDlg, IDC_COUNTFACT), CB_GETITEMDATA, indx, 0);
				indx = SendMessage(GetDlgItem(hDlg, IDC_CPU_TYPE), CB_GETCURSEL, 0, 0);
				RomCPUType = SendMessage(GetDlgItem(hDlg, IDC_CPU_TYPE), CB_GETITEMDATA, indx, 0);
				indx = SendMessage(GetDlgItem(hDlg, IDC_SELFMOD), CB_GETCURSEL, 0, 0);
				RomSelfMod = SendMessage(GetDlgItem(hDlg, IDC_SELFMOD), CB_GETITEMDATA, indx, 0);
				RomAudioSignal = SendMessage(GetDlgItem(hDlg, IDC_AUDIO_SIGNAL), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomDelaySI = SendMessage(GetDlgItem(hDlg, IDC_DELAY_SI), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomDelayRDP = SendMessage(GetDlgItem(hDlg, IDC_DELAY_RDP), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomDelayRSP = SendMessage(GetDlgItem(hDlg, IDC_DELAY_RSP), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomAlignDMA = SendMessage(GetDlgItem(hDlg, IDC_ALIGN_DMA), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomSPHack = SendMessage(GetDlgItem(hDlg, IDC_ROM_SPHACK), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomUseTlb = SendMessage(GetDlgItem(hDlg, IDC_USE_TLB), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomUseCache = SendMessage(GetDlgItem(hDlg, IDC_ROM_REGCACHE), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				RomUseLargeBuffer = SendMessage(GetDlgItem(hDlg, IDC_LARGE_COMPILE_BUFFER), BM_GETSTATE, 0, 0) == BST_CHECKED ? TRUE : FALSE;
			}
			SaveRomOptions();
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}