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
#include "Main.h"
#include "CPU.h"
#include "Memory.h"
#include "Plugin.h"
#include "Cheat.h"
#include "UnZIP.h"
#include "Resource.h"
#include "ROM Tools Common.h"
#define MenuLocOfUsedFiles	11
#define MenuLocOfUsedDirs	(MenuLocOfUsedFiles+1)
DWORD ClearFrame,RomClearFrame,RomFileSize,SaveUsing,RomSaveUsing,CPUType,RomSelfModCheck,UseTLB,RomUseTLB,FiftyNineHertz,RomFiftyNineHertz,RomJAI,AudioSignal,RomAudioSignal,RomCF,UseCache,RomUseCache,RomShankleAziAI,RomAltEmulateAI,SyncGametoAudio,RomSyncGametoAudio,CF1CF0,RomCF1CF0,DelayRDP,RomDelayRDP,DelayRSP,RomDelayRSP,AlignDMA,RomAlignDMA,DelayRDP,RomDelayRDP,DelayRSP,RomDelayRSP,DelaySI,RomDelaySI,RomRspRecompiler,CpuRecompiler,RomCpuRecompiler,RomJumperPak,ForceAuto16kbit,ForceDisableTLB,ForceDisableCaching,ForceEnableDMA,EmulateAI;
char CurrentFileName[MAX_PATH+1]={ "" },RomName[MAX_PATH+1]={ "" },RomHeader[0x1000],LastRoms[10][MAX_PATH+1],LastDirs[10][MAX_PATH+1];
BOOL IsValidRomImage (BYTE Test[4]);
void AddRecentDir(HWND hWnd,char * addition) {
	DWORD count;
	if (addition!=NULL&&RomDirsToRemember>0) {
		char Dir[MAX_PATH+1];
		BOOL bFound=FALSE;
		strcpy(Dir,addition);
		for (count=0; count<RomDirsToRemember&&!bFound; count ++) {
			if (strcmp(addition,LastDirs[count])==0) {
				if (count!=0) {
					memmove(&LastDirs[1],&LastDirs[0],sizeof(LastDirs[0]) * count);
				}
				bFound=TRUE;
			}
		}
		if (bFound==FALSE) { memmove(&LastDirs[1],&LastDirs[0],sizeof(LastDirs[0]) * (RomDirsToRemember-1)); }
		strcpy(LastDirs[0],Dir);
		SaveRecentDirs();
	}
}
void AddRecentFile(HWND hWnd,char * addition) {
	DWORD count;
	if (addition!=NULL&&RomsToRemember>0) {
		char Rom[MAX_PATH+1];
		BOOL bFound=FALSE;
		strcpy(Rom,addition);
		for (count=0; count<RomsToRemember&&!bFound; count ++) {
			if (strcmp(addition,LastRoms[count])==0) {
				if (count!=0) {
					memmove(&LastRoms[1],&LastRoms[0],sizeof(LastRoms[0]) * count);
				}
				bFound=TRUE;
			}
		}
		if (bFound==FALSE) { memmove(&LastRoms[1],&LastRoms[0],sizeof(LastRoms[0]) * (RomsToRemember-1)); }
		strcpy(LastRoms[0],Rom);
		SaveRecentFiles();
	}
}
void ByteSwapRom (BYTE * Rom,DWORD RomLen) {
	DWORD count;
	switch (*((DWORD *)&Rom[0])) {
	case 0x12408037:
		for(count=0 ; count<RomLen; count +=4) {
			Rom[count] ^=Rom[count+2];
			Rom[count+2] ^=Rom[count];
			Rom[count] ^=Rom[count+2];
			Rom[count+1] ^=Rom[count+3];
			Rom[count+3] ^=Rom[count+1];
			Rom[count+1] ^=Rom[count+3];
		}
	case 0x80371240: break;
	case 0x40072780: // N64DD IPL (J)
	case 0x40123780:
		for(count=0 ; count<RomLen; count +=4) {
			Rom[count] ^=Rom[count+3];
			Rom[count+3] ^=Rom[count];
			Rom[count] ^=Rom[count+3];
			Rom[count+1] ^=Rom[count+2];
			Rom[count+2] ^=Rom[count+1];
			Rom[count+1] ^=Rom[count+2];
		}
		break;
	default:
		DisplayError(GS(MSG_UNKNOWN_FILE_FORMAT));
	}
}
void GetRomDirectory (char * Directory) {
	char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT],Dir[255],Group[200];
	long lResult;
	HKEY hKeyResults=0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath(path_buffer,drive,dir,fname,ext);
	sprintf(Group,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,&hKeyResults);
	sprintf(Directory,"%s%sDefault ROM Directory (.z64 .v64 .n64)\\",drive,dir);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Bytes=sizeof(Dir);
		lResult=RegQueryValueEx(hKeyResults,"CustomPath ROMs",0,&Type,(LPBYTE)Dir,&Bytes);
		if (lResult==ERROR_SUCCESS) { strcpy(Directory,Dir); }
	}
	RegCloseKey(hKeyResults);
}
BOOL IsValidRomImage (BYTE Test[4]) {
	if (*((DWORD *)&Test[0])==0x40123780) return TRUE;
	if (*((DWORD *)&Test[0])==0x12408037) return TRUE;
	if (*((DWORD *)&Test[0])==0x80371240) return TRUE;
	if (*((DWORD *)&Test[0])==0x40072780) return TRUE; // N64DD IPL (J)
	return FALSE;
}
BOOL LoadDataFromRomFile(char * FileName,BYTE * Data,int DataLen,int * RomSize) {
	BYTE Test[4];
	int count;
	if (_strnicmp(&FileName[strlen(FileName)-4],".ZIP",4)==0) {
		int len,port=0,FoundRom;
		unz_file_info info;
		char zname[132];
		unzFile file;
		file=unzOpen(FileName);
		if (file==NULL) return FALSE;
		port=unzGoToFirstFile(file);
		FoundRom=FALSE;
		while(port==UNZ_OK&&FoundRom==FALSE) {
			unzGetCurrentFileInfo(file,&info,zname,128,NULL,0,NULL,0);
			if (unzLocateFile(file,zname,1)!=UNZ_OK) {
				unzClose(file);
				return FALSE;
			}
			if(unzOpenCurrentFile(file)!=UNZ_OK) {
				unzClose(file);
				return FALSE;
			}
			unzReadCurrentFile(file,Test,4);
			if (IsValidRomImage(Test)) {
				FoundRom=TRUE;
				RomFileSize=info.uncompressed_size;
				memcpy(Data,Test,4);
				len=unzReadCurrentFile(file,&Data[4],DataLen-4)+4;
				if ((int)DataLen!=len) {
					unzCloseCurrentFile(file);
					unzClose(file);
					return FALSE;
				}
				*RomSize=info.uncompressed_size;
				if(unzCloseCurrentFile(file)==UNZ_CRCERROR) {
					unzClose(file);
					return FALSE;
				}
				unzClose(file);
			}
			if (FoundRom==FALSE) {
				unzCloseCurrentFile(file);
				port=unzGoToNextFile(file);
			}
		}
		if (FoundRom==FALSE) {
			unzClose(file);
			return FALSE;
		}
	} else {
		DWORD dwRead;
		HANDLE hFile;
		hFile=CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
		if (hFile==INVALID_HANDLE_VALUE) { return FALSE; }
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,Test,4,&dwRead,NULL);
		if (!IsValidRomImage(Test)) { CloseHandle(hFile); return FALSE; }
		RomFileSize=GetFileSize(hFile,NULL);
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		if (!ReadFile(hFile,Data,DataLen,&dwRead,NULL)) { CloseHandle(hFile); return FALSE; }
		*RomSize=GetFileSize(hFile,NULL);
		CloseHandle(hFile);
	}
	switch (*((DWORD *)&Data[0])) {
	case 0x12408037:
		for(count=0 ; count<DataLen; count +=4) {
			Data[count] ^=Data[count+2];
			Data[count+2] ^=Data[count];
			Data[count] ^=Data[count+2];
			Data[count+1] ^=Data[count+3];
			Data[count+3] ^=Data[count+1];
			Data[count+1] ^=Data[count+3];
		}
	case 0x80371240: break;
	case 0x40072780: // N64DD IPL (J)
	case 0x40123780:
		for(count=0 ; count<DataLen; count +=4) {
			Data[count] ^=Data[count+3];
			Data[count+3] ^=Data[count];
			Data[count] ^=Data[count+3];
			Data[count+1] ^=Data[count+2];
			Data[count+2] ^=Data[count+1];
			Data[count+1] ^=Data[count+2];
		}
		break;
	}
	return TRUE;
}
void CreateRecentDirList (HMENU hMenu) {
	HKEY hKeyResults=0;
	char String[256];
	long lResult;
	DWORD count;
	sprintf(String,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Bytes;
		for (count=0; count<RomDirsToRemember; count++) {
			Bytes=sizeof(LastDirs[count]);
			sprintf(String,"RecentDir%d",count+1);
			lResult=RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)LastDirs[count],&Bytes);
			if (lResult!=ERROR_SUCCESS) {
				memset(LastDirs[count],0,sizeof(LastDirs[count]));
				break;
			}
		}
		RegCloseKey(hKeyResults);
	}
	{
		HMENU hSubMenu;
		MENUITEMINFO menuinfo;
		hSubMenu=GetSubMenu(hMenu,0);
		DeleteMenu(hSubMenu,MenuLocOfUsedDirs,MF_BYPOSITION);
		memset(&menuinfo,0,sizeof(MENUITEMINFO));
		menuinfo.cbSize=sizeof(MENUITEMINFO);
		menuinfo.fMask=MIIM_TYPE|MIIM_ID;
		menuinfo.fType=MFT_STRING;
		menuinfo.fState=MFS_ENABLED;
		menuinfo.dwTypeData=String;
		menuinfo.cch=256;
		sprintf(String," ");
		InsertMenuItem(hSubMenu,MenuLocOfUsedDirs,TRUE,&menuinfo);
		hSubMenu=CreateMenu();
		if (strlen(LastDirs[0])==0) {
			menuinfo.wID=ID_FILE_RECENT_DIR;
			sprintf(String," ");
			InsertMenuItem(hSubMenu,0,TRUE,&menuinfo);
		}
		menuinfo.fMask=MIIM_TYPE|MIIM_ID;
		for (count=0; count<RomDirsToRemember; count ++) {
			if (strlen(LastDirs[count])==0) { break; }
			menuinfo.wID=ID_FILE_RECENT_DIR+count;
			sprintf(String,"&%d %s",(count+1) % 10,LastDirs[count]);
			InsertMenuItem(hSubMenu,count,TRUE,&menuinfo);
		}
		ModifyMenu(GetSubMenu(hMenu,0),MenuLocOfUsedDirs,MF_POPUP|MF_BYPOSITION,(DWORD)hSubMenu,GS(MENU_RECENT_DIR));
		if (strlen(LastDirs[0])==0||RomDirsToRemember==0) {
			EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedDirs,MF_BYPOSITION|MFS_DISABLED);
		} else {
			EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedDirs,MF_BYPOSITION|MFS_ENABLED);
		}
	}
}
void CreateRecentFileList(HMENU hMenu) {
	HKEY hKeyResults=0;
	char String[256];
	long lResult;
	DWORD count;
	sprintf(String,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Bytes;
		for (count=0; count<RomsToRemember; count++) {
			Bytes=sizeof(LastRoms[count]);
			sprintf(String,"RecentFile%d",count+1);
			lResult=RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)LastRoms[count],&Bytes);
			if (lResult!=ERROR_SUCCESS) {
				memset(LastRoms[count],0,sizeof(LastRoms[count]));
				break;
			}
		}
		RegCloseKey(hKeyResults);
	}
	HMENU hSubMenu;
	MENUITEMINFO menuinfo;
	memset(&menuinfo,0,sizeof(MENUITEMINFO));
	menuinfo.cbSize=sizeof(MENUITEMINFO);
	menuinfo.fMask=MIIM_TYPE|MIIM_ID;
	menuinfo.fType=MFT_STRING;
	menuinfo.fState=MFS_ENABLED;
	menuinfo.dwTypeData=String;
	menuinfo.cch=256;
	hSubMenu=GetSubMenu(hMenu,0);
	DeleteMenu(hSubMenu,MenuLocOfUsedFiles,MF_BYPOSITION);
	sprintf(String," ");
	InsertMenuItem(hSubMenu,MenuLocOfUsedFiles,TRUE,&menuinfo);
	hSubMenu=CreateMenu();
	if (strlen(LastRoms[0])==0) {
		menuinfo.wID=ID_FILE_RECENT_DIR;
		sprintf(String," ");
		InsertMenuItem(hSubMenu,0,TRUE,&menuinfo);
	}
	menuinfo.fMask=MIIM_TYPE|MIIM_ID;
	for (count=0; count<RomsToRemember; count ++) {
		if (strlen(LastRoms[count])==0) { break; }
		menuinfo.wID=ID_FILE_RECENT_FILE+count;
		sprintf(String,"&%d %s",(count+1) % 10,LastRoms[count]);
		InsertMenuItem(hSubMenu,count,TRUE,&menuinfo);
	}
	ModifyMenu(GetSubMenu(hMenu,0),MenuLocOfUsedFiles,MF_POPUP|MF_BYPOSITION,(DWORD)hSubMenu,GS(MENU_RECENT_ROM));
	if (strlen(LastRoms[0])==0) {
		EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedFiles,MF_BYPOSITION|MFS_DISABLED);
	} else {
		EnableMenuItem(GetSubMenu(hMenu,0),MenuLocOfUsedFiles,MF_BYPOSITION|MFS_ENABLED);
	}
}
void LoadRecentRom (DWORD Index) {
	DWORD ThreadID;
	Index -=ID_FILE_RECENT_FILE;
	if (Index<0||Index>RomsToRemember) return;
	strcpy(CurrentFileName,LastRoms[Index]);
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)OpenChosenFile,NULL,0,&ThreadID);
}
BOOL LoadRomHeader (void) {
	char drive[_MAX_DRIVE],FileName[_MAX_DIR],dir[_MAX_DIR],ext[_MAX_EXT];
	BYTE Test[4];
	if (_strnicmp(&CurrentFileName[strlen(CurrentFileName)-4],".ZIP",4)==0) {
		int port=0,FoundRom;
		unz_file_info info;
		char zname[132];
		unzFile file;
		file=unzOpen(CurrentFileName);
		if (file==NULL) {
			DisplayError(GS(MSG_FAIL_OPEN_ZIP));
			return FALSE;
		}
		port=unzGoToFirstFile(file);
		FoundRom=FALSE;
		while(port==UNZ_OK&&FoundRom==FALSE) {
			unzGetCurrentFileInfo(file,&info,zname,128,NULL,0,NULL,0);
			if (unzLocateFile(file,zname,1)!=UNZ_OK) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_OPEN_ZIP));
				return FALSE;
			}
			if(unzOpenCurrentFile(file)!=UNZ_OK) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_OPEN_ZIP));
				return FALSE;
			}
			unzReadCurrentFile(file,Test,4);
			if (IsValidRomImage(Test)) {
				RomFileSize=info.uncompressed_size;
				FoundRom=TRUE;
				memcpy(RomHeader,Test,4);
				unzReadCurrentFile(file,&RomHeader[4],sizeof(RomHeader)-4);
				if(unzCloseCurrentFile(file)==UNZ_CRCERROR) {
					unzClose(file);
					DisplayError(GS(MSG_FAIL_OPEN_ZIP));
					return FALSE;
				}
				_splitpath(CurrentFileName,drive,dir,FileName,ext);
				unzClose(file);
			}
				unzCloseCurrentFile(file);
				port=unzGoToNextFile(file);
		}
		if (FoundRom==FALSE) {
			DisplayError(GS(MSG_FAIL_OPEN_ZIP));
			unzClose(file);
			return FALSE;
		}
	} else {
		DWORD dwRead;
		HANDLE hFile;
		hFile=CreateFile(CurrentFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,
			NULL);
		if (hFile==INVALID_HANDLE_VALUE) return FALSE;
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,Test,4,&dwRead,NULL);
		if (!IsValidRomImage(Test)) {
			CloseHandle(hFile);
			DisplayError(GS(MSG_FAIL_IMAGE));
			return FALSE;
		}
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,RomHeader,sizeof(RomHeader),&dwRead,NULL);
		CloseHandle(hFile);
	}
	ByteSwapRom(RomHeader,sizeof(RomHeader));
	memcpy(&RomName[0],&RomHeader[0x20],20);
	RomName[256]='\0';
	if (strlen(RomName)==0) { strcpy(RomName,FileName); }
	return FALSE;
}
void HandleTimers(void) {
	DWORD crc1=*(DWORD*)(&RomHeader[0x10]),crc2=*(DWORD*)(&RomHeader[0x14]);
	BYTE crcC=RomHeader[0x3D];
	if (!CPURunning) return;
	if (LimitFPS) {
		if (FiftyNineHertz) {
			EmuAI_SetFrameRate(59);
			Timer_Initialize((double)59);
		} else {
			switch (RomRegion(*(ROM+0x3D))) {
			case PAL_Region:
				EmuAI_SetFrameRate(50);
				Timer_Initialize((double)50); break;
			default:
				EmuAI_SetFrameRate(60);
				Timer_Initialize((double)60);
			}
		}
	} else if (SpeedCap) {
		if (FiftyNineHertz) {
			Timer_Initialize((double)118);
		} else {
			switch (RomRegion(*(ROM+0x3D))) {
			case PAL_Region:
				Timer_Initialize((double)100); break;
			default:
				Timer_Initialize((double)120);
			}
		}
	}
}
void ReadRomOptions(void) {
	RomSaveUsing=Auto;
	RomCF=-1;
	RomAudioSignal=FALSE;
	RomSelfModCheck=ModCode_Default;
	if (ForceDisableTLB) RomUseTLB=FALSE;
	else RomUseTLB=TRUE;
	RomClearFrame=FALSE;
	RomFiftyNineHertz=FALSE;
	RomDelaySI=FALSE;
	if (strcmp(RSPDLL,"RSP.dll")==0&&(strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")!=0||strcmp(RomName,"THE LEGEND OF ZELDA")==0||strcmp(RomName,"THE MASK OF MUJURA")==0||strcmp(RomName,"ZELDA MAJORA'S MASK")==0||strcmp(RomName,"BANJO KAZOOIE 2")==0||strcmp(RomName,"BANJO TOOIE")==0||strcmp(RomName,"CONKER BFD")==0||strcmp(RomName,"DONKEY KONG 64")==0||strcmp(RomName,"JET FORCE GEMINI")==0||strcmp(RomName,"STAR TWINS")==0||strcmp(RomName,"Perfect Dark")==0)) RomRspRecompiler=TRUE;
	else RomRspRecompiler=FALSE;
	RomCpuRecompiler=TRUE;
	RomJumperPak=FALSE;
	RomCF1CF0=TRUE;
	RomDelayRDP=FALSE;
	RomDelayRSP=FALSE;
	RomShankleAziAI=FALSE;
	RomAltEmulateAI=FALSE;
	if (strcmp(AudioDLL,"Jabo_Dsound.dll")==0) RomSyncGametoAudio=TRUE;
	else RomSyncGametoAudio=FALSE;
	if (ForceEnableDMA) RomAlignDMA=TRUE;
	else RomAlignDMA=FALSE;
	if (ForceDisableCaching) RomUseCache=REG_CACHE_OFF;
	else RomUseCache=UseCache_Default;
	RomJAI=FALSE;
	if (strlen(RomName)!=0) {
		LPSTR IniFileName;
		char String[100],Identifier[100];
		IniFileName=GetIniFileName();
		sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD*)(&RomHeader[0x10]),*(DWORD*)(&RomHeader[0x14]),RomHeader[0x3D]);
		RomCF=_GetPrivateProfileInt(Identifier,"Counter Factor",-1,IniFileName);
		if (RomCF>3||RomCF<1) RomCF=-1;
		_GetPrivateProfileString(Identifier,"Save Type","",String,sizeof(String),IniFileName);
		if (strcmp(String,"4kbit EEPROM")==0) { RomSaveUsing=EEPROM_4K; }
		else if (strcmp(String,"16kbit EEPROM")==0) { RomSaveUsing=EEPROM_16K; }
		else if (strcmp(String,"SRAM")==0) { RomSaveUsing=SRAM; }
		else if (strcmp(String,"FlashRAM")==0) { RomSaveUsing=FlashRAM; }
		else { RomSaveUsing=Auto; }
		_GetPrivateProfileString(Identifier,"Clear Frame","",String,sizeof(String),IniFileName);
		if ((strcmp(GfxDLL,"Jabo_Direct3D8.dll")==0||strcmp(GfxDLL,"Jabo_Direct3DL.dll")==0)&&(strcmp(String,"1")==0||strcmp(String,"2")==0)) RomClearFrame=TRUE;
		_GetPrivateProfileString(Identifier,"TLB","",String,sizeof(String),IniFileName);
		if (strcmp(String,"OFF")==0) RomUseTLB=FALSE;
		_GetPrivateProfileString(Identifier,"Azi AI","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0&&strcmp(AudioDLL,"Shankle_Audio.dll")==0) RomShankleAziAI=TRUE;
		_GetPrivateProfileString(Identifier,"Sync Game to Audio","",String,sizeof(String),IniFileName);
		if (strcmp(String,"OFF")==0) RomSyncGametoAudio=FALSE;
		_GetPrivateProfileString(Identifier,"Delay SI","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomDelaySI=TRUE;
		_GetPrivateProfileString(Identifier,"CF1-->0","",String,sizeof(String),IniFileName);
		if (strcmp(String,"OFF")==0) RomCF1CF0=FALSE;
		_GetPrivateProfileString(Identifier,"Signal","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomAudioSignal=TRUE;
		_GetPrivateProfileString(Identifier,"Delay RSP","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomDelayRSP=TRUE;
		_GetPrivateProfileString(Identifier,"Delay RDP","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomDelayRDP=TRUE;
		_GetPrivateProfileString(Identifier,"Align DMA","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomAlignDMA=TRUE;
		_GetPrivateProfileString(Identifier,"59 Hz","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomFiftyNineHertz=TRUE;
		_GetPrivateProfileString(Identifier,"RSP Recompiler","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomRspRecompiler=TRUE;
		_GetPrivateProfileString(Identifier,"CPU Recompiler","",String,sizeof(String),IniFileName);
		if (strcmp(String,"OFF")==0) RomCpuRecompiler=FALSE;
		_GetPrivateProfileString(Identifier,"Jumper Pak","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0) RomJumperPak=TRUE;
		_GetPrivateProfileString(Identifier,"Jabo AI","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0&&strcmp(AudioDLL,"Jabo_Dsound.dll")==0) { RomSyncGametoAudio=TRUE; RomJAI=TRUE; }
		_GetPrivateProfileString(Identifier,"Legacy AiLenChanged","",String,sizeof(String),IniFileName);
		if (strcmp(String,"ON")==0&&(RomJAI||RomShankleAziAI)) RomAltEmulateAI=TRUE;
		if (RomCpuRecompiler) {
			_GetPrivateProfileString(Identifier,"Self-modifying Code Method","",String,sizeof(String),IniFileName);
			if (strcmp(String,"Cache")==0) { RomSelfModCheck=ModCode_Cache; }
			else if (strcmp(String,"Check Memory")==0) { RomSelfModCheck=ModCode_CheckMemory; }
			else if (strcmp(String,"Protect Memory")==0) { RomSelfModCheck=ModCode_ProtectMemory; }
			else RomSelfModCheck=ModCode_Default;
			if (!ForceDisableCaching) {
				_GetPrivateProfileString(Identifier,"Register Caching","",String,sizeof(String),IniFileName);
				if (strcmp(String,"ON")==0) { RomUseCache=REG_CACHE_ON; }
				else if (strcmp(String,"OFF")==0) { RomUseCache=REG_CACHE_OFF; }
				else RomUseCache=UseCache_Default;
			}
		} else RomUseTLB = TRUE;
		if (!RomCpuRecompiler||RomCF!=-1&&RomCF!=1) RomCF1CF0=FALSE;
	}
}
void SetNewFileDirectory (void) {
	char String[256],Directory[255],CurrentDir[255];
	HKEY hKeyResults=0;
	long lResult;
	sprintf(String,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
		DWORD Type,ChangeRomDir,Bytes=4;
		lResult=RegQueryValueEx(hKeyResults,"AppPath ROMs",0,&Type,(LPBYTE)(&ChangeRomDir),&Bytes);
		if (Type!=REG_DWORD||lResult!=ERROR_SUCCESS) { ChangeRomDir=FALSE; }
		if (!ChangeRomDir) return;
		_splitpath(CurrentFileName,drive,dir,fname,ext);
		sprintf(Directory,"%s%s",drive,dir);
		GetRomDirectory(CurrentDir);
		if (strcmp(CurrentDir,Directory)==0) return;
		SetRomDirectory(Directory);
	}
}
void OpenChosenFile(void) {
#define ReadFromRomSection	0x400000
	char drive[_MAX_DRIVE],FileName[_MAX_DIR],dir[_MAX_DIR],ext[_MAX_EXT];
	char MapFile[_MAX_PATH];
	int count;
	BYTE Test[4];
	for (count=0; count<(int)RomsToRemember; count++) {
		if (strlen(LastRoms[count])==0) { break; }
		EnableMenuItem(hMainMenu,ID_FILE_RECENT_FILE+count,MFS_DISABLED|MF_BYCOMMAND);
	}
	CloseCheatWindow();
	SetNewFileDirectory();
	strcpy(MapFile,CurrentFileName);
	if (_strnicmp(&CurrentFileName[strlen(CurrentFileName)-4],".ZIP",4)==0) {
		int len,port=0,FoundRom;
		unz_file_info info;
		char zname[132];
		unzFile file;
		file=unzOpen(CurrentFileName);
		if (file==NULL) {
			unzClose(file);
			DisplayError(GS(OPENROM_FAILED));
			HandleShutdown(hMainWindow);
			return;
		}
		port=unzGoToFirstFile(file);
		FoundRom=FALSE;
		while (port==UNZ_OK&&FoundRom==FALSE) {
			unzGetCurrentFileInfo(file,&info,zname,128,NULL,0,NULL,0);
			if (unzLocateFile(file,zname,1)!=UNZ_OK) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_OPEN_ZIP));
				HandleShutdown(hMainWindow);
				return;
			}
			if (unzOpenCurrentFile(file)!=UNZ_OK) {
				unzClose(file);
				DisplayError(GS(MSG_FAIL_OPEN_ZIP));
				HandleShutdown(hMainWindow);
				return;
			}
			unzReadCurrentFile(file,Test,4);
			if (IsValidRomImage(Test)) {
				FoundRom=TRUE;
				RomFileSize=info.uncompressed_size;
				if (!Allocate_ROM()) {
					unzCloseCurrentFile(file);
					unzClose(file);
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					HandleShutdown(hMainWindow);
					return;
				}
				memcpy(ROM,Test,4);
				len=4;
				for (count=4; count<(int)RomFileSize; count +=ReadFromRomSection) {
					len +=unzReadCurrentFile(file,&ROM[count],ReadFromRomSection);
				}
				if ((int)RomFileSize!=len) {
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
					HandleShutdown(hMainWindow);
					return;
				}
				if (unzCloseCurrentFile(file)==UNZ_CRCERROR) {
					unzClose(file);
					DisplayError(GS(MSG_FAIL_OPEN_ZIP));
					HandleShutdown(hMainWindow);
					return;
				}
				AddRecentFile(hMainWindow,CurrentFileName);
				_splitpath(CurrentFileName,drive,dir,FileName,ext);
				unzClose(file);
			}
			if (FoundRom==FALSE) {
				unzCloseCurrentFile(file);
				port=unzGoToNextFile(file);
			}
		}
		if (FoundRom==FALSE) {
			unzClose(file);
			DisplayError(GS(MSG_FAIL_IMAGE));
			HandleShutdown(hMainWindow);
			return;
		}
	} else {
		DWORD dwRead,dwToRead,TotalRead;
		HANDLE hFile;
		hFile=CreateFile(CurrentFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,
			NULL);
		if (hFile==INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
			DisplayError(GS(OPENROM_FAILED));
			HandleShutdown(hMainWindow);
			return;
		}
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		ReadFile(hFile,Test,4,&dwRead,NULL);
		if (!IsValidRomImage(Test)) {
			CloseHandle(hFile);
			DisplayError(GS(MSG_FAIL_IMAGE));
			HandleShutdown(hMainWindow);
			return;
		}
		RomFileSize=GetFileSize(hFile,NULL);
		if (!Allocate_ROM()) {
			CloseHandle(hFile);
			DisplayError(GS(MSG_MEM_ALLOC_ERROR));
			HandleShutdown(hMainWindow);
			return;
		}
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		TotalRead=0;
		for (count=0; count<(int)RomFileSize; count +=ReadFromRomSection) {
			dwToRead=RomFileSize-count;
			if (dwToRead>ReadFromRomSection) dwToRead=ReadFromRomSection;
			if (!ReadFile(hFile,&ROM[count],dwToRead,&dwRead,NULL)) {
				CloseHandle(hFile);
				HandleShutdown(hMainWindow);
				return;
			}
			TotalRead +=dwRead;
		}
		dwRead=TotalRead;
		if (RomFileSize!=dwRead) {
			CloseHandle(hFile);
			HandleShutdown(hMainWindow);
			return;
		}
		CloseHandle(hFile);
		AddRecentFile(hMainWindow,CurrentFileName);
		_splitpath(CurrentFileName,drive,dir,FileName,ext);
	}
	ByteSwapRom(ROM,RomFileSize);
	memcpy(RomHeader,ROM,sizeof(RomHeader));
	RecalculateCRC();
	memcpy(&RomName[0],(void *)(ROM+0x20),20);
	for (count=0; count<20; count +=4) {
		RomName[count] ^=RomName[count+3];
		RomName[count+3] ^=RomName[count];
		RomName[count] ^=RomName[count+3];
		RomName[count+1] ^=RomName[count+2];
		RomName[count+2] ^=RomName[count+1];
		RomName[count+1] ^=RomName[count+2];
	}
	for (count=19; count>=0; count--) {
		if (RomName[count]==' ') {
			RomName[count]='\0';
		}
		else if (RomName[count]=='\0') {
		}
		else {
			count=-1;
		}
	}
	RomName[20]='\0';
	for (count=0; count<(int)strlen(RomName); count++) {
		switch (RomName[count]) {
		case '/':
		case '\\':
			RomName[count]='-';
			break;
		case ':':
			RomName[count]=';';
			break;
		}
	}
	if (CPURunning) ResetFunction();
	else {
		CPURunning=TRUE;
		if (__argc>1) GetCurrentDlls();
		HideRomBrowser();
	}
	SetCurrentSaveState(hMainWindow,ID_CURRENTSAVE_DEFAULT);
	SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)"");
	if (AutoFullScreen&&strcmp(GfxDLL,"RiceVideo.dll")!=0) {
		timeBeginPeriod(16);
		Sleep(200);
		timeEndPeriod(16);
		SendMessage(hMainWindow,WM_COMMAND,ID_OPTIONS_FULLSCREEN,0);
	} else SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MSG_EMULATION_STARTED));
}
void RecalculateCRC (void) {
	int bootcode,i;
	unsigned int seed,crc[2];
	unsigned int t1,t2,t3,t4,t5,t6,r,d,j;
	DWORD crc1=*(DWORD*)(&RomHeader[0x10]),crc2=*(DWORD*)(&RomHeader[0x14]);
	BYTE crcC=RomHeader[0x3D];
	if (crcC==0x45) {
		// Decades Later by BroDute
		if (crc1==0xE1CE3595&&crc2==0x68941049) {
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,"Author of this game-'That one german guy that made the Star Revenge Series and remade vanilla SM64 to be finally a good game.'\n\nKnowing that this game was made in bad faith to undermine the original work,do you still want to play this game?","Illegit Game Disclaimer",MB_YESNO|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDNO) SendMessage(hMainWindow,WM_COMMAND,ID_FILE_EXIT,0);
			HandleModal2(hMainWindow);
		}
		// Star Revenge by BroDute
		if (crc1==0x5394053C&&crc2==0xA5D8610A||crc1==0xCEE7DD5F&&crc2==0x4046AC23||crc1==0xC380A1E6&&crc2==0x75432881) {
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,"'Trashlux' 'the memetrash Skelux'\n\nKnowing that the author of this game does not appreciate the work SKELUX put into the tools used by said author to develop his work,do you still want to play this game?","Illegit Game Disclaimer",MB_YESNO|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDNO) SendMessage(hMainWindow,WM_COMMAND,ID_FILE_EXIT,0);
			HandleModal2(hMainWindow);
		}
		/*
		* Star Road by SKELUX,Hijack "Retooled" Edit by Pyro Jay
		*
		* Comment from the developer found below.
		*
		*
		* "When you put a lot of work into something,other people love to come along and put
		* in a fraction of the work modifying it just so they can slap their name over yours."
		*/
		if (crc1==0xCAC63712&&crc2==0xE2372AF3) {
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,"On christmas of 2021,Super Mario Star Road Retooled would release on YouTube (https://www.youtube.com/watch?v=HdMHkK1MPO0). In the video,it appears to be a rerelease of a ROMhack called Star Road originally created by a user who goes by the alias 'Pyro Jay'. The video opens with 'Pyro Jay Presents' 'Rebuilt from the ground up 10 years later...' 'Star Road Retooled' And the video showcases lots of levels in this ROMhack rerelease before cutting to 'Revisit a timeless classic...' '... with improved visuals,bug fixes,gameplay features,and more!' 'New areas to visit and redesigned stars to collect!' 'Star Road Retooled' and the video fades to black,then says 'Hack by Pyro Jay' 'Trailer by SpK' 'Available Now!' 'Link in the description'.\n\nBut this hack wasn't solely made by Pyro Jay and it doesn't belong to them. This is actually an infringing hackjob of the original made to undermine the true author's official release (https://www.youtube.com/watch?v=_JBdxcnyxeQ).\n\n\nKnowing that is an infringing hackjob of somebody else's game that was was made in bad faith to undermine the original work,do you still want to play this game?","Illegit Game Disclaimer",MB_YESNO|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDNO) SendMessage(hMainWindow,WM_COMMAND,ID_FILE_EXIT,0);
			HandleModal2(hMainWindow);
		}
		/*
		* B3313 by Chrisrlillo,Hijack "Unabandoned" Edit by Thegreatestroman&Chlorobyte/Benedani
		*
		* Comment from the developer found below.
		*
		*
		* "B3313 unabandoned is an edited rom of an unfinished version of my hack B3313 from 2023,
		* made without my consent and stealing as much content from my friends and I as possible.
		* If you value ROMhacking ethics,ignore this ROM."
		*/
		if (crc1==0xC39F397B&&crc2==0x9C2D6AFF) {
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,"Author of B3313-'At one point I worked with benedani,also known as chlorobyte and thegreatest roman. These 2 users ended up organizing a raid to my discord server to take control,hijacked the ROMhacking.com pages,tried to hijack my music distributors as I'm a musician,while spreading misinformation about and harassming me. I suggest staying away from these people.'\n\n\nKnowing that this infringing game was made by these same sick and heartless jerks to undermine the original work,do you still want to play this game?","Illegit Game Disclaimer",MB_YESNO|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDNO) SendMessage(hMainWindow,WM_COMMAND,ID_FILE_EXIT,0);
			HandleModal2(hMainWindow);
		}
	}
	switch ((bootcode=6100+GetCicChipID(ROM))) {
	case 6101:
	case 6102:
		seed=0xF8CA4DDC;
		break;
	case 6103:
		seed=0xA3886759;
		break;
	case 6105:
		seed=0xDF26F436;
		break;
	case 6106:
		seed=0x1FEA617A;
		break;
	default:
		return;
	}
	t1=t2=t3=t4=t5=t6=seed;
	for (i=0x00001000; i<0x00101000; i +=4) {
		if ((unsigned int)(i+3)>RomFileSize) d=0;
		else d=ROM[i+3]<<24|ROM[i+2]<<16|ROM[i+1]<<8|ROM[i];
		if ((t6+d)<t6) t4++;
		t6 +=d;
		t3 ^=d;
		r=(d<<(d&0x1F))|(d>>(32-(d&0x1F)));
		t5 +=r;
		if (t2>d) t2 ^=r;
		else t2 ^=t6 ^ d;
		if (bootcode==6105) {
			j=0x40+0x0710+(i&0xFF);
			t1 +=(ROM[j+3]<<24|ROM[j+2]<<16|ROM[j+1]<<8|ROM[j]) ^ d;
		} else t1 +=t5 ^ d;
	}
	if (bootcode==6103) {
		crc[0]=(t6 ^ t4)+t3;
		crc[1]=(t5 ^ t2)+t1;
	} else if (bootcode==6106) {
		crc[0]=(t6 * t4)+t3;
		crc[1]=(t5 * t2)+t1;
	} else {
		crc[0]=t6 ^ t4 ^ t3;
		crc[1]=t5 ^ t2 ^ t1;
	}
	if (*(DWORD*)&ROM[0x10]!=crc[0]) {
		ROM[0x13]=(crc[0]&0xFF000000)>>24;
		ROM[0x12]=(crc[0]&0x00FF0000)>>16;
		ROM[0x11]=(crc[0]&0x0000FF00)>>8;
		ROM[0x10]=(crc[0]&0x000000FF);
		ROM[0x17]=(crc[1]&0xFF000000)>>24;
		ROM[0x16]=(crc[1]&0x00FF0000)>>16;
		ROM[0x15]=(crc[1]&0x0000FF00)>>8;
		ROM[0x14]=(crc[1]&0x000000FF);
	}
}
void SaveRecentDirs (void) {
	long lResult;
	HKEY hKeyResults=0;
	DWORD Disposition=0;
	char String[256];
	sprintf(String,"PJ64 V 1.6.2\\Configuration");
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult==ERROR_SUCCESS) {
		DWORD count;
		for (count=0; count<RomDirsToRemember; count++) {
			if (strlen(LastDirs[count])==0) { break; }
			sprintf(String,"RecentDir%d",count+1);
			RegSetValueEx(hKeyResults,String,0,REG_SZ,(LPBYTE)LastDirs[count],strlen(LastDirs[count]));
		}
		RegCloseKey(hKeyResults);
	}
}
void SaveRecentFiles (void) {
	long lResult;
	HKEY hKeyResults=0;
	DWORD Disposition=0;
	char String[256];
	sprintf(String,"PJ64 V 1.6.2\\Configuration");
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult==ERROR_SUCCESS) {
		DWORD count;
		for (count=0; count<RomsToRemember; count++) {
			if (strlen(LastRoms[count])==0) { break; }
			sprintf(String,"RecentFile%d",count+1);
			RegSetValueEx(hKeyResults,String,0,REG_SZ,(LPBYTE)LastRoms[count],strlen(LastRoms[count]));
		}
		RegCloseKey(hKeyResults);
	}
}
void HandleWindowTitle (void) {
	char* IniFile,String[256],String2[256],GameName[256],Identifier[256],WinTitle[256];
	IniFile=GetIniFileName();
	_GetPrivateProfileString("Meta","Version","",String,sizeof(String),IniFile);
	_GetPrivateProfileString("Meta","Date","",String2,sizeof(String2),IniFile);
	if (strlen(String)==0) strcpy(String,"UNKNOWN VERSION");
	if (strlen(String2)==0) strcpy(String2,"UNKNOWN DATE");
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD*)(&RomHeader[0x10]),*(DWORD*)(&RomHeader[0x14]),RomHeader[0x3D]);
	_GetPrivateProfileString(Identifier,"Game Name","",GameName,sizeof(GameName),IniFile);
	if (strlen(RomName)==0) strcpy(RomName,"INTERNAL NAME UNAVAILABLE");
	if (strlen(GameName)==0) strcpy(GameName,"UNREGISTERED GAME ENTRY");
	sprintf(WinTitle,"PJ64BIV%s %s - %s, %s",String,String2,RomName,GameName);
	SetWindowText(hMainWindow,WinTitle);
}
void SaveRomOptions (void) {
	char Identifier[100];
	LPSTR IniFileName;
	char String[100];
	IniFileName=GetIniFileName();
	sprintf(Identifier,"Meta");
	sprintf(String,"%s",__DATE__);
	_WritePrivateProfileString(Identifier,"Date",String,GetIniFileName());
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	if (!RomCpuRecompiler) _WritePrivateProfileString(Identifier,"Self-modifying Code Method","Default",GetIniFileName());
	else {
		switch (RomSelfModCheck) {
		case ModCode_Cache: sprintf(String,"Cache"); break;
		case ModCode_CheckMemory: sprintf(String,"Check Memory"); break;
		case ModCode_ProtectMemory: sprintf(String,"Protect Memory"); break;
		default: sprintf(String,"Default");
		}
		_WritePrivateProfileString(Identifier,"Self-modifying Code Method",String,GetIniFileName());
	}
	if (!RomCpuRecompiler) _WritePrivateProfileString(Identifier,"Register Caching","Default",GetIniFileName());
	else if (!ForceDisableCaching) {
		switch (RomUseCache) {
		case REG_CACHE_ON: sprintf(String,"ON"); break;
		case REG_CACHE_OFF: sprintf(String,"OFF"); break;
		default: sprintf(String,"Default");
		}
		_WritePrivateProfileString(Identifier,"Register Caching",String,GetIniFileName());
	}
	switch (RomCF) {
	case 1: case 2: case 3: sprintf(String,"%d",RomCF); break;
	default: sprintf(String,"Default");
	}
	_WritePrivateProfileString(Identifier,"Counter Factor",String,GetIniFileName());
	switch (RomSaveUsing) {
	case EEPROM_4K: sprintf(String,"4kbit EEPROM"); break;
	case EEPROM_16K: sprintf(String,"16kbit EEPROM"); break;
	case SRAM: sprintf(String,"SRAM"); break;
	case FlashRAM: sprintf(String,"FlashRAM"); break;
	default: sprintf(String,"Default");
	}
	_WritePrivateProfileString(Identifier,"Save Type",String,GetIniFileName());
	if (strcmp(AudioDLL,"Jabo_Dsound.dll")==0) {
		if (RomJAI) _WritePrivateProfileString(Identifier,"Sync Game to Audio","Default",GetIniFileName());
		else _WritePrivateProfileString(Identifier,"Sync Game to Audio",RomSyncGametoAudio?"Default":"OFF",GetIniFileName());
		_WritePrivateProfileString(Identifier,"Jabo AI",RomJAI?"ON":"Default",GetIniFileName());
	}
	if (strcmp(AudioDLL,"Shankle_Audio.dll")==0) _WritePrivateProfileString(Identifier,"Azi AI",RomShankleAziAI?"ON":"Default",GetIniFileName());
	if (RomJAI||RomShankleAziAI) _WritePrivateProfileString(Identifier,"Legacy AiLenChanged",RomAltEmulateAI?"ON":"Default",GetIniFileName());
	else _WritePrivateProfileString(Identifier,"Legacy AiLenChanged","Default",GetIniFileName());
	if (strcmp(RSPDLL, "RSP.dll")==0&&(strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0||strcmp(RomName,"THE LEGEND OF ZELDA")==0||strcmp(RomName,"THE MASK OF MUJURA")==0||strcmp(RomName,"ZELDA MAJORA'S MASK")==0||strcmp(RomName,"BANJO KAZOOIE 2")==0||strcmp(RomName,"BANJO TOOIE")==0||strcmp(RomName,"CONKER BFD")==0||strcmp(RomName,"DONKEY KONG 64")==0||strcmp(RomName,"JET FORCE GEMINI")==0||strcmp(RomName,"STAR TWINS")==0||strcmp(RomName,"Perfect Dark")==0)) _WritePrivateProfileString(Identifier,"RSP Recompiler",RomRspRecompiler?"ON":"Default",GetIniFileName());
	_WritePrivateProfileString(Identifier,"CPU Recompiler",RomCpuRecompiler?"Default":"OFF",GetIniFileName());
	if (!RomCpuRecompiler) _WritePrivateProfileString(Identifier,"TLB","Default",GetIniFileName());
	else if (!ForceDisableTLB) _WritePrivateProfileString(Identifier,"TLB",RomUseTLB?"Default":"OFF",GetIniFileName());
	_WritePrivateProfileString(Identifier,"Signal",RomAudioSignal?"ON":"Default",GetIniFileName());
	_WritePrivateProfileString(Identifier,"59 Hz",RomFiftyNineHertz?"ON":"Default",GetIniFileName());
	if (!RomCpuRecompiler||RomCF!=-1&&RomCF!=1) _WritePrivateProfileString(Identifier,"CF1-->0","Default",GetIniFileName());
	else _WritePrivateProfileString(Identifier,"CF1-->0",RomCF1CF0?"Default":"OFF",GetIniFileName());
	if (!ForceEnableDMA) _WritePrivateProfileString(Identifier,"Align DMA",RomAlignDMA?"ON":"Default",GetIniFileName());
	_WritePrivateProfileString(Identifier,"Jumper Pak",RomJumperPak?"ON":"Default",GetIniFileName());
	_WritePrivateProfileString(Identifier,"Delay RDP",RomDelayRDP?"ON":"Default",GetIniFileName());
	_WritePrivateProfileString(Identifier,"Delay RSP",RomDelayRSP?"ON":"Default",GetIniFileName());
	_WritePrivateProfileString(Identifier,"Delay SI",RomDelaySI?"ON":"Default",GetIniFileName());
	if (!CPURunning) HandleWindowTitle();
}
void SetRecentRomDir (DWORD Index) {
	Index -=ID_FILE_RECENT_DIR;
	if (Index<0||Index>RomDirsToRemember) return;
	SetRomDirectory(LastDirs[Index]);
}
void SetRomDirectory (char * Directory) {
	long lResult;
	HKEY hKeyResults=0;
	DWORD Disposition=0;
	char Group[200];
	sprintf(Group,"PJ64 V 1.6.2\\Configuration");
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER,Group,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult==ERROR_SUCCESS) {
		RegSetValueEx(hKeyResults,"CustomPath ROMs",0,REG_SZ,(LPBYTE)Directory,strlen(Directory));
		AddRecentDir(hMainWindow,Directory);
		RegCloseKey(hKeyResults);
		if (CPURunning) __argc=2;
		else RefreshRomBrowser();
		SetupMenu(hMainWindow);
	}
}