/*
*Project 64 - A Nintendo 64 emulator.
*
*(c) Copyright 2001 zilmar (zilmar@emulation64.com) and
*Jabo (jabo@emulation64.com).
*
*pj64 homepage: www.pj64.net
*
*Permission to use, copy, modify and distribute Project64 in both binary and
*source form, for non-commercial purposes, is hereby granted without fee,
*providing that this license information and copyright notice appear with
*all copies and any derived work.
*
*This software is provided 'as-is', without any express or implied
*warranty. In no event shall the authors be held liable for any damages
*arising from the use of this software.
*
*Project64 is freeware for PERSONAL USE only. Commercial users should
*seek permission of the copyright holders first. Commercial use includes
*charging money for Project64 or software derived from Project64.
*
*The copyright holders request that bug fixes and improvements to the code
*should be forwarded to them so if they want them.
*
*/
#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
#include "Plugin.h"
#include "Resource.h"
BOOL CALLBACK DefaultOptionsProc   (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK GeneralOptionsProc   (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DirSelectProc        (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK PluginSelectProc     (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK RomBrowserProc       (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK RomSettingsProc      (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK RomNotesProc         (HWND,UINT,WPARAM,LPARAM);
typedef struct {
	int     LanguageID;
	WORD    TemplateID;
	DLGPROC pfnDlgProc;
} SETTINGS_TAB;
SETTINGS_TAB SettingsTabs[]={
	{ TAB_PLUGIN,         IDD_Settings_PlugSel,  PluginSelectProc     },
	{ TAB_DIRECTORY,      IDD_Settings_Directory,DirSelectProc        },
	{ MENU_OPTIONS,        IDD_Settings_General,  GeneralOptionsProc   },
	{ TAB_ROMSELECTION,   IDD_Settings_RomBrowser,RomBrowserProc       },
	{ TAB_ADVANCED,       IDD_Settings_Options,  DefaultOptionsProc   },
	{ TAB_ROMSETTINGS,    IDD_Settings_Rom,      RomSettingsProc      },
	{ TAB_ROMNOTES,       IDD_Settings_RomNotes, RomNotesProc         },
};
SETTINGS_TAB SettingsTabsBasic[]={
	{ TAB_PLUGIN,IDD_Settings_PlugSel,PluginSelectProc               },
	{ TAB_DIRECTORY,IDD_Settings_Directory,DirSelectProc              },
	{ MENU_OPTIONS,IDD_Settings_General,GeneralOptionsProc             },
	{ TAB_ROMSELECTION,IDD_Settings_RomBrowser,RomBrowserProc         },
	{ TAB_ROMNOTES,IDD_Settings_RomNotes,RomNotesProc                 },
};
SETTINGS_TAB SettingsTabsRom[]={
	{ TAB_ROMNOTES,   IDD_Settings_RomNotes,RomNotesProc     },
};
void ChangeRomSettings(HWND hwndOwner) {
	char OrigRomName[sizeof(RomName)],OrigFileName[sizeof(CurrentFileName)];
	PROPSHEETPAGE psp[sizeof(SettingsTabsRom) / sizeof(SETTINGS_TAB)];
	BYTE OrigByteHeader[sizeof(RomHeader)];
	PROPSHEETHEADER psh;
	DWORD OrigFileSize,count;
	strncpy(OrigRomName,RomName,sizeof(OrigRomName));
	strncpy(OrigFileName,CurrentFileName,sizeof(OrigFileName));
	memcpy(OrigByteHeader,RomHeader,sizeof(RomHeader));
	strncpy(CurrentFileName,CurrentRBFileName,sizeof(CurrentFileName));
	OrigFileSize=RomFileSize;
	LoadRomHeader();
	for (count=0; count<(sizeof(SettingsTabsRom) / sizeof(SETTINGS_TAB)); count++) {
		psp[count].dwSize=sizeof(PROPSHEETPAGE);
		psp[count].dwFlags=PSP_USETITLE;
		psp[count].hInstance=hInst;
		psp[count].pszTemplate=MAKEINTRESOURCE(SettingsTabsRom[count].TemplateID);
		psp[count].pfnDlgProc=SettingsTabsRom[count].pfnDlgProc;
		psp[count].pszTitle=GS(SettingsTabsRom[count].LanguageID);
		psp[count].lParam=0;
		psp[count].pfnCallback=NULL;
	}
	psh.dwSize=sizeof(PROPSHEETHEADER);
	psh.dwFlags=PSH_PROPSHEETPAGE|PSH_NOAPPLYNOW;
	psh.hwndParent=hwndOwner;
	psh.hInstance=hInst;
	psh.pszCaption=(LPSTR)GS(MENU_SETTINGS);
	psh.nPages=sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage=0;
	psh.ppsp=(LPCPROPSHEETPAGE)&psp;
	psh.pfnCallback=NULL;
	PropertySheet(&psh);
	strncpy(RomName,OrigRomName,sizeof(RomName));
	strncpy(CurrentFileName,OrigFileName,sizeof(CurrentFileName));
	memcpy(RomHeader,OrigByteHeader,sizeof(RomHeader));
	RomFileSize=OrigFileSize;
}
void ChangeSettings(HWND hwndOwner) {
	PROPSHEETPAGE psp[sizeof(SettingsTabs) / sizeof(SETTINGS_TAB)];
	PROPSHEETPAGE BasicPsp[sizeof(SettingsTabsBasic) / sizeof(SETTINGS_TAB)];
	PROPSHEETHEADER psh;
	int count;
	for (count=0; count<(sizeof(SettingsTabs) / sizeof(SETTINGS_TAB)); count++) {
		psp[count].dwSize=sizeof(PROPSHEETPAGE);
		psp[count].dwFlags=PSP_USETITLE;
		psp[count].hInstance=hInst;
		psp[count].pszTemplate=MAKEINTRESOURCE(SettingsTabs[count].TemplateID);
		psp[count].pfnDlgProc=SettingsTabs[count].pfnDlgProc;
		psp[count].pszTitle=GS(SettingsTabs[count].LanguageID);
		psp[count].lParam=0;
		psp[count].pfnCallback=NULL;
	}
	for (count=0; count<(sizeof(SettingsTabsBasic) / sizeof(SETTINGS_TAB)); count++) {
		BasicPsp[count].dwSize=sizeof(PROPSHEETPAGE);
		BasicPsp[count].dwFlags=PSP_USETITLE;
		BasicPsp[count].hInstance=hInst;
		BasicPsp[count].pszTemplate=MAKEINTRESOURCE(SettingsTabsBasic[count].TemplateID);
		BasicPsp[count].pfnDlgProc=SettingsTabsBasic[count].pfnDlgProc;
		BasicPsp[count].pszTitle=GS(SettingsTabsBasic[count].LanguageID);
		BasicPsp[count].lParam=0;
		BasicPsp[count].pfnCallback=NULL;
	}
	psh.dwSize=sizeof(PROPSHEETHEADER);
	psh.dwFlags=PSH_PROPSHEETPAGE|PSH_NOAPPLYNOW;
	psh.hwndParent=hwndOwner;
	psh.hInstance=hInst;
	psh.pszCaption=(LPSTR)GS(MENU_SETTINGS);
	psh.nPages=(BasicMode?sizeof(BasicPsp):sizeof(psp)) / sizeof(PROPSHEETPAGE);
	psh.nStartPage=0;
	psh.ppsp=BasicMode?(LPCPROPSHEETPAGE)&BasicPsp:(LPCPROPSHEETPAGE)&psp;
	psh.pfnCallback=NULL;
	PropertySheet(&psh);
	if (!AutoSleep&&!ManualPaused&&(CPU_Paused||CPU_Action.Pause)) PauseCPU();
	if (SetupPluginsAfterSaveRomOpt) {
		SetupPluginsAfterSaveRomOpt=FALSE;
		SetupPlugins(hHiddenWin);
	} else SetupMenu(hMainWindow);
}
void SetFlagControl (HWND hDlg,BOOL*Flag,WORD CtrlID,int StringID) {
	SetDlgItemText(hDlg,CtrlID,GS(StringID));
	if (*Flag) { SendMessage(GetDlgItem(hDlg,CtrlID),BM_SETCHECK,BST_CHECKED,0); }
}
BOOL CALLBACK GeneralOptionsProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		SetFlagControl(hDlg,&AutoSleep,     IDC_AUTOSLEEP,      OPTION_AUTO_SLEEP);
		SetFlagControl(hDlg,&AutoHide,		  IDC_HIDE,			   OPTION_AUTO_HIDE);
		SetFlagControl(hDlg,&AutoFullScreen,IDC_LOAD_FULLSCREEN,OPTION_AUTO_FULLSCREEN);
		SetFlagControl(hDlg,&BasicMode,     IDC_BASIC_MODE,     OPTION_BASIC_MODE);
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			long lResult;
			HKEY hKeyResults=0;
			DWORD Disposition=0;
			char String[256];
			sprintf(String,"PJ64 V 1.6.2\\Configuration\\Settings\\Options");
			lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
			if (lResult==ERROR_SUCCESS) {
				AutoFullScreen=SendMessage(GetDlgItem(hDlg,IDC_LOAD_FULLSCREEN),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Enter Fullscreen Mode Upon ROM Opening",0,REG_DWORD,(BYTE*)&AutoFullScreen,sizeof(DWORD));
				BasicMode=SendMessage(GetDlgItem(hDlg,IDC_BASIC_MODE),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Basic Mode",0,REG_DWORD,(BYTE*)&BasicMode,sizeof(DWORD));
				AutoSleep=SendMessage(GetDlgItem(hDlg,IDC_AUTOSLEEP),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Pause CPU Upon Focus Loss",0,REG_DWORD,(BYTE*)&AutoSleep,sizeof(DWORD));
				AutoHide=SendMessage(GetDlgItem(hDlg,IDC_HIDE),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Always Hide Cursor in Fullscreen",0,REG_DWORD,(BYTE*)&AutoHide,sizeof(DWORD));
			}
			RegCloseKey(hKeyResults);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
void AddDropDownItem (HWND hDlg,WORD CtrlID,int StringID,int ItemData,int*Variable) {
	HWND hCtrl=GetDlgItem(hDlg,CtrlID);
	int indx;
	indx=SendMessage(hCtrl,CB_ADDSTRING,0,(LPARAM)GS(StringID));
	SendMessage(hCtrl,CB_SETITEMDATA,indx,ItemData);
	if (*Variable==ItemData) { SendMessage(hCtrl,CB_SETCURSEL,indx,0); }
	if (SendMessage(hCtrl,CB_GETCOUNT,0,0)==0) { SendMessage(hCtrl,CB_SETCURSEL,0,0); }
}
BOOL CALLBACK DefaultOptionsProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	int indx;
	switch (uMsg) {
	case WM_INITDIALOG:
		if (CPURunning) SetDlgItemText(hDlg,IDC_ROMSETTWARN,GS(ROMSETTWARN));
		SetDlgItemText(hDlg,IDC_CORE_DEFAULTS,GS(DEFAULT_TEXT));
		SetDlgItemText(hDlg,IDC_TEXT5,GS(ROM_COUNTER_FACTOR));
		SetDlgItemText(hDlg,IDC_TEXT6,GS(REG_CACHE));
		SetFlagControl(hDlg,&ForceDisableTLB,IDC_ForceDisableTLB,FORCE_DISABLE_TLB);
		SetFlagControl(hDlg,&ForceEnableDMA,IDC_ForceEnableDMA,FORCE_ENABLE_DMA);
		SetFlagControl(hDlg,&ForceDisableCaching,IDC_ForceDisableCaching,FORCE_DISABLE_REGISTERCACHING);
		SetFlagControl(hDlg,&ForceAuto16kbit,IDC_ForceAuto16kbit,FORCE_AUTO4kbit);
		AddDropDownItem(hDlg,IDC_REGCACHE,ON,REG_CACHE_ON,&SystemUseCache);
		AddDropDownItem(hDlg,IDC_REGCACHE,OFF,REG_CACHE_OFF,&SystemUseCache);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_1,1,&SystemCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_2,2,&SystemCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_3,3,&SystemCF);
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			long lResult;
			HKEY hKeyResults=0;
			DWORD Disposition=0;
			char String[256];
			sprintf(String,"PJ64 V 1.6.2\\Configuration\\Settings\\Advanced");
			lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
			if (lResult==ERROR_SUCCESS) {
				ForceDisableTLB=SendMessage(GetDlgItem(hDlg,IDC_ForceDisableTLB),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Always Disable TLB",0,REG_DWORD,(BYTE*)&ForceDisableTLB,sizeof(DWORD));
				ForceEnableDMA=SendMessage(GetDlgItem(hDlg,IDC_ForceEnableDMA),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Always Enable Align DMA",0,REG_DWORD,(BYTE*)&ForceEnableDMA,sizeof(DWORD));
				ForceDisableCaching=SendMessage(GetDlgItem(hDlg,IDC_ForceDisableCaching),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Always Disable Register Caching",0,REG_DWORD,(BYTE*)&ForceDisableCaching,sizeof(DWORD));
				ForceAuto16kbit=SendMessage(GetDlgItem(hDlg,IDC_ForceAuto16kbit),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"Always Autodetect With 16kbit",0,REG_DWORD,(BYTE*)&ForceAuto16kbit,sizeof(DWORD));
				indx=SendMessage(GetDlgItem(hDlg,IDC_REGCACHE),CB_GETCURSEL,0,0);
				SystemUseCache=SendMessage(GetDlgItem(hDlg,IDC_REGCACHE),CB_GETITEMDATA,indx,0);
				RegSetValueEx(hKeyResults,"Register Caching",0,REG_DWORD,(BYTE*)&SystemUseCache,sizeof(DWORD));
				indx=SendMessage(GetDlgItem(hDlg,IDC_CF),CB_GETCURSEL,0,0);
				SystemCF=SendMessage(GetDlgItem(hDlg,IDC_CF),CB_GETITEMDATA,indx,0);
				RegSetValueEx(hKeyResults,"Counter Factor",0,REG_DWORD,(BYTE*)&SystemCF,sizeof(DWORD));
			}
			RegCloseKey(hKeyResults);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK DirSelectProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			HKEY hKeyResults=0;
			char String[256];
			char Directory[256];
			long lResult;
			sprintf(String,"PJ64 V 1.6.2\\Configuration");
			lResult=RegOpenKeyEx(HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
			if (lResult==ERROR_SUCCESS) {
				DWORD Type,Value,Bytes=4;
				lResult=RegQueryValueEx(hKeyResults,"AppPath ROMs",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type!=REG_DWORD||lResult!=ERROR_SUCCESS) { Value=FALSE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_ROM_DEFAULT:IDC_ROM_OTHER),BM_SETCHECK,BST_CHECKED,0);
				lResult=RegQueryValueEx(hKeyResults,"AppPath Save Data",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type!=REG_DWORD||lResult!=ERROR_SUCCESS) { Value=TRUE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_AUTO_DEFAULT:IDC_AUTO_OTHER),BM_SETCHECK,BST_CHECKED,0);
				lResult=RegQueryValueEx(hKeyResults,"AppPath Screenshots",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type!=REG_DWORD||lResult!=ERROR_SUCCESS) { Value=TRUE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_SNAP_DEFAULT:IDC_SNAP_OTHER),BM_SETCHECK,BST_CHECKED,0);
				lResult=RegQueryValueEx(hKeyResults,"AppPath Savestates",0,&Type,(LPBYTE)(&Value),&Bytes);
				if (Type!=REG_DWORD||lResult!=ERROR_SUCCESS) { Value=TRUE; }
				SendMessage(GetDlgItem(hDlg,Value?IDC_INSTANT_DEFAULT:IDC_INSTANT_OTHER),BM_SETCHECK,BST_CHECKED,0);
				Bytes=sizeof(Directory);
				lResult=RegQueryValueEx(hKeyResults,"CustomPath Savestates",0,&Type,(LPBYTE)Directory,&Bytes);
				if (lResult!=ERROR_SUCCESS) { GetInstantSaveDir(Directory); }
				SetDlgItemText(hDlg,IDC_INSTANT_DIR,Directory);
				Bytes=sizeof(Directory);
				lResult=RegQueryValueEx(hKeyResults,"CustomPath Save Data",0,&Type,(LPBYTE)Directory,&Bytes);
				if (lResult!=ERROR_SUCCESS) { GetAutoSaveDir(Directory); }
				SetDlgItemText(hDlg,IDC_AUTO_DIR,Directory);
				Bytes=sizeof(Directory);
				lResult=RegQueryValueEx(hKeyResults,"CustomPath Screenshots",0,&Type,(LPBYTE)Directory,&Bytes);
				if (lResult!=ERROR_SUCCESS) { GetSnapShotDir(Directory); }
				SetDlgItemText(hDlg,IDC_SNAP_DIR,Directory);
			} else {
				SendMessage(GetDlgItem(hDlg,IDC_ROM_DEFAULT),BM_SETCHECK,BST_CHECKED,0);
				SendMessage(GetDlgItem(hDlg,IDC_AUTO_DEFAULT),BM_SETCHECK,BST_CHECKED,0);
				SendMessage(GetDlgItem(hDlg,IDC_INSTANT_DEFAULT),BM_SETCHECK,BST_CHECKED,0);
				SendMessage(GetDlgItem(hDlg,IDC_SNAP_DEFAULT),BM_SETCHECK,BST_CHECKED,0);
				GetInstantSaveDir(Directory);
				SetDlgItemText(hDlg,IDC_INSTANT_DIR,Directory);
				GetAutoSaveDir(Directory);
				SetDlgItemText(hDlg,IDC_AUTO_DIR,Directory);
				GetSnapShotDir(Directory);
				SetDlgItemText(hDlg,IDC_SNAP_DIR,Directory);
			}
			GetRomDirectory(Directory);
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
				char Buffer[MAX_PATH],Directory[255],Title[255];
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
				}
				bi.hwndOwner=hDlg;
				bi.pidlRoot=NULL;
				bi.pszDisplayName=Buffer;
				bi.lpszTitle=Title;
				bi.ulFlags=BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
				bi.lpfn=(BFFCALLBACK)SelectRomDirCallBack;
				bi.lParam=(DWORD)Directory;
				if ((pidl=SHBrowseForFolder(&bi))!=NULL) {
					if (SHGetPathFromIDList(pidl,Directory)) {
						int len=strlen(Directory);
						if (Directory[len-1]!='\\') { strcat(Directory,"\\"); }
						switch (LOWORD(wParam)) {
						case IDC_SELECT_ROM_DIR:
							SetDlgItemText(hDlg,IDC_ROM_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_ROM_DEFAULT),BM_SETCHECK,BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_ROM_OTHER),BM_SETCHECK,BST_CHECKED,0);
							SetRomDirectory(Directory);
							break;
						case IDC_SELECT_INSTANT_DIR:
							SetDlgItemText(hDlg,IDC_INSTANT_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_INSTANT_DEFAULT),BM_SETCHECK,BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_INSTANT_OTHER),BM_SETCHECK,BST_CHECKED,0);
							break;
						case IDC_SELECT_AUTO_DIR:
							SetDlgItemText(hDlg,IDC_AUTO_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_AUTO_DEFAULT),BM_SETCHECK,BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_AUTO_OTHER),BM_SETCHECK,BST_CHECKED,0);
							break;
						case IDC_SELECT_SNAP_DIR:
							SetDlgItemText(hDlg,IDC_SNAP_DIR,Directory);
							SendMessage(GetDlgItem(hDlg,IDC_SNAP_DEFAULT),BM_SETCHECK,BST_UNCHECKED,0);
							SendMessage(GetDlgItem(hDlg,IDC_SNAP_OTHER),BM_SETCHECK,BST_CHECKED,0);
						}
					}
				}
			}
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			long lResult;
			HKEY hKeyResults=0;
			DWORD Disposition=0;
			char String[256];
			sprintf(String,"PJ64 V 1.6.2\\Configuration");
			lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
			if (lResult==ERROR_SUCCESS) {
				DWORD Value;
				Value=SendMessage(GetDlgItem(hDlg,IDC_ROM_DEFAULT),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath ROMs",0,REG_DWORD,(BYTE*)&Value,sizeof(DWORD));
				if (Value==FALSE) {
					GetDlgItemText(hDlg,IDC_ROM_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath ROMs",0,REG_SZ,(CONST BYTE*)String,strlen(String));
				}
				Value=SendMessage(GetDlgItem(hDlg,IDC_AUTO_DEFAULT),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath Save Data",0,REG_DWORD,(BYTE*)&Value,sizeof(DWORD));
				if (Value==FALSE) {
					GetDlgItemText(hDlg,IDC_AUTO_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath Save Data",0,REG_SZ,(CONST BYTE*)String,strlen(String));
				}
				Value=SendMessage(GetDlgItem(hDlg,IDC_INSTANT_DEFAULT),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath Savestates",0,REG_DWORD,(BYTE*)&Value,sizeof(DWORD));
				if (Value==FALSE) {
					GetDlgItemText(hDlg,IDC_INSTANT_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath Savestates",0,REG_SZ,(CONST BYTE*)String,strlen(String));
				}
				Value=SendMessage(GetDlgItem(hDlg,IDC_SNAP_DEFAULT),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RegSetValueEx(hKeyResults,"AppPath Screenshots",0,REG_DWORD,(BYTE*)&Value,sizeof(DWORD));
				if (Value==FALSE) {
					GetDlgItemText(hDlg,IDC_SNAP_DIR,String,sizeof(String));
					RegSetValueEx(hKeyResults,"CustomPath Screenshots",0,REG_SZ,(CONST BYTE*)String,strlen(String));
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
BOOL PluginsChanged (HWND hDlg) {
	DWORD index;
	index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETCURSEL,0,0);
	index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index>=0) {
		if(_stricmp(RSPDLL,PluginNames[index])!=0) return TRUE;
	}
	index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETCURSEL,0,0);
	index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index>=0) {
		if(_stricmp(GfxDLL,PluginNames[index])!=0) return TRUE;
	}
	index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETCURSEL,0,0);
	index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index>=0) {
		if(_stricmp(AudioDLL,PluginNames[index])!=0) return TRUE;
	}
	index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETCURSEL,0,0);
	index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETITEMDATA,(WPARAM)index,0);
	if ((int)index>=0) {
		if(_stricmp(ControllerDLL,PluginNames[index])!=0) return TRUE;
	}
	return FALSE;
}
void FreePluginList() {
	unsigned int count;
	for (count=0; count <	PluginCount; count++) {
		free(PluginNames[count]);
	}
	PluginCount=0;
}
BOOL CALLBACK PluginSelectProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	char Plugin[300];
	HANDLE hLib;
	DWORD index;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetupPluginScreen(hDlg);
		if (CPURunning) SetDlgItemText(hDlg,IDC_ROMSETTWARN,GS(ROMSETTWARN));
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case RSP_LIST:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETCURSEL,0,0);
				if (index==CB_ERR) { break; }
				index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib=LoadLibrary(Plugin);
				if (hLib==NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				RSPDLLAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,RSP_ABOUT),RSPDLLAbout!=NULL?TRUE:FALSE);
			}
			break;
		case GFX_LIST:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETCURSEL,0,0);
				if (index==CB_ERR) { break; }
				index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib=LoadLibrary(Plugin);
				if (hLib==NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				GFXDllAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,GFX_ABOUT),GFXDllAbout!=NULL?TRUE:FALSE);
			}
			if (BootupSettings) PostMessage(GetParent(hDlg),WM_COMMAND,IDCANCEL,0);
			break;
		case AUDIO_LIST:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETCURSEL,0,0);
				if (index==CB_ERR) { break; }
				index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib=LoadLibrary(Plugin);
				if (hLib==NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				AiDllAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,GFX_ABOUT),GFXDllAbout!=NULL?TRUE:FALSE);
			}
			break;
		case CONT_LIST:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETCURSEL,0,0);
				if (index==CB_ERR) { break; }
				index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				GetPluginDir(Plugin);
				strcat(Plugin,PluginNames[index]);
				hLib=LoadLibrary(Plugin);
				if (hLib==NULL) { DisplayError("%s %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin); }
				ContDllAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,CONT_ABOUT),ContDllAbout!=NULL?TRUE:FALSE);
			}
			break;
		case RSP_ABOUT: RSPDLLAbout(hDlg); break;
		case GFX_ABOUT: GFXDllAbout(hDlg); break;
		case CONT_ABOUT: ContDllAbout(hDlg); break;
		case AUDIO_ABOUT: AiDllAbout(hDlg);
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			long lResult;
			HKEY hKeyResults=0;
			DWORD Disposition=0;
			char String[256];
			if (PluginsChanged(hDlg)==FALSE) { FreePluginList(); break; }
			if (!CPURunning) SetupPluginsAfterSaveRomOpt=TRUE;
			sprintf(String,"PJ64 V 1.6.2\\Configuration\\Dll");
			lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
			if (lResult==ERROR_SUCCESS) {
				DWORD index;
				index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETCURSEL,0,0);
				index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"RSP Dll",0,REG_SZ,(CONST BYTE*)String,
					strlen(String));
				index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETCURSEL,0,0);
				index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"Graphics Dll",0,REG_SZ,(CONST BYTE*)String,
					strlen(String));
				index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETCURSEL,0,0);
				index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"Audio Dll",0,REG_SZ,(CONST BYTE*)String,
					strlen(String));
				index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETCURSEL,0,0);
				index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_GETITEMDATA,(WPARAM)index,0);
				sprintf(String,"%s",PluginNames[index]);
				RegSetValueEx(hKeyResults,"Controller Dll",0,REG_SZ,(CONST BYTE*)String,
					strlen(String));
			}
			RegCloseKey(hKeyResults);
			FreePluginList();
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
void RomAddFieldToList (HWND hDlg,char*Name,int Pos,int ID) {
	int listCount,index;
	if (Pos<0) {
		index=SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_ADDSTRING,0,(LPARAM)Name);
		SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_SETITEMDATA,index,ID);
		return;
	}
	listCount=SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
	if (Pos>listCount) { Pos=listCount; }
	index=SendDlgItemMessage(hDlg,IDC_USING,LB_INSERTSTRING,Pos,(LPARAM)Name);
	SendDlgItemMessage(hDlg,IDC_USING,LB_SETITEMDATA,index,ID);
}
BOOL CALLBACK RomBrowserProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		if (Recursion) { SendMessage(GetDlgItem(hDlg,IDC_RECURSION),BM_SETCHECK,BST_CHECKED,0); }
		{
			int count;
			for (count=0; count<NoOfFields; count++) {
				RomAddFieldToList(hDlg,GS(RomBrowserFields[count].LangID),RomBrowserFields[count].Pos,count);
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
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_ADD:
			{
				char String[100];
				int index,listCount,Data;
				index=SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETCURSEL,0,0);
				if (index<0) { break; }
				SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETTEXT,index,(LPARAM)String);
				Data=SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETITEMDATA,index,0);
				SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_DELETESTRING,index,0);
				listCount=SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_GETCOUNT,0,0);
				if (index>=listCount) { index -=1;}
				SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_SETCURSEL,index,0);
				index=SendDlgItemMessage(hDlg,IDC_USING,LB_ADDSTRING,0,(LPARAM)String);
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETITEMDATA,index,Data);
			}
			break;
		case IDC_REMOVE:
			{
				char String[100];
				int index,listCount,Data;
				index=SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETCURSEL,0,0);
				if (index<0) { break; }
				SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETTEXT,index,(LPARAM)String);
				Data=SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETITEMDATA,index,0);
				SendDlgItemMessage(hDlg,IDC_USING,LB_DELETESTRING,index,0);
				listCount=SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
				if (index>=listCount) { index -=1;}
				SendDlgItemMessage(hDlg,IDC_USING,LB_SETCURSEL,index,0);
				index=SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_ADDSTRING,0,(LPARAM)String);
				SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_SETITEMDATA,index,Data);
			}
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			char String[256],szIndex[10];
			int index,listCount,Pos;
			DWORD Disposition=0;
			HKEY hKeyResults=0;
			long lResult;
			Recursion=SendMessage(GetDlgItem(hDlg,IDC_RECURSION),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
			sprintf(String,"PJ64 V 1.6.2\\Configuration\\Settings\\ROM Selection");
			lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
			if (lResult==ERROR_SUCCESS) RegSetValueEx(hKeyResults,"Directory Recursion",0,REG_DWORD,(BYTE*)&Recursion,sizeof(DWORD));
			SaveRomBrowserColoumnInfo();
			listCount=SendDlgItemMessage(hDlg,IDC_USING,LB_GETCOUNT,0,0);
			for (Pos=0; Pos<listCount; Pos++) {
				index=SendMessage(GetDlgItem(hDlg,IDC_USING),LB_GETITEMDATA,Pos,0);
				SaveRomBrowserColoumnPosition(index,Pos);
			}
			listCount=SendDlgItemMessage(hDlg,IDC_AVAILABLE,LB_GETCOUNT,0,0);
			strcpy(szIndex,"-1");
			for (Pos=0; Pos<listCount; Pos++) {
				index=SendMessage(GetDlgItem(hDlg,IDC_AVAILABLE),LB_GETITEMDATA,Pos,0);
				SaveRomBrowserColoumnPosition(index,-1);
			}
			LoadRomBrowserColoumnInfo();
			ResetRomBrowserColomuns();
			RomsToRemember=GetDlgItemInt(hDlg,IDC_REMEMBER,NULL,FALSE);
			if (RomsToRemember<0) { RomsToRemember=0; }
			if (RomsToRemember>10) { RomsToRemember=10; }
			RegSetValueEx(hKeyResults,"Max # of ROMs Remembered",0,REG_DWORD,(BYTE*)&RomsToRemember,sizeof(DWORD));
			RomDirsToRemember=GetDlgItemInt(hDlg,IDC_REMEMBERDIR,NULL,FALSE);
			if (RomDirsToRemember<0) { RomDirsToRemember=0; }
			if (RomDirsToRemember>10) { RomDirsToRemember=10; }
			RegSetValueEx(hKeyResults,"Max # of ROM Dirs Remembered",0,REG_DWORD,(BYTE*)&RomDirsToRemember,sizeof(DWORD));
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK RomNotesProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDC_STATUS_TEXT,GS(NOTE_STATUS));
		SetDlgItemText(hDlg,IDC_CORE,GS(NOTE_CORE));
		SetDlgItemText(hDlg,IDC_PLUGIN,GS(NOTE_PLUGIN));
		{
			char Identifier[100],*IniFile;
			sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD*)(&RomHeader[0x10]),*(DWORD*)(&RomHeader[0x14]),RomHeader[0x3D]);
			IniFile=GetIniFileName();
			{
				char String[0x3000],RomStatus[100],Status[200],*p;
				int len,index;
				_GetPrivateProfileString(Identifier,"Status",GS(UNKNOWN),RomStatus,sizeof(RomStatus),IniFile);
				GetPrivateProfileSection("ROM Status",String,sizeof(String),IniFile);
				for (p=String; strlen(p)>0; p+=strlen(p)+1) {
					strncpy(Status,p,sizeof(Status));
					if (strrchr(Status,'=')==NULL) continue;
					*(strrchr(Status,'='))=0;
					len=strlen(Status);
					if (len>4&&_strnicmp(&Status[len-4],".Sel",4)==0) continue;
					index=SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_ADDSTRING,0,(LPARAM)Status);
					if (strcmp(Status,RomStatus)==0) { SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_SETCURSEL,index,0); }
					if (SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_GETCOUNT,0,0)==0) { SendMessage(GetDlgItem(hDlg,IDC_STATUS),CB_SETCURSEL,0,0); }
				}
			}
			{
				char CoreNotes[800];
				_GetPrivateProfileString(Identifier,"Core Note","",CoreNotes,sizeof(CoreNotes),IniFile);
				SetDlgItemText(hDlg,IDC_CORE_NOTES,CoreNotes);
			}
			{
				char PluginNotes[800];
				_GetPrivateProfileString(Identifier,"Plugin Note","",PluginNotes,sizeof(PluginNotes),IniFile);
				SetDlgItemText(hDlg,IDC_PLUGIN_NOTE,PluginNotes);
			}
		}
		if (strlen(RomName)==0) {
			EnableWindow(GetDlgItem(hDlg,IDC_STATUS_TEXT),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_STATUS),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CORE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CORE_NOTES),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PLUGIN),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PLUGIN_NOTE),FALSE);
		}
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			char Identifier[100],string[256],*IniFile;
			sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD*)(&RomHeader[0x10]),*(DWORD*)(&RomHeader[0x14]),RomHeader[0x3D]);
			IniFile=GetIniFileName();
			GetWindowText(GetDlgItem(hDlg,IDC_STATUS),string,sizeof(string));
			if (strlen(string)==0) { strcpy(string,GS(UNKNOWN)); }
			_WritePrivateProfileString(Identifier,"Status",string,IniFile);
			GetWindowText(GetDlgItem(hDlg,IDC_CORE_NOTES),string,sizeof(string));
			_WritePrivateProfileString(Identifier,"Core Note",string,IniFile);
			GetWindowText(GetDlgItem(hDlg,IDC_PLUGIN_NOTE),string,sizeof(string));
			_WritePrivateProfileString(Identifier,"Plugin Note",string,IniFile);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK RomSettingsProc (HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	int indx;
	switch (uMsg) {
	case WM_INITDIALOG:
		ReadRomOptions();
		if (CPURunning) SetDlgItemText(hDlg,IDC_ROMSETTWARN,GS(ROMSETTWARN));
		SetDlgItemText(hDlg,IDC_REGCACHE_TEXT,GS(REG_CACHE));
		SetDlgItemText(hDlg,IDC_SAVE_TYPE_TEXT,GS(ROM_SAVE_TYPE));
		SetDlgItemText(hDlg,IDC_CF_TEXT,GS(ROM_COUNTER_FACTOR));
		SetDlgItemText(hDlg,IDC_RECOMPILER_SETTINGS,GS(CORE_RECOMPILER));
		AddDropDownItem(hDlg,IDC_REGCACHE,DEFAULT_TEXT,UseCache_Default,&RomUseCache);
		AddDropDownItem(hDlg,IDC_REGCACHE,ON,REG_CACHE_ON,&RomUseCache);
		AddDropDownItem(hDlg,IDC_REGCACHE,OFF,REG_CACHE_OFF,&RomUseCache);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,DEFAULT_TEXT,Auto,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_4K_EEPROM,EEPROM_4K,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_16K_EEPROM,EEPROM_16K,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_SRAM,SRAM,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_SAVE_TYPE,SAVE_FlashRAM,FlashRAM,&RomSaveUsing);
		AddDropDownItem(hDlg,IDC_CF,DEFAULT_TEXT,-1,&RomCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_1,1,&RomCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_2,2,&RomCF);
		AddDropDownItem(hDlg,IDC_CF,NUMBER_3,3,&RomCF);
		SetFlagControl(hDlg,&RomAudioSignal,IDC_AUDIO_SIGNAL,ROM_AUDIO_SIGNAL);
		SetFlagControl(hDlg,&RomJAI,IDC_JAI,ROM_JAI);
		SetFlagControl(hDlg,&RomFiftyNineHertz,IDC_FiftyNineHertz,FIFTYNINEHERTZ_LANG);
		SetFlagControl(hDlg,&RomUseTLB,IDC_USE_TLB,ROM_USE_TLB);
		SetFlagControl(hDlg,&RomShankleAziAI,SHANKLE_AZI_AI,SHANKLE_AZI_AI_LANG);
		SetFlagControl(hDlg,&RomAltEmulateAI,ALTERNATE_EMUAI,ALTERNATE_EMUAI_LANG);
		SetFlagControl(hDlg,&RomSyncGametoAudio,IDC_SyncGametoAudio,SyncGametoAudio_LANG);
		SetFlagControl(hDlg,&RomDelaySI,IDC_DELAY_SI,ROM_DELAY_SI);
		SetFlagControl(hDlg,&RomRspRecompiler,IDC_RSP_RECOMPILER,FORCE_RSP_RECOMPILER);
		SetFlagControl(hDlg,&RomCpuRecompiler,IDC_CPU_RECOMPILER,CORE_RECOMPILER);
		SetFlagControl(hDlg,&RomProtectMemory,IDC_PROTECT_MEMORY,SMCM_PROTECTED);
		SetFlagControl(hDlg,&RomJumperPak,IDC_JUMPER_PAK,JUMPER_PAK);
		SetFlagControl(hDlg,&RomDelayRDP,IDC_DELAY_RDP,ROM_DELAY_RDP);
		SetFlagControl(hDlg,&RomDelayRSP,IDC_DELAY_RSP,ROM_DELAY_RSP);
		SetFlagControl(hDlg,&RomAlignDMA,IDC_ALIGN_DMA,ROM_ALIGN_DMA);
		SetFlagControl(hDlg,&RomCF1CF0,IDC_CF1_CF0,CF1_CF0);
		if (strlen(RomName)==0) {
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE_TYPE_TEXT),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SAVE_TYPE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_DELAY_SI),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_DELAY_RDP),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_DELAY_RSP),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_AUDIO_SIGNAL),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_NOTES),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CF_TEXT),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CF),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_FiftyNineHertz),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CPU_RECOMPILER),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_JUMPER_PAK),FALSE);
		}
		if (strlen(RomName)==0||ForceDisableCaching||!RomCpuRecompiler) {
			EnableWindow(GetDlgItem(hDlg,IDC_REGCACHE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_REGCACHE_TEXT),FALSE);
		}
		if (strlen(RomName)==0||!RomCpuRecompiler) EnableWindow(GetDlgItem(hDlg,IDC_RECOMPILER_SETTINGS),FALSE);
		if (strlen(RomName)==0||!RomCpuRecompiler||ForceDisableTLB) EnableWindow(GetDlgItem(hDlg,IDC_USE_TLB),FALSE);
		if (strlen(RomName)==0||ForceEnableDMA) EnableWindow(GetDlgItem(hDlg,IDC_ALIGN_DMA),FALSE);
		if (strlen(RomName)==0||!RomCpuRecompiler) EnableWindow(GetDlgItem(hDlg,IDC_PROTECT_MEMORY),FALSE);
		if (strlen(RomName)==0||!RomCpuRecompiler||RomCF!=-1&&RomCF!=1) EnableWindow(GetDlgItem(hDlg,IDC_CF1_CF0),FALSE);
		if (strlen(RomName)==0||strcmp(RSPDLL,"RSP.dll")!=0||strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")!=0||strcmp(RomName,"THE LEGEND OF ZELDA")==0||strcmp(RomName,"THE MASK OF MUJURA")==0||strcmp(RomName,"ZELDA MAJORA'S MASK")==0||strcmp(RomName,"BANJO KAZOOIE 2")==0||strcmp(RomName,"BANJO TOOIE")==0||strcmp(RomName,"CONKER BFD")==0||strcmp(RomName,"DONKEY KONG 64")==0||strcmp(RomName,"JET FORCE GEMINI")==0||strcmp(RomName,"STAR TWINS")==0||strcmp(RomName,"Perfect Dark")==0) EnableWindow(GetDlgItem(hDlg,IDC_RSP_RECOMPILER),FALSE);
		if (strlen(RomName)==0||!RomJAI&&!RomShankleAziAI) EnableWindow(GetDlgItem(hDlg,ALTERNATE_EMUAI),FALSE);
		if (strlen(RomName)==0||strcmp(AudioDLL,"Shankle_Audio.dll")!=0) EnableWindow(GetDlgItem(hDlg,SHANKLE_AZI_AI),FALSE);
		if (strlen(RomName)==0||strcmp(AudioDLL,"Jabo_Dsound.dll")!=0) EnableWindow(GetDlgItem(hDlg,IDC_JAI),FALSE);
		if (strlen(RomName)==0||RomJAI||strcmp(AudioDLL,"Jabo_Dsound.dll")!=0) EnableWindow(GetDlgItem(hDlg,IDC_SyncGametoAudio),FALSE);
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR*) lParam)->code==PSN_APPLY) {
			if (strlen(RomName)==0) break;
			{
				indx=SendMessage(GetDlgItem(hDlg,IDC_SAVE_TYPE),CB_GETCURSEL,0,0);
				RomSaveUsing=SendMessage(GetDlgItem(hDlg,IDC_SAVE_TYPE),CB_GETITEMDATA,indx,0);
				indx=SendMessage(GetDlgItem(hDlg,IDC_CF),CB_GETCURSEL,0,0);
				RomCF=SendMessage(GetDlgItem(hDlg,IDC_CF),CB_GETITEMDATA,indx,0);
				indx=SendMessage(GetDlgItem(hDlg,IDC_REGCACHE),CB_GETCURSEL,0,0);
				RomUseCache=SendMessage(GetDlgItem(hDlg,IDC_REGCACHE),CB_GETITEMDATA,indx,0);
				RomAudioSignal=SendMessage(GetDlgItem(hDlg,IDC_AUDIO_SIGNAL),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomDelaySI=SendMessage(GetDlgItem(hDlg,IDC_DELAY_SI),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomRspRecompiler=SendMessage(GetDlgItem(hDlg,IDC_RSP_RECOMPILER),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomCpuRecompiler=SendMessage(GetDlgItem(hDlg,IDC_CPU_RECOMPILER),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomProtectMemory=SendMessage(GetDlgItem(hDlg,IDC_PROTECT_MEMORY),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomJumperPak=SendMessage(GetDlgItem(hDlg,IDC_JUMPER_PAK),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomDelayRDP=SendMessage(GetDlgItem(hDlg,IDC_DELAY_RDP),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomDelayRSP=SendMessage(GetDlgItem(hDlg,IDC_DELAY_RSP),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomAlignDMA=SendMessage(GetDlgItem(hDlg,IDC_ALIGN_DMA),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomCF1CF0=SendMessage(GetDlgItem(hDlg,IDC_CF1_CF0),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomUseTLB=SendMessage(GetDlgItem(hDlg,IDC_USE_TLB),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomFiftyNineHertz=SendMessage(GetDlgItem(hDlg,IDC_FiftyNineHertz),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomShankleAziAI=SendMessage(GetDlgItem(hDlg,SHANKLE_AZI_AI),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomAltEmulateAI=SendMessage(GetDlgItem(hDlg,ALTERNATE_EMUAI),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomSyncGametoAudio=SendMessage(GetDlgItem(hDlg,IDC_SyncGametoAudio),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
				RomJAI=SendMessage(GetDlgItem(hDlg,IDC_JAI),BM_GETSTATE,0,0)==BST_CHECKED?TRUE:FALSE;
			}
			SaveRomOptions();
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}