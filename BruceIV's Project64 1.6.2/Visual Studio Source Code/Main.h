/*
*Project64 - A Nintendo 64 emulator.
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
#define __main_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <objbase.h>
#include "Types.h"
#include "Win32Timer.h"
#include "Settings API.h"
#include "ROM Browser.h"
#include "Language.h"
/*Source Code V26 R1.03. Last updated at 06:22PM 10/15/2026*/
#define AppName						"BruceIV-Project64-1.6.2"
#define IniName						"Game.ini"
#define CheatIniName				"Cheat.ini"
#define LangFileName				"English.txt"
#define Default_AutoSleep			FALSE
#define Default_RecordRGB			FALSE
#define Default_AutoHide			TRUE
#define Default_CreateStatusBar		TRUE
#define Default_VideoToScreen		FALSE
#define Default_LimitFPS			TRUE
#define Default_SpeedCap			FALSE
#define Default_UsuallyonTop		FALSE
#define Default_BasicMode			TRUE
#define Default_AutoRefreshRomBrowser TRUE
#define Default_RomsToRemember		10
#define Default_RomsDirsToRemember	10
#define Default_CountPerOp			2
#define Default_ForceDisableTLB		FALSE
#define Default_ForceEnableDMA		FALSE
#define Default_ForceAuto16kbit		FALSE
#define ID_FILE_RECENT_FILE		1000
#define ID_FILE_RECENT_DIR		1100
#define ID_LANG_SELECT			2000
#define UseCache_Default	-1
#define REG_CACHE_ON		0
#define REG_CACHE_OFF		1
#define Default_UseCache REG_CACHE_ON
#define ProtectMemoryEnlargeBuffer_Default		-1
#define PROTECT_MEMORY_ENLARGE_BUFFER_ON		0
#define PROTECT_MEMORY_ENLARGE_BUFFER_OFF		1
#define Default_UseProtectMemoryEnlargeBuffer PROTECT_MEMORY_ENLARGE_BUFFER_OFF
#define Default_Recursion			TRUE
typedef struct {
	char     szFullFileName[MAX_PATH+1];
	char     Status[60];
	char     FileName[200];
	char     InternalName[22];
	char     GameName[200];
	char     CartID[3];
	char     Developer[30];
	char     ReleaseDate[30];
	char     Genre[15];
	int		 Players;
	int      RomSize;
	BYTE     Manufacturer;
	BYTE     Country;
	DWORD    CRC1;
	DWORD    CRC2;
	int      CicChip;
	char     ForceFeedback[15];
} ROM_INFO;
typedef struct {
	int    ListCount;
	int    ListAlloc;
	ROM_INFO*List;
} ITEM_LIST;
extern ITEM_LIST ItemList;
extern LARGE_INTEGER Frequency,Frames[9],LastFrame;
extern BOOL AutoSleep,AutoHide,CreateStatusBar,RecordRGB,Recursion,LimitFPS,SpeedCap,AutoFullScreen,SystemCF,UsuallyonTop,VideoToScreen,BasicMode,AutoRefreshRomBrowser,BootupSettings,DelayHandleGlobalTasks,SPECIAL_BREAK_Trigger,SPECIAL_BREAK_Yes,FirstBoot,UserCancelPluginInit,RefreshRomBrowserOnShutdown;
extern DWORD CurrentFrame,SystemUseCache,SystemProtectMemoryEnlargeBuffer,RomsToRemember,RomDirsToRemember;
extern HWND hMainWindow,hHiddenWin,hStatusWnd,hRomList;
extern char CurrentSave[256];
extern HMENU hMainMenu;
extern HINSTANCE hInst;
DWORD AsciiToHex          (char*HexValue);
int CALLBACK SelectRomDirCallBack (HWND hwnd,DWORD uMsg,DWORD lp,DWORD lpData);
void UsuallyonTopWindow	  (HWND hWnd);
void HandleModal1		  (HWND hWnd);
void HandleModal2		  (HWND hWnd);
void  __cdecl DisplayError(char*Message,...);
void  ChangeWinSize       (HWND hWnd,long width,long height,HWND hStatusBar);
char*GetIniFileName       ();
char*GetLangFileName      ();
void  LoadSettings        ();
void  MenuSetText         (HMENU hMenu,int MenuPos,char*Title,char*Shortcut);
void  SetCurrentSaveState (HWND hWnd,int State);
void  SetupMenu           (HWND hWnd);
void CreateRomListControl (HWND hParent);
#ifdef __cplusplus
}
#endif
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
/*Credits for source code updates from other emulators:


https://github.com/project64/project64/commit/0e691d2e53ab4fb744a6b1f08d622c8ffb4da21d
https://github.com/project64/project64/commit/c7f895737ab3abc257931c4f9a7d65890a037f02
from oddMLan in...
https://github.com/pj64team/Project64-Legacy/commit/78a8a720074530b9ffff383b3d59a5017bbca167

https://github.com/project64/project64/commit/7345ecd97e131907d2d1ce6de9438dcd60793d6c
https://github.com/project64/project64/commit/7909b1850600bf3bfb2673f10830a73d425693ad
from oddMLan in...
https://github.com/pj64team/Project64-Legacy/commit/371c029d0574f8916a9d149822be89debea3ee2d

https://github.com/pj64team/Project64-Legacy/commit/2e68aa6e1781fb26a2697197a162ad7e5b39f575

https://github.com/pj64team/Project64-Legacy/commit/1b4ae1635e1409e12c2c74e84308a2aff771f7b2

https://github.com/mupen64plus/mupen64plus-core/commit/2170d5e0c9ed3c12e487eb1c5425798712b6dac8

https://github.com/pj64team/Project64-Legacy/commit/6da131efcc6a61360bc2e7a0828b3725fa168d6a

https://github.com/pj64team/Project64-Legacy/commit/d05e6faeedcbfb721f4bacfc25d22e4781988f50

https://github.com/pj64team/Project64-Legacy/commit/c1eaf72b4558f66278cca05a7f1dd464a89a87c8

https://github.com/project64/project64/commit/cd2f3cf173e88c8c1e18108f9f21405c077352e8

https://github.com/pj64team/Project64-1.6-Plus/pull/23/changes/250c21c674f6494c0d4c8fa84ea234ede8df10c0

https://github.com/aglab2/Project64-Legacy/commit/d4fc0473caa45f6de00689a3909c3ca08e8df899

N64 PIF Protection Scheme Algorithm by Azimer & Zilmar, adapted into source code by X-Scale
(see N64 CIC-NUS-6105 and 1.6 pif2 files).

Real-time clock support (for Animal Crossing?) and if (RomFileSize>PI_CART_ADDR_REG) were introduced in Project64 1.6.1 source code. All other changes were done by Edwin Bruce Shankle IV.*/