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
/********* Source Version 19 WIP 2 *********/
/********* General Defaults **********/
#define AppName						"BruceIV's Project64 1.6.2"
#define IniName						"Game.ini"
#define CheatIniName				"Cheat.ini"
#define LangFileName				"English.txt"
#define Default_AutoSleep			FALSE
#define Default_AutoHide			TRUE
#define Default_RDRAMsize			0x800000
#define Default_LimitFPS			TRUE
#define Default_SpeedCap			FALSE
#define Default_UsuallyonTop		FALSE
#define Default_BasicMode			TRUE
#define Default_RomsToRemember		10
#define Default_RomsDirsToRemember	10
#define Default_CountPerOp		2
#define Default_ForceDisableTLB		FALSE
#define Default_ForceEnableDMA		FALSE
#define Default_ForceDisableCaching	FALSE
#define Default_ForceAuto16kbit		FALSE
/*********** Menu Stuff **************/
#define ID_FILE_RECENT_FILE		1000
#define ID_FILE_RECENT_DIR		1100
#define ID_LANG_SELECT			2000
/************ CPU Core ***************/
#define CPU_Default				-1
#define CPU_Interpreter			0
#define CPU_Recompiler			1
#define Default_CPU			CPU_Recompiler
/********* Register Caching **********/
#define UseCache_Default	-1
#define REG_CACHE_ON		0
#define REG_CACHE_OFF		1
#define Default_UseCache REG_CACHE_ON // Use OFF if low-end devices aren't significantly affected
/******* Self modifying code *********/
#define ModCode_Default					-1
#define ModCode_Cache					0
#define ModCode_CheckSetMemory	1
#define ModCode_CheckMemory		2
#define ModCode_ProtectMemory			3
#define Default_SelfModCheck			ModCode_CheckMemory // ProtectMemory would be ideal but low-end devices suffer significantly with it
/********** Rom Browser **************/
#define Default_UseRB				TRUE
#define Default_Recursion			TRUE
/********* Global Variables **********/
extern LARGE_INTEGER Frequency,Frames[9],LastFrame;
extern BOOL AutoSleep,AutoHide,Recursion,LimitFPS,SpeedCap,AutoFullScreen,SystemCF,UsuallyonTop,BasicMode,BootupSettings,SetupPluginsAfterSaveRomOpt;
extern DWORD CurrentFrame,CPU_Type,SystemCPU_Type,SelfModCheck,SystemSelfModCheck,SystemUseCache,RomsToRemember,RomDirsToRemember;
extern HWND hMainWindow,hHiddenWin,hStatusWnd;
extern char CurrentSave[256];
extern HMENU hMainMenu;
extern HINSTANCE hInst;
/******** Function Prototype *********/
DWORD AsciiToHex          (char * HexValue);
int CALLBACK SelectRomDirCallBack (HWND hwnd,DWORD uMsg,DWORD lp,DWORD lpData);
void UsuallyonTopWindow    (HWND hWnd);
void HandleModal1 (HWND hWnd);
void HandleModal2(HWND hWnd);
void  __cdecl DisplayError       (char * Message,...);
void  ChangeWinSize        (HWND hWnd,long width,long height,HWND hStatusBar);
void  DisplayFPS          (void);
char* GetIniFileName      (void);
char* GetLangFileName     (void);
void  LoadSettings        (void);
void  MenuSetText         (HMENU hMenu,int MenuPos,char * Title,char * Shortcut);
void  SetCurrentSaveState (HWND hWnd,int State);
void  FixupMenubar		  (HWND hWnd);
void  SetupMenu           (HWND hWnd);
void  CreateRomListControl (HWND hWnd);
#ifdef __cplusplus
}
#endif
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")