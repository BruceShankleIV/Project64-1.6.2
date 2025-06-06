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
#include <stdio.h>
#include "main.h"
#include "cpu.h"
#include "memory.h"
#include "plugin.h"
#include "cheats.h"
#include "unzip.h"
#include "resource.h"
#include "RomTools_Common.h"
#define MenuLocOfUsedFiles	11
#define MenuLocOfUsedDirs	(MenuLocOfUsedFiles + 1)
DWORD RomFileSize, ROMRAMsize, RomSaveUsing, RomCPUType, RomSelfMod,
RomUseTlb, RomAudioSignal, RomCF, RomUseLargeBuffer, RomUseCache,
	RomDelaySI, RomSPHack, RomDelayRDP, RomDelayRSP, RomAlignDMA;
char CurrentFileName[MAX_PATH + 1] = { "" }, RomName[MAX_PATH + 1] = { "" }, RomHeader[0x1000];
char LastRoms[10][MAX_PATH+1], LastDirs[10][MAX_PATH+1];
BOOL IsValidRomImage ( BYTE Test[4] );
void AddRecentDir(HWND hWnd, char * addition) {
	DWORD count;
	if (addition != NULL && RomDirsToRemember > 0) {
		char Dir[MAX_PATH+1];
		BOOL bFound = FALSE;
		strcpy(Dir,addition);
		for (count = 0; count < RomDirsToRemember && !bFound; count ++ ) {
			if (strcmp(addition, LastDirs[count]) == 0) {
				if (count != 0) {
					memmove(&LastDirs[1],&LastDirs[0],sizeof(LastDirs[0]) * count);
				}
				bFound = TRUE;
			}
		}
		if (bFound == FALSE) { memmove(&LastDirs[1],&LastDirs[0],sizeof(LastDirs[0]) * (RomDirsToRemember - 1)); }
		strcpy(LastDirs[0],Dir);
		SaveRecentDirs();
	}
	SetupMenu(hMainWindow);
}
void AddRecentFile(HWND hWnd, char * addition) {
	DWORD count;
	if (addition != NULL && RomsToRemember > 0) {
		char Rom[MAX_PATH+1];
		BOOL bFound = FALSE;
		strcpy(Rom,addition);
		for (count = 0; count < RomsToRemember && !bFound; count ++ ) {
			if (strcmp(addition, LastRoms[count]) == 0) {
				if (count != 0) {
					memmove(&LastRoms[1],&LastRoms[0],sizeof(LastRoms[0]) * count);
				}
				bFound = TRUE;
			}
		}
		if (bFound == FALSE) { memmove(&LastRoms[1],&LastRoms[0],sizeof(LastRoms[0]) * (RomsToRemember - 1)); }
		strcpy(LastRoms[0],Rom);
		SaveRecentFiles();
	}
	SetupMenu(hMainWindow);
}
void ByteSwapRom (BYTE * Rom, DWORD RomLen) {
	DWORD count;
	switch (*((DWORD *)&Rom[0])) {
	case 0x12408037:
		for( count = 0 ; count < RomLen; count += 4 ) {
			Rom[count] ^= Rom[count+2];
			Rom[count + 2] ^= Rom[count];
			Rom[count] ^= Rom[count+2];
			Rom[count + 1] ^= Rom[count + 3];
			Rom[count + 3] ^= Rom[count + 1];
			Rom[count + 1] ^= Rom[count + 3];
		}
		break;
	case 0x40072780:
	case 0x40123780:
		for( count = 0 ; count < RomLen; count += 4 ) {
			Rom[count] ^= Rom[count+3];
			Rom[count + 3] ^= Rom[count];
			Rom[count] ^= Rom[count+3];
			Rom[count + 1] ^= Rom[count + 2];
			Rom[count + 2] ^= Rom[count + 1];
			Rom[count + 1] ^= Rom[count + 2];
		}
		break;
	case 0x80371240: break;
	default:
		DisplayError(GS(MSG_UNKNOWN_FILE_FORMAT));
	}
}
void GetRomDirectory ( char * Directory ) {
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath( path_buffer, drive, dir, fname, ext );
	sprintf(Group,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	sprintf(Directory,"%s%sROMs (.z64 .v64 .n64)\\",drive,dir);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Bytes = sizeof(Dir);
		lResult = RegQueryValueEx(hKeyResults,"CustomPath ROMs",0,&Type,(LPBYTE)Dir,&Bytes);
		if (lResult == ERROR_SUCCESS) { strcpy(Directory,Dir); }
	}
	RegCloseKey(hKeyResults);
}
BOOL IsValidRomImage ( BYTE Test[4] ) {
	if ( *((DWORD *)&Test[0]) == 0x40123780 ) { return TRUE; }
	if ( *((DWORD *)&Test[0]) == 0x12408037 ) { return TRUE; }
	if ( *((DWORD *)&Test[0]) == 0x80371240 ) { return TRUE; }
	if ( *((DWORD *)&Test[0]) == 0x40072780 ) { return TRUE; }
	return FALSE;
}
BOOL LoadDataFromRomFile(char * FileName,BYTE * Data,int DataLen, int * RomSize) {
	BYTE Test[4];
        int count;
	if (_strnicmp(&FileName[strlen(FileName)-4], ".ZIP",4) == 0 ){
		int len, port = 0, FoundRom;
	    unz_file_info info;
		char zname[132];
		unzFile file;
		file = unzOpen(FileName);
		if (file == NULL) { return FALSE; }
		port = unzGoToFirstFile(file);
		FoundRom = FALSE;
		while(port == UNZ_OK && FoundRom == FALSE) {
			unzGetCurrentFileInfo(file, &info, zname, 128, NULL,0, NULL,0);
		    if (unzLocateFile(file, zname, 1) != UNZ_OK ) {
				unzClose(file);
				return FALSE;
			}
			if( unzOpenCurrentFile(file) != UNZ_OK ) {
				unzClose(file);
				return FALSE;
			}
			unzReadCurrentFile(file,Test,4);
			if (IsValidRomImage(Test)) {
				FoundRom = TRUE;
				RomFileSize = info.uncompressed_size;
				memcpy(Data,Test,4);
				len = unzReadCurrentFile(file,&Data[4],DataLen - 4) + 4;
				if ((int)DataLen != len) {
					unzCloseCurrentFile(file);
					unzClose(file);
					return FALSE;
				}
				*RomSize = info.uncompressed_size;
				if(unzCloseCurrentFile(file) == UNZ_CRCERROR) {
					unzClose(file);
					return FALSE;
				}
				unzClose(file);
			}
			if (FoundRom == FALSE) {
				unzCloseCurrentFile(file);
				port = unzGoToNextFile(file);
			}
		}
		if (FoundRom == FALSE) {
			unzClose(file);
			return FALSE;
		}
	} else {
		DWORD dwRead;
		HANDLE hFile;
		hFile = CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
			NULL);
		if (hFile == INVALID_HANDLE_VALUE) {  return FALSE; }
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,Test,4,&dwRead,NULL);
		if (!IsValidRomImage(Test)) { CloseHandle( hFile ); return FALSE; }
		RomFileSize = GetFileSize(hFile,NULL);
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		if (!ReadFile(hFile,Data,DataLen,&dwRead,NULL)) { CloseHandle( hFile ); return FALSE; }
		*RomSize = GetFileSize(hFile,NULL);
		CloseHandle( hFile );
	}
	switch (*((DWORD *)&Data[0])) {
	case 0x12408037:
		for( count = 0 ; count < DataLen; count += 4 ) {
			Data[count] ^= Data[count+2];
			Data[count + 2] ^= Data[count];
			Data[count] ^= Data[count+2];
			Data[count + 1] ^= Data[count + 3];
			Data[count + 3] ^= Data[count + 1];
			Data[count + 1] ^= Data[count + 3];
		}
		break;
	case 0x40072780:
	case 0x40123780:
		for( count = 0 ; count < DataLen; count += 4 ) {
			Data[count] ^= Data[count+3];
			Data[count + 3] ^= Data[count];
			Data[count] ^= Data[count+3];
			Data[count + 1] ^= Data[count + 2];
			Data[count + 2] ^= Data[count + 1];
			Data[count + 1] ^= Data[count + 2];
		}
		break;
	case 0x80371240: break;
	}
	return TRUE;
}
void CreateRecentDirList (HMENU hMenu) {
	HKEY hKeyResults = 0;
	char String[256];
	long lResult;
	DWORD count;
	sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Bytes;
		for (count = 0; count < RomDirsToRemember; count++) {
			Bytes = sizeof(LastDirs[count]);
			sprintf(String,"RecentDir%d",count+1);
			lResult = RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)LastDirs[count],&Bytes);
			if (lResult != ERROR_SUCCESS) {
				memset(LastDirs[count],0,sizeof(LastDirs[count]));
				break;
			}
		}
		RegCloseKey(hKeyResults);
	}
	{
		HMENU hSubMenu;
		MENUITEMINFO menuinfo;
		hSubMenu = GetSubMenu(hMenu,0);
		DeleteMenu(hSubMenu, MenuLocOfUsedDirs, MF_BYPOSITION);
		if (!RomListVisible()) return;
		memset(&menuinfo, 0, sizeof(MENUITEMINFO));
		menuinfo.cbSize = sizeof(MENUITEMINFO);
		menuinfo.fMask = MIIM_TYPE|MIIM_ID;
		menuinfo.fType = MFT_STRING;
		menuinfo.fState = MFS_ENABLED;
		menuinfo.dwTypeData = String;
		menuinfo.cch = 256;
		sprintf(String," ");
		InsertMenuItem(hSubMenu, MenuLocOfUsedDirs, TRUE, &menuinfo);
		hSubMenu = CreateMenu();
		if (strlen(LastDirs[0]) == 0) {
			menuinfo.wID = ID_FILE_RECENT_DIR;
			sprintf(String," ");
			InsertMenuItem(hSubMenu, 0, TRUE, &menuinfo);
		}
		menuinfo.fMask = MIIM_TYPE|MIIM_ID;
		for (count = 0; count < RomDirsToRemember; count ++ ) {
			if (strlen(LastDirs[count]) == 0) { break; }
			menuinfo.wID = ID_FILE_RECENT_DIR + count;
			sprintf(String,"&%d %s",(count + 1) % 10,LastDirs[count]);
			InsertMenuItem(hSubMenu, count, TRUE, &menuinfo);
		}
		ModifyMenu(GetSubMenu(hMenu,0),MenuLocOfUsedDirs,MF_POPUP|MF_BYPOSITION,(DWORD)hSubMenu,GS(MENU_RECENT_DIR));
		if (strlen(LastDirs[0]) == 0 || CPURunning || RomDirsToRemember == 0) {
			EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedDirs,MF_BYPOSITION|MFS_DISABLED);
		} else {
			EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedDirs,MF_BYPOSITION|MFS_ENABLED);
		}
	}
}
void CreateRecentFileList(HMENU hMenu) {
	HKEY hKeyResults = 0;
	char String[256];
	long lResult;
	DWORD count;
	sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Bytes;
		for (count = 0; count < RomsToRemember; count++) {
			Bytes = sizeof(LastRoms[count]);
			sprintf(String, "RecentFile%d", count + 1);
			lResult = RegQueryValueEx(hKeyResults, String, 0, &Type, (LPBYTE)LastRoms[count], &Bytes);
			if (lResult != ERROR_SUCCESS) {
				memset(LastRoms[count], 0, sizeof(LastRoms[count]));
				break;
			}
		}
		RegCloseKey(hKeyResults);
	}
		if (!RomListVisible()) {
		HMENU hSubMenu;
		MENUITEMINFO menuinfo;
		hSubMenu = GetSubMenu(hMenu,0);
		DeleteMenu(hSubMenu, MenuLocOfUsedFiles, MF_BYPOSITION);
		if (strlen(LastRoms[0]) == 0) {
			DeleteMenu(hSubMenu, MenuLocOfUsedFiles, MF_BYPOSITION);
		}
		memset(&menuinfo, 0, sizeof(MENUITEMINFO));
		menuinfo.cbSize = sizeof(MENUITEMINFO);
		menuinfo.fMask = MIIM_TYPE|MIIM_ID;
		menuinfo.fType = MFT_STRING;
		menuinfo.fState = MFS_ENABLED;
		menuinfo.dwTypeData = String;
		menuinfo.cch = 256;
		for (count = 0; count < RomsToRemember; count ++ ) {
			if (strlen(LastRoms[count]) == 0) { break; }
			menuinfo.wID = ID_FILE_RECENT_FILE + count;
			sprintf(String,"&%d %s",(count + 1) % 10,LastRoms[count]);
			InsertMenuItem(hSubMenu, MenuLocOfUsedFiles + count, TRUE, &menuinfo);
		}
	} else {
		HMENU hSubMenu;
		MENUITEMINFO menuinfo;
		memset(&menuinfo, 0, sizeof(MENUITEMINFO));
		menuinfo.cbSize = sizeof(MENUITEMINFO);
		menuinfo.fMask = MIIM_TYPE|MIIM_ID;
		menuinfo.fType = MFT_STRING;
		menuinfo.fState = MFS_ENABLED;
		menuinfo.dwTypeData = String;
		menuinfo.cch = 256;
		hSubMenu = GetSubMenu(hMenu,0);
		DeleteMenu(hSubMenu, MenuLocOfUsedFiles, MF_BYPOSITION);
		sprintf(String," ");
		InsertMenuItem(hSubMenu, MenuLocOfUsedFiles, TRUE, &menuinfo);
		hSubMenu = CreateMenu();
		if (strlen(LastRoms[0]) == 0) {
			menuinfo.wID = ID_FILE_RECENT_DIR;
			sprintf(String," ");
			InsertMenuItem(hSubMenu, 0, TRUE, &menuinfo);
		}
		menuinfo.fMask = MIIM_TYPE|MIIM_ID;
		for (count = 0; count < RomsToRemember; count ++ ) {
			if (strlen(LastRoms[count]) == 0) { break; }
			menuinfo.wID = ID_FILE_RECENT_FILE + count;
			sprintf(String,"&%d %s",(count + 1) % 10,LastRoms[count]);
			InsertMenuItem(hSubMenu, count, TRUE, &menuinfo);
		}
		ModifyMenu(GetSubMenu(hMenu,0),MenuLocOfUsedFiles,MF_POPUP|MF_BYPOSITION,(DWORD)hSubMenu,GS(MENU_RECENT_ROM));
		if (strlen(LastRoms[0]) == 0) {
			EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedFiles,MF_BYPOSITION|MFS_DISABLED);
		} else {
			EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedFiles,MF_BYPOSITION|MFS_ENABLED);
		}
	}
}
void LoadRecentRom (DWORD Index) {
	DWORD ThreadID;
	Index -= ID_FILE_RECENT_FILE;
	if (Index < 0 || Index > RomsToRemember) { return; }
	strcpy(CurrentFileName,LastRoms[Index]);
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)OpenChosenFile,NULL,0, &ThreadID);
}
BOOL LoadRomHeader ( void ) {
	char drive[_MAX_DRIVE] ,FileName[_MAX_DIR],dir[_MAX_DIR], ext[_MAX_EXT];
	BYTE Test[4];
	if (_strnicmp(&CurrentFileName[strlen(CurrentFileName)-4], ".ZIP",4) == 0 ){
		int port = 0, FoundRom;
	    unz_file_info info;
		char zname[132];
		unzFile file;
		file = unzOpen(CurrentFileName);
		if (file == NULL) {
			DisplayError(GS(MSG_FAIL_OPEN_ZIP));
			return FALSE;
		}
		port = unzGoToFirstFile(file);
		FoundRom = FALSE;
		while(port == UNZ_OK && FoundRom == FALSE) {
			unzGetCurrentFileInfo(file, &info, zname, 128, NULL,0, NULL,0);
		    if (unzLocateFile(file, zname, 1) != UNZ_OK ) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_ZIP));
				return FALSE;
			}
			if( unzOpenCurrentFile(file) != UNZ_OK ) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_OPEN_ZIP));
				return FALSE;
			}
			unzReadCurrentFile(file,Test,4);
			if (IsValidRomImage(Test)) {
				RomFileSize = info.uncompressed_size;
				FoundRom = TRUE;
				memcpy(RomHeader,Test,4);
				unzReadCurrentFile(file,&RomHeader[4],sizeof(RomHeader) - 4);
				if(unzCloseCurrentFile(file) == UNZ_CRCERROR) {
					unzClose(file);
					DisplayError(GS(MSG_FAIL_OPEN_ZIP));
					return FALSE;
				}
				_splitpath( CurrentFileName, drive, dir, FileName, ext );
				unzClose(file);
			}
				unzCloseCurrentFile(file);
				port = unzGoToNextFile(file);
		}
		if (FoundRom == FALSE) {
		    DisplayError(GS(MSG_FAIL_OPEN_ZIP));
		    unzClose(file);
			return FALSE;
		}
	} else {
		DWORD dwRead;
		HANDLE hFile;
		hFile = CreateFile(CurrentFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
			NULL);
		if (hFile == INVALID_HANDLE_VALUE) return FALSE;
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,Test,4,&dwRead,NULL);
		if (!IsValidRomImage(Test)) {
			CloseHandle( hFile );
			DisplayError(GS(MSG_FAIL_IMAGE));
			return FALSE;
		}
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,RomHeader,sizeof(RomHeader),&dwRead,NULL);
		CloseHandle( hFile );
	}
	ByteSwapRom(RomHeader,sizeof(RomHeader));
	memcpy(&RomName, &FileName, 60);
	RomName[260] = '\0';
	if (strlen(RomName) == 0) { strcpy(RomName,FileName); }
	return FALSE;
}
void LoadRomOptions ( void ) {
	DWORD NewRAMsize;
	ReadRomOptions();
	NewRAMsize = ROMRAMsize;
	if ((int)ROMRAMsize < 0) { NewRAMsize = SystemRDRAMsize; }
	if (RomUseLargeBuffer) {
		if (VirtualAlloc(RecompCode, LargeCompileBufferSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE)==NULL) {
			//DisplayError(GS(MSG_MEM_ALLOC_ERROR));
			//ExitThread(0);
		}
	} else {
		VirtualFree(RecompCode, LargeCompileBufferSize,MEM_DECOMMIT);
		if (VirtualAlloc(RecompCode, NormalCompileBufferSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE)==NULL) {
			//DisplayError(GS(MSG_MEM_ALLOC_ERROR));
			//ExitThread(0);
		}
	}
	if (NewRAMsize != RDRAMsize) {
		if (RDRAMsize == 0x400000) {
			if (VirtualAlloc(N64MEM + 0x400000, 0x400000, MEM_COMMIT, PAGE_READWRITE)==NULL) {
				//DisplayError(GS(MSG_MEM_ALLOC_ERROR));
				//ExitThread(0);
			}
			if (VirtualAlloc((BYTE *)JumpTable + 0x400000, 0x400000, MEM_COMMIT, PAGE_READWRITE)==NULL) {
				//DisplayError(GS(MSG_MEM_ALLOC_ERROR));
				//ExitThread(0);
			}
			if (VirtualAlloc((BYTE *)DelaySlotTable + (0x400000 >> 0xA), (0x400000 >> 0xA), MEM_COMMIT, PAGE_READWRITE)==NULL) {
				//DisplayError(GS(MSG_MEM_ALLOC_ERROR));
				//ExitThread(0);
			}
		} else {
			VirtualFree(N64MEM + 0x400000, 0x400000,MEM_DECOMMIT);
			VirtualFree((BYTE *)JumpTable + 0x400000, 0x400000,MEM_DECOMMIT);
			VirtualFree((BYTE *)DelaySlotTable + (0x400000 >> 0xA), (0x400000 >> 0xA),MEM_DECOMMIT);
		}
	}
	RDRAMsize = NewRAMsize;
	CPU_Type = SystemCPU_Type;
	if (RomCPUType != CPU_Default) { CPU_Type = RomCPUType; }
	CountPerOp = RomCF;
	if (CountPerOp < 1 || CountPerOp > 3)  { CountPerOp = SystemCF; }
	SaveUsing = RomSaveUsing;
	SelfModCheck = SystemSelfModCheck;
	if (RomSelfMod != ModCode_Default) { SelfModCheck = RomSelfMod; }
	UseTlb = RomUseTlb;
	DelaySI = RomDelaySI;
	DelayRDP = RomDelayRDP;
	DelayRSP = RomDelayRSP;
	AlignDMA = RomAlignDMA;
	SPHack = RomSPHack;
	AudioSignal = RomAudioSignal;
	DWORD crc1 = *(DWORD *)(&RomHeader[0x10]);
	DWORD crc2 = *(DWORD *)(&RomHeader[0x14]);
	BYTE crcC = RomHeader[0x3D];
	if (LimitFPS) {
		if (crc1 == 0x6AECEC4F && crc2 == 0xF0924814 && crcC == 0x50) {
			Timer_Initialize((double)59);
		} else {
			switch(RomRegion(*(ROM + 0x3D))) {
				case PAL_Region:
					Timer_Initialize((double)50); break;
				default:
					Timer_Initialize((double)60); break;
			}
		}
	} else if (SpeedCap) {
		if (crc1 == 0x6AECEC4F && crc2 == 0xF0924814 && crcC == 0x50) {
			Timer_Initialize((double)177);
		} else {
			switch(RomRegion(*(ROM + 0x3D))) {
				case PAL_Region:
					Timer_Initialize((double)150); break;
				default:
					Timer_Initialize((double)180); break;
			}
		}
	}
}
void LimitFPSLogic (void) {
	DWORD crc1 = *(DWORD*)(&RomHeader[0x10]);
	DWORD crc2 = *(DWORD*)(&RomHeader[0x14]);
	BYTE crcC = RomHeader[0x3D];
	if (CPURunning) {
		if (SpeedCap) {
			if (LimitFPS) {
				if (crc1 == 0x6AECEC4F && crc2 == 0xF0924814 && crcC == 0x50) {
					Timer_Initialize((double)59);
				}
				else {
					switch (RomRegion(*(ROM + 0x3D))) {
					case PAL_Region:
						Timer_Initialize((double)50); break;
					default:
						Timer_Initialize((double)60); break;
					}
				}
			}
			else {
				if (crc1 == 0x6AECEC4F && crc2 == 0xF0924814 && crcC == 0x50) {
					Timer_Initialize((double)177);
				}
				else {
					switch (RomRegion(*(ROM + 0x3D))) {
					case PAL_Region:
						Timer_Initialize((double)150); break;
					default:
						Timer_Initialize((double)180); break;
					}
				}
			}
		} else {
			if (crc1 == 0x6AECEC4F && crc2 == 0xF0924814 && crcC == 0x50) {
				Timer_Initialize((double)59);
			}
			else {
				switch (RomRegion(*(ROM + 0x3D))) {
				case PAL_Region:
					Timer_Initialize((double)50); break;
				default:
					Timer_Initialize((double)60); break;
				}
			}
		}
	}
}
void SpeedCapLogic (void) {
	DWORD crc1 = *(DWORD*)(&RomHeader[0x10]);
	DWORD crc2 = *(DWORD*)(&RomHeader[0x14]);
	BYTE crcC = RomHeader[0x3D];
	if (CPURunning) {
		if (!LimitFPS && SpeedCap) {
			if (crc1 == 0x6AECEC4F && crc2 == 0xF0924814 && crcC == 0x50) {
				Timer_Initialize((double)177);
			}
			else {
				switch (RomRegion(*(ROM + 0x3D))) {
				case PAL_Region:
					Timer_Initialize((double)150); break;
				default:
					Timer_Initialize((double)180); break;
				}
			}
		}
	}
}
void RemoveRecentDirList (HWND hWnd) {
	HMENU hMenu;
	DWORD count;
    hMenu = GetMenu(hWnd);
	for (count = 0; count < RomDirsToRemember; count ++ ) {
		DeleteMenu(hMenu, ID_FILE_RECENT_DIR + count, MF_BYCOMMAND);
	}
	memset(LastRoms[0],0,sizeof(LastRoms[0]));
}
void RemoveRecentList (HWND hWnd) {
	HMENU hMenu;
	DWORD count;
    hMenu = GetMenu(hWnd);
	for (count = 0; count < RomsToRemember; count ++ ) {
		DeleteMenu(hMenu, ID_FILE_RECENT_FILE + count, MF_BYCOMMAND);
	}
	memset(LastRoms[0],0,sizeof(LastRoms[0]));
}
void ReadRomOptions(void) {
	ROMRAMsize = -1;
	RomSaveUsing = Auto;
	RomCF = -1;
	RomAudioSignal = FALSE;
	RomCPUType = CPU_Default;
	RomSelfMod = ModCode_Default;
	RomUseTlb = TRUE;
	RomDelaySI = FALSE;
	RomSPHack = FALSE;
	RomUseCache = FALSE;
	RomUseLargeBuffer = FALSE;
	RomDelayRDP = FALSE;
	RomDelayRSP = FALSE;
	RomAlignDMA = FALSE;
	if (strlen(RomName) != 0) {
		char Identifier[100];
		LPSTR IniFileName;
		char String[100];
		IniFileName = GetIniFileName();
		sprintf(Identifier, "%08X-%08X-C:%X", *(DWORD*)(&RomHeader[0x10]), *(DWORD*)(&RomHeader[0x14]), RomHeader[0x3D]);
		ROMRAMsize = _GetPrivateProfileInt(Identifier, "MEM", -1, IniFileName);
		if (ROMRAMsize == 4 || ROMRAMsize == 8) {
			ROMRAMsize *= 0x100000;
		}
		else {
			ROMRAMsize = -1;
		}
		RomCF = _GetPrivateProfileInt(Identifier, "CF", -1, IniFileName);
		_GetPrivateProfileString(Identifier, "SAVE", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "4") == 0) { RomSaveUsing = eepROM_4K; }
		else if (strcmp(String, "16") == 0) { RomSaveUsing = eepROM_16K; }
		else if (strcmp(String, "SRAM") == 0) { RomSaveUsing = SRAM; }
		else if (strcmp(String, "FlashRAM") == 0) { RomSaveUsing = FlashRAM; }
		else { RomSaveUsing = Auto; }
		_GetPrivateProfileString(Identifier, "CPU", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "Interpreter") == 0) { RomCPUType = CPU_Interpreter; }
		else if (strcmp(String, "Recompiler") == 0) { RomCPUType = CPU_Recompiler; }
		else { RomCPUType = CPU_Default; }
		_GetPrivateProfileString(Identifier, "SCM", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "Cache") == 0) { RomSelfMod = ModCode_Cache; }
		else if (strcmp(String, "Change") == 0) { RomSelfMod = ModCode_ChangeMemory; }
		else if (strcmp(String, "Check") == 0) { RomSelfMod = ModCode_CheckMemoryCache; }
		else if (strcmp(String, "Return") == 0) { RomSelfMod = ModCode_CheckMemoryReturn; }
		else if (strcmp(String, "Advance") == 0) { RomSelfMod = ModCode_CheckMemoryAdvance; }
		else if (strcmp(String, "None") == 0) { RomSelfMod = ModCode_None; }
		else if (strcmp(String, "Protect") == 0) { RomSelfMod = ModCode_ProtectedMemory; }
		else { RomSelfMod = ModCode_Default; }
		_GetPrivateProfileString(Identifier, "TLB", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "OFF") == 0) { RomUseTlb = FALSE; }
		_GetPrivateProfileString(Identifier, "SI", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomDelaySI = TRUE; }
		_GetPrivateProfileString(Identifier, "SPH", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomSPHack = TRUE; }
		_GetPrivateProfileString(Identifier, "CACHING", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomUseCache = TRUE; }
		_GetPrivateProfileString(Identifier, "BUFFER", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomUseLargeBuffer = TRUE; }
		_GetPrivateProfileString(Identifier, "SIGNAL", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomAudioSignal = TRUE; }
		_GetPrivateProfileString(Identifier, "RSP", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomDelayRSP = TRUE; }
		_GetPrivateProfileString(Identifier, "RDP", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomDelayRDP = TRUE; }
		_GetPrivateProfileString(Identifier, "DMA", "", String, sizeof(String), IniFileName);
		if (strcmp(String, "ON") == 0) { RomAlignDMA = TRUE; }
	}
}
void SetNewFileDirectory (void ){
	char String[256], Directory[255], CurrentDir[255];
	HKEY hKeyResults = 0;
	long lResult;
	sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		char drive[_MAX_DRIVE] ,dir[_MAX_DIR], fname[_MAX_FNAME],ext[_MAX_EXT];
		DWORD Type, ChangeRomDir, Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"AppPath ROMs",0,&Type,(LPBYTE)(&ChangeRomDir),&Bytes);
		if (Type != REG_DWORD || lResult != ERROR_SUCCESS) { ChangeRomDir = FALSE; }
		if (!ChangeRomDir) { return; }
		_splitpath( CurrentFileName, drive, dir, fname, ext );
		sprintf(Directory,"%s%s",drive,dir);
		GetRomDirectory( CurrentDir );
		if (strcmp(CurrentDir,Directory) == 0) { return; }
		SetRomDirectory(Directory);
	}
}
void OpenChosenFile ( void ) {
#define ReadFromRomSection	0x400000
	char drive[_MAX_DRIVE] ,FileName[_MAX_DIR],dir[_MAX_DIR], ext[_MAX_EXT];
	char WinTitle[300], MapFile[_MAX_PATH];
	BYTE Test[4];
	int count;
	if (!PluginsInitialized) {
		DisplayError(GS(MSG_PLUGIN_NOT_INIT));
		return;
	}
	HandleStartup1(hMainWindow);
	for (count = 0; count < (int)RomsToRemember; count ++ ) {
		if (strlen(LastRoms[count]) == 0) { break; }
		EnableMenuItem(hMainMenu,ID_FILE_RECENT_FILE + count,MFS_DISABLED|MF_BYCOMMAND);
	}
	if (CPURunning) {
		CloseCheatWindow();
		CloseCpu();
		memset(N64MEM, 0, RDRAMsize);
	}
	SetNewFileDirectory();
	strcpy(MapFile,CurrentFileName);
	if (_strnicmp(&CurrentFileName[strlen(CurrentFileName)-4], ".ZIP",4) == 0 ){
		int len, port = 0, FoundRom;
	    	unz_file_info info;
		char zname[132];
		unzFile file;
		file = unzOpen(CurrentFileName);
		if (file == NULL) {
			DisplayError(GS(MSG_FAIL_OPEN_ZIP));
			HandleShutdown();
			return;
		}
		port = unzGoToFirstFile(file);
		FoundRom = FALSE;
		while(port == UNZ_OK && FoundRom == FALSE) {
			unzGetCurrentFileInfo(file, &info, zname, 128, NULL,0, NULL,0);
		    if (unzLocateFile(file, zname, 1) != UNZ_OK ) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_ZIP));
				HandleShutdown();
				return;
			}
			if( unzOpenCurrentFile(file) != UNZ_OK ) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_OPEN_ZIP));
				HandleShutdown();
				return;
			}
			unzReadCurrentFile(file,Test,4);
			if (IsValidRomImage(Test)) {
				FoundRom = TRUE;
				RomFileSize = info.uncompressed_size;
				if (!Allocate_ROM()) {
					unzCloseCurrentFile(file);
					unzClose(file);
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					HandleShutdown();
					return;
				}
				memcpy(ROM,Test,4);
				len = 4;
				for (count = 4; count < (int)RomFileSize; count += ReadFromRomSection) {
					len += unzReadCurrentFile(file,&ROM[count],ReadFromRomSection);
				}
				if ((int)RomFileSize != len) {
					unzCloseCurrentFile(file);
					unzClose(file);
					switch (len) {
					case UNZ_ERRNO:
					case UNZ_EOF:
					case UNZ_PARAMERROR:
					case UNZ_BADZIPFILE:
					case UNZ_INTERNALERROR:
					case UNZ_CRCERROR:
						DisplayError(GS(MSG_FAIL_OPEN_ZIP));
						break;
					}
					HandleShutdown();
					return;
				}
				if(unzCloseCurrentFile(file) == UNZ_CRCERROR) {
					unzClose(file);
					DisplayError(GS(MSG_FAIL_OPEN_ZIP));
					HandleShutdown();
					return;
				}
				AddRecentFile(hMainWindow,CurrentFileName);
				_splitpath( CurrentFileName, drive, dir, FileName, ext );
				unzClose(file);
			}
			if (FoundRom == FALSE) {
				unzCloseCurrentFile(file);
				port = unzGoToNextFile(file);
			}
		}
		if (FoundRom == FALSE) {
		    DisplayError(GS(MSG_FAIL_OPEN_ZIP));
		    unzClose(file);
			HandleShutdown();
			return;
		}
	} else {
		DWORD dwRead, dwToRead, TotalRead;
		HANDLE hFile;
		hFile = CreateFile(CurrentFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
			NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			HandleShutdown();
			return;
		}
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,Test,4,&dwRead,NULL);
		if (!IsValidRomImage(Test)) {
			CloseHandle( hFile );
			DisplayError(GS(MSG_FAIL_IMAGE));
			HandleShutdown();
			return;
		}
		RomFileSize = GetFileSize(hFile,NULL);
		if (!Allocate_ROM()) {
			CloseHandle( hFile );
			DisplayError(GS(MSG_MEM_ALLOC_ERROR));
			HandleShutdown(); 
			return;
		}
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		TotalRead = 0;
		for (count = 0; count < (int)RomFileSize; count += ReadFromRomSection) {
			dwToRead = RomFileSize - count;
			if (dwToRead > ReadFromRomSection) dwToRead = ReadFromRomSection;
			if (!ReadFile(hFile, &ROM[count], dwToRead, &dwRead, NULL)) {
				CloseHandle( hFile );
				HandleShutdown();
				return;
			}
			TotalRead += dwRead;
		}
	dwRead = TotalRead;
	if (RomFileSize != dwRead) {
	CloseHandle( hFile );
	HandleShutdown();
	return;
	}
		CloseHandle( hFile );
		AddRecentFile(hMainWindow,CurrentFileName);
		_splitpath( CurrentFileName, drive, dir, FileName, ext );
	}
	ByteSwapRom(ROM, RomFileSize);
	memcpy(RomHeader,ROM,sizeof(RomHeader));
	RecalculateCRCs();
// Is this needed?#ifdef ROM_IN_MAPSPACE
	{
		DWORD OldProtect, NewProtect;
		VirtualProtect(ROM, RomFileSize, PAGE_READONLY, &OldProtect);
		VirtualProtect(ROM, RomFileSize, OldProtect, &NewProtect);
		if (NewProtect != PAGE_READONLY) VirtualProtect(ROM, RomFileSize, PAGE_READONLY, &OldProtect);
	}
// Is this needed?#endif
	memcpy(&RomName[0],(void *)(ROM + 0x20),20);
	for( count = 0 ; count < 20; count += 4 ) {
		RomName[count] ^= RomName[count+3];
		RomName[count + 3] ^= RomName[count];
		RomName[count] ^= RomName[count+3];
		RomName[count + 1] ^= RomName[count + 2];
		RomName[count + 2] ^= RomName[count + 1];
		RomName[count + 1] ^= RomName[count + 2];
	}
	for( count = 19 ; count >= 0; count -- ) {
		if (RomName[count] == ' ') {
			RomName[count] = '\0';
		} else if (RomName[count] == '\0') {
		} else {
			count = -1;
		}
	}
	RomName[20] = '\0';
	if (strlen(RomName) == 0) { strcpy(RomName,FileName); }
	sprintf(WinTitle, "%s - %s, %s", AppName, RomName, FileName);
	for( count = 0 ; count < (int)strlen(RomName); count ++ ) {
		switch (RomName[count]) {
		case '/':
		case '\\':
			RomName[count] = '-';
			break;
		case ':':
			RomName[count] = ';';
			break;
		}
	}
	if (strcmp(GfxDLL, "Icepir8sLegacyLLE.dll") == 0 && (CPURunning || __argc > 1)) {
		ShowRomList(hMainWindow);
		HideRomBrowser();
		SetupMenu(hMainWindow);
	}
	if (!CPURunning) {
		CPURunning = TRUE;
		if (__argc == 1) HideRomBrowser();
		SetupMenu(hMainWindow);
	} else UsuallyonTopWindow(hMainWindow);
	HandleStartup1(hMainWindow);
	SetWindowText(hMainWindow,WinTitle);
	if (!RememberCheats) { DisableAllCheats(); }
	SetCurrentSaveState(hMainWindow,ID_CURRENTSAVE_DEFAULT);
	SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)"");
	{
		DWORD ThreadID, count;
		memset(&CPU_Action, 0, sizeof(CPU_Action));
		CPU_Action.hStepping = CreateEvent(NULL, FALSE, FALSE, NULL);
		WrittenToRom = FALSE;
		InitializeTLB();
		InitializeR4300iRegisters(LoadPifRom(*(ROM + 0x3D)), *(ROM + 0x3D), GetCicChipID(ROM));
		BuildInterpreter();
		RecompPos = RecompCode;
		Timers.CurrentTimerType = -1;
		Timers.Timer = 0;
		CurrentFrame = 0;
		for (count = 0; count < MaxTimers; count++) { Timers.Active[count] = FALSE; }
		ChangeTimer(ViTimer, 5000);
		ChangeCompareTimer();
		ViFieldSerration = 0;
		DMAUsed = FALSE;
		CPU_Paused = FALSE;
		ManualPaused = FALSE;
		Timer_Start();
		LoadRomOptions();
		LoadCheats();
		ResetAudio(hMainWindow);
		SetupPlugins(hMainWindow);
		switch (CPU_Type) {
		case CPU_Interpreter: hCPU = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartInterpreterCPU, NULL, 0, &ThreadID); break;
		case CPU_Recompiler: hCPU = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRecompilerCPU, NULL, 0, &ThreadID);	break;
		}
		HandleStartup2(hMainWindow);
		SendMessage(hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MSG_EMULATION_STARTED));
	}
	if (AutoFullScreen) SendMessage(hMainWindow, WM_COMMAND, ID_OPTIONS_FULLSCREEN, 0);
}
void RecalculateCRCs ( void ) {
	int bootcode, i;
	unsigned int seed, crc[2];
	unsigned int t1, t2, t3, t4, t5, t6, r, d, j;
	switch ((bootcode = 6100 + GetCicChipID(ROM))) {
	case 6101:
	case 6102:
		seed = 0xF8CA4DDC; break;
	case 6103:
		seed = 0xA3886759; break;
	case 6105:
		seed = 0xDF26F436; break;
	case 6106:
		seed = 0x1FEA617A; break;
	default:
		return;
	}
	t1 = t2 = t3 = t4 = t5 = t6 = seed;
	for (i = 0x00001000; i < 0x00101000; i += 4) {
		if ((unsigned int)(i + 3) > RomFileSize)
			d = 0;
		else
			d = ROM[i + 3] << 24 | ROM[i + 2] << 16 | ROM[i + 1] << 8 | ROM[i];
		if ((t6 + d) < t6)
			t4++;
		t6 += d;
		t3 ^= d;
		r = (d << (d & 0x1F)) | (d >> (32 - (d & 0x1F)));
		t5 += r;
		if (t2 > d)
			t2 ^= r;
		else
			t2 ^= t6 ^ d;
		if (bootcode == 6105) {
			j = 0x40 + 0x0710 + (i & 0xFF);
			t1 += (ROM[j + 3] << 24 | ROM[j + 2] << 16 | ROM[j + 1] << 8 | ROM[j]) ^ d;
		}
		else
			t1 += t5 ^ d;
	}
	if (bootcode == 6103) {
		crc[0] = (t6 ^ t4) + t3;
		crc[1] = (t5 ^ t2) + t1;
	}
	else if (bootcode == 6106) {
		crc[0] = (t6 * t4) + t3;
		crc[1] = (t5 * t2) + t1;
	}
	else {
		crc[0] = t6 ^ t4 ^ t3;
		crc[1] = t5 ^ t2 ^ t1;
	}
	if (*(DWORD *)&ROM[0x10] != crc[0]) {
		ROM[0x13] = (crc[0] & 0xFF000000) >> 24;
		ROM[0x12] = (crc[0] & 0x00FF0000) >> 16;
		ROM[0x11] = (crc[0] & 0x0000FF00) >> 8;
		ROM[0x10] = (crc[0] & 0x000000FF);
		ROM[0x17] = (crc[1] & 0xFF000000) >> 24;
		ROM[0x16] = (crc[1] & 0x00FF0000) >> 16;
		ROM[0x15] = (crc[1] & 0x0000FF00) >> 8;
		ROM[0x14] = (crc[1] & 0x000000FF);
	}
}
void SaveRecentDirs (void) {
	long lResult;
	HKEY hKeyResults = 0;
	DWORD Disposition = 0;
	char String[200];
	sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		DWORD count;
		for (count = 0; count < RomDirsToRemember; count++) {
			if (strlen(LastDirs[count]) == 0) { break; }
			sprintf(String,"RecentDir%d",count+1);
			RegSetValueEx(hKeyResults,String,0,REG_SZ,(LPBYTE)LastDirs[count],strlen(LastDirs[count]));
		}
		RegCloseKey(hKeyResults);
	}
}
void SaveRecentFiles (void) {
	long lResult;
	HKEY hKeyResults = 0;
	DWORD Disposition = 0;
	char String[200];
	sprintf(String,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		DWORD count;
		for (count = 0; count < RomsToRemember; count++) {
			if (strlen(LastRoms[count]) == 0) { break; }
			sprintf(String,"RecentFile%d",count+1);
			RegSetValueEx(hKeyResults,String,0,REG_SZ,(LPBYTE)LastRoms[count],strlen(LastRoms[count]));
		}
		RegCloseKey(hKeyResults);
	}
}
void SaveRomOptions (void) {
	char Identifier[100];
	LPSTR IniFileName;
	char String[100];
	if (strlen(RomName) == 0) { return; }
	IniFileName = GetIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
			switch (RomCPUType) {
			case CPU_Interpreter: sprintf(String, "Interpreter"); break;
			case CPU_Recompiler: sprintf(String, "Recompiler"); break;
			default: sprintf(String, "Default"); break;
			}
			_WritePrivateProfileString(Identifier, "CPU", String, GetIniFileName());
		switch (ROMRAMsize) {
		case 0x400000: strcpy(String, "4"); break;
		case 0x800000: strcpy(String, "8"); break;
		default: strcpy(String, "Default"); break;
		}
		_WritePrivateProfileString(Identifier, "MEM", String, GetIniFileName());
		switch (RomSelfMod) {
		case ModCode_Cache: sprintf(String, "Cache"); break;
		case ModCode_ChangeMemory: sprintf(String, "Change"); break;
		case ModCode_CheckMemoryCache: sprintf(String, "Check"); break;
		case ModCode_CheckMemoryReturn: sprintf(String, "Return"); break;
		case ModCode_CheckMemoryAdvance: sprintf(String, "Advance"); break;
		case ModCode_None: sprintf(String, "None"); break;
		case ModCode_ProtectedMemory: sprintf(String, "Protect"); break;
		default: sprintf(String, "Default"); break;
		}
		_WritePrivateProfileString(Identifier, "SCM", String, GetIniFileName());
		switch (RomCF) {
		case 1: case 2: case 3: sprintf(String, "%d", RomCF); break;
		default: sprintf(String, "Default"); break;
		}
		_WritePrivateProfileString(Identifier, "CF", String, GetIniFileName());
	switch (RomSaveUsing) {
	case eepROM_4K: sprintf(String,"4"); break;
	case eepROM_16K: sprintf(String,"16"); break;
	case SRAM: sprintf(String,"SRAM"); break;
	case FlashRAM: sprintf(String,"FlashRAM"); break;
	default: sprintf(String,"Default/Autodetected"); break;
	}
	_WritePrivateProfileString(Identifier, "SAVE",String,GetIniFileName());
	_WritePrivateProfileString(Identifier, "CACHING", RomUseCache ? "ON" : "Default/OFF", GetIniFileName());
	_WritePrivateProfileString(Identifier, "BUFFER", RomUseLargeBuffer ? "ON" : "Default/OFF", GetIniFileName());
	if (!ForceEnableDMA)
	_WritePrivateProfileString(Identifier, "DMA", RomAlignDMA ? "ON" : "Default/OFF", GetIniFileName());
	if (!ForceDisableTLB)
	_WritePrivateProfileString(Identifier, "TLB",RomUseTlb? "Default/ON" : "OFF", GetIniFileName());
	_WritePrivateProfileString(Identifier, "SIGNAL", RomAudioSignal ? "ON" : "Default/OFF", GetIniFileName());
	_WritePrivateProfileString(Identifier, "SPH", RomSPHack ? "ON" : "Default/OFF", GetIniFileName());
	_WritePrivateProfileString(Identifier, "RDP", RomDelayRDP ? "ON" : "Default/OFF", GetIniFileName());
	_WritePrivateProfileString(Identifier, "RSP", RomDelayRSP ? "ON" : "Default/OFF", GetIniFileName());
	_WritePrivateProfileString(Identifier, "SI", RomDelaySI ? "ON" : "Default/OFF", GetIniFileName());
}
void SetRecentRomDir (DWORD Index) {
	Index -= ID_FILE_RECENT_DIR;
	if (Index < 0 || Index > RomDirsToRemember) { return; }
	SetRomDirectory(LastDirs[Index]);
}
void SetRomDirectory ( char * Directory ) {
	long lResult;
	HKEY hKeyResults = 0;
	DWORD Disposition = 0;
	char Group[200];
	sprintf(Group,"PJ64 V 1.6.2\\%s",AppName);
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER, Group,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueEx(hKeyResults,"CustomPath ROMs",0,REG_SZ,(LPBYTE)Directory,strlen(Directory));
		AddRecentDir(hMainWindow,Directory);
		RegCloseKey(hKeyResults);
		RefreshRomBrowser();
	}
}