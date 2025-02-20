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
#include "win32Timer.h"
#include "Settings API.h"
#include "rombrowser.h"
#include "Language.h"
/********* General Defaults **********/
#define AppVer   "Finale"
#define AppName  "Project64 Version 1.6.2 Finale"
#define IniName						"PJ64.rdb"
#define CheatIniName				"PJ64.cdb"
#define LangFileName				"PJ64.ldb"
#define Default_AutoSleep			TRUE
#define Default_DisableRegCaching	FALSE
#define Default_RDRAMsize			0x800000
#define Default_UseIni				TRUE
#define Default_LimitFPS			TRUE
#define Default_AlwaysOnTop			FALSE
#define Default_BasicMode			TRUE
#define Default_RememberCheats		TRUE
#define Default_RomsToRemember		10
#define Default_RomsDirsToRemember	10
#define Default_CountPerOp		2
/*********** Menu Stuff **************/
#define ID_FILE_RECENT_FILE		1000
#define ID_FILE_RECENT_DIR		1100
#define ID_LANG_SELECT			2000
/************** Core *****************/
#define CPU_Default					-1
#define CPU_Interpreter				0
#define CPU_Recompiler				1
#define Default_CPU					CPU_Recompiler
/******* Self modifying code *********/
#define ModCode_Default			-1
#define ModCode_Cache			0
#define ModCode_ChangeMemory		1
#define ModCode_CheckMemoryCache	2
#define ModCode_CheckMemoryReturn	3
#define ModCode_CheckMemoryAdvance	4
#define ModCode_None			5
#define ModCode_ProtectedMemory		6
#define Default_SelfModCheck		ModCode_CheckMemoryReturn
/********** Rom Browser **************/
#define Default_UseRB				TRUE
#define Default_Rercursion			TRUE
/********* Global Variables **********/
extern LARGE_INTEGER Frequency, Frames[8], LastFrame;
extern BOOL AutoSleep, DisableRegCaching, UseIni, UseTlb, UseLinking, RomBrowser,
	IgnoreMove, Rercursion, LimitFPS,
	AutoFullScreen, SystemCF, AlwaysOnTop, BasicMode, DelaySI, RememberCheats,
	DelayRDP, DelayRSP, AlignDMA;
extern DWORD CurrentFrame, CPU_Type, SystemCPU_Type, SelfModCheck, SystemSelfModCheck,
	RomsToRemember, RomDirsToRemember;
extern HWND hMainWindow, hHiddenWin, hStatusWnd;
extern char CurrentSave[256];
extern HMENU hMainMenu;
extern HINSTANCE hInst;
/******** Function Prototype *********/
DWORD AsciiToHex          ( char * HexValue );
void AlwaysOnTopWindow    ( HWND hWnd );
void  __cdecl DisplayError       ( char * Message, ... );
void  __cdecl DisplayErrorFatal  ( char * Message, ... );
void ChangeWinSize        ( HWND hWnd, long width, long height, HWND hStatusBar );
void  DisplayFPS          ( void );
char* GetIniFileName      ( void );
char* GetLangFileName     ( void );
int   GetStoredWinPos     ( char * WinName, DWORD * X, DWORD * Y );
int   GetStoredWinSize    ( char * WinName, DWORD * Width, DWORD * Height );
void  LoadSettings        ( void );
void  MenuSetText         ( HMENU hMenu, int MenuPos, char * Title, char * Shortcut );
void  RegisterExtension   ( char * Extension, BOOL RegisterWithPj64 );
void  SetCurrentSaveState ( HWND hWnd, int State);
void  SetupMenu           ( HWND hWnd );
void  StoreCurrentWinPos  ( char * WinName, HWND hWnd );
void  StoreCurrentWinSize ( char * WinName, HWND hWnd );
BOOL  TestExtensionRegistered ( char * Extension );
#ifdef __cplusplus
}
#endif
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")