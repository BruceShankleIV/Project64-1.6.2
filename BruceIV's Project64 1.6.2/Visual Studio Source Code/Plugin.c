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
#include <stdio.h>
#include "Main.h"
#include "Cheat.h"
#include "CPU.h"
#include "ROM.h"
#include "Plugin.h"
#include "Resource.h"
char RSPDLL[100],GfxDLL[100],AudioDLL[100],ControllerDLL[100],*PluginNames[MaxDlls];
HINSTANCE hAudioDll,hControllerDll,hGfxDll,hRSPDLL;
DWORD PluginCount,RspTaskValue,AudioIntrReg;
WORD RSPVersion,ContVersion;
HANDLE hAudioThread=NULL;
CONTROL Controllers[4];
BOOL PluginsInitialized=FALSE,GLideN64NeedsToBeSetupFirst=FALSE,GLideN64HasBeenSetupFirst=FALSE,PluginsChanged(HWND hDlg),ValidPluginVersion(PLUGIN_INFO*PluginInfo);
void AudioThread (void) {
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	for (;;) { AiUpdate(TRUE); }
}
void GetCurrentDlls (void) {
	long lResult;
	HKEY hKeyResults=0;
	char String[256];
	sprintf(String,"PJ64 V 1.6.2\\Configuration\\Dll");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Bytes=100;
		lResult=RegQueryValueEx(hKeyResults,"RSP Dll",0,&Type,(LPBYTE)(RSPDLL),&Bytes);
		if (lResult!=ERROR_SUCCESS) sprintf(RSPDLL,"%s",DefaultRSPDLL);
		if (!inFullScreen) {
			Bytes=100;
			lResult=RegQueryValueEx(hKeyResults,"Graphics Dll",0,&Type,(LPBYTE)(GfxDLL),&Bytes);
			if (lResult!=ERROR_SUCCESS) sprintf(GfxDLL,"%s",DefaultGFXDll);
		}
		Bytes=100;
		lResult=RegQueryValueEx(hKeyResults,"Audio Dll",0,&Type,(LPBYTE)(AudioDLL),&Bytes);
		if (lResult!=ERROR_SUCCESS) sprintf(AudioDLL,"%s",DefaultAudioDll);
		Bytes=100;
		lResult=RegQueryValueEx(hKeyResults,"Controller Dll",0,&Type,(LPBYTE)(ControllerDLL),&Bytes);
		if (lResult!=ERROR_SUCCESS) sprintf(ControllerDLL,"%s",DefaultControllerDll);
	} else {
		sprintf(RSPDLL,"%s",DefaultRSPDLL);
		sprintf(GfxDLL,"%s",DefaultGFXDll);
		sprintf(AudioDLL,"%s",DefaultAudioDll);
		sprintf(ControllerDLL,"%s",DefaultControllerDll);
	}
	if ((strcmp(GfxDLL,"Glide64.dll")==0||strcmp(GfxDLL,"Jabo_Direct3D8.dll")==0||strcmp(GfxDLL,"Jabo_Direct3DL.dll")==0||strcmp(GfxDLL,"RiceVideo.dll")==0)&&strcmp(RSPDLL,"Icepir8sLegacyRSP.dll")==0) strcpy(RSPDLL,"RSP.dll");
}
void GetPluginDir(char*Directory) {
	char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath(path_buffer,drive,dir,fname,ext);
	strcpy(Directory,drive);
	strcat(Directory,dir);
	strcat(Directory,"Plugin\\");
}
void GetSnapShotDir(char*Directory) {
	char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT],Dir[255],Group[200];
	long lResult;
	HKEY hKeyResults=0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath(path_buffer,drive,dir,fname,ext);
	sprintf(Directory,"%s%sScreenshots\\",drive,dir);
	sprintf(Group,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Value,Bytes;
		Bytes=4;
		lResult=RegQueryValueEx(hKeyResults,"AppPath Screenshots",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult==ERROR_SUCCESS&&Value==FALSE) {
			Bytes=sizeof(Dir);
			lResult=RegQueryValueEx(hKeyResults,"CustomPath Screenshots",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult==ERROR_SUCCESS) { strcpy(Directory,Dir); }
		}
	}
	RegCloseKey(hKeyResults);
}
BOOL LoadAudioDll(char*AudioDll) {
	PLUGIN_INFO PluginInfo;
	char DllName[300];
	GetPluginDir(DllName);
	strcat(DllName,AudioDll);
	hAudioDll=LoadLibrary(DllName);
	if (hAudioDll==NULL) return FALSE;
	GetDllInfo=(void (__cdecl*)(PLUGIN_INFO*))GetProcAddress(hAudioDll,"GetDllInfo");
	if (GetDllInfo==NULL) return FALSE;
	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo)||PluginInfo.MemoryBswaped==FALSE) return FALSE;
	AiCloseDLL=(void (__cdecl*)(void))GetProcAddress(hAudioDll,"CloseDLL");
	if (AiCloseDLL==NULL) return FALSE;
	AiDacrateChanged=(void (__cdecl*)(int))GetProcAddress(hAudioDll,"AiDacrateChanged");
	if (AiDacrateChanged==NULL) return FALSE;
	AiLenChanged=(void (__cdecl*)(void))GetProcAddress(hAudioDll,"AiLenChanged");
	if (AiLenChanged==NULL) return FALSE;
	AiReadLength=(DWORD (__cdecl*)(void))GetProcAddress(hAudioDll,"AiReadLength");
	if (AiReadLength==NULL) return FALSE;
	InitiateAudio=(BOOL (__cdecl*)(AUDIO_INFO))GetProcAddress(hAudioDll,"InitiateAudio");
	if (InitiateAudio==NULL) return FALSE;
	AiRomClosed=(void (__cdecl*)(void))GetProcAddress(hAudioDll,"RomClosed");
	if (AiRomClosed==NULL) return FALSE;
	ProcessAList=(void (__cdecl*)(void))GetProcAddress(hAudioDll,"ProcessAList");
	if (ProcessAList==NULL) return FALSE;
	AiDllConfig=(void (__cdecl*)(HWND))GetProcAddress(hAudioDll,"DllConfig");
	AiUpdate=(void (__cdecl*)(BOOL))GetProcAddress(hAudioDll,"AiUpdate");
	return TRUE;
}
BOOL LoadControllerDll(char*ControllerDll) {
	PLUGIN_INFO PluginInfo;
	char DllName[300];
	GetPluginDir(DllName);
	strcat(DllName,ControllerDll);
	hControllerDll=LoadLibrary(DllName);
	if (hControllerDll==NULL) return FALSE;
	GetDllInfo=(void (__cdecl*)(PLUGIN_INFO*))GetProcAddress(hControllerDll,"GetDllInfo");
	if (GetDllInfo==NULL) return FALSE;
	PluginInfo.MemoryBswaped=TRUE;
	PluginInfo.NormalMemory=TRUE;
	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo)||PluginInfo.MemoryBswaped==FALSE) return FALSE;
	ContVersion=PluginInfo.Version;
	ContCloseDLL=(void (__cdecl*)(void))GetProcAddress(hControllerDll,"CloseDLL");
	if (ContCloseDLL==NULL) return FALSE;
	if (ContVersion==0x0100) {
		InitiateControllers_1_0=(void (__cdecl*)(HWND,CONTROL*))GetProcAddress(hControllerDll,"InitiateControllers");
		if (InitiateControllers_1_0==NULL) return FALSE;
	}
	if (ContVersion==0x0101) {
		InitiateControllers_1_1=(void (__cdecl*)(CONTROL_INFO))GetProcAddress(hControllerDll,"InitiateControllers");
		if (InitiateControllers_1_1==NULL) return FALSE;
	}
	ControllerCommand=(void (__cdecl*)(int,BYTE*))GetProcAddress(hControllerDll,"ControllerCommand");
	ReadController=(void (__cdecl*)(int,BYTE*))GetProcAddress(hControllerDll,"ReadController");
	ContConfig=(void (__cdecl*)(HWND))GetProcAddress(hControllerDll,"DllConfig");
	GetKeys=(void (__cdecl*)(int,BUTTONS*))GetProcAddress(hControllerDll,"GetKeys");
	WM_KeyDown=(void (__cdecl*)(WPARAM,LPARAM))GetProcAddress(hControllerDll,"WM_KeyDown");
	WM_KeyUp=(void (__cdecl*)(WPARAM,LPARAM))GetProcAddress(hControllerDll,"WM_KeyUp");
	ContRomOpen=(void (__cdecl*)(void))GetProcAddress(hControllerDll,"RomOpen");
	ContRomClosed=(void (__cdecl*)(void))GetProcAddress(hControllerDll,"RomClosed");
	RumbleCommand=(void (__cdecl*)(int,BOOL))GetProcAddress(hControllerDll,"RumbleCommand");
	return TRUE;
}
BOOL LoadGFXDll(char*RSPDLL) {
	PLUGIN_INFO PluginInfo;
	char DllName[300];
	GetPluginDir(DllName);
	strcat(DllName,GfxDLL);
	hGfxDll=LoadLibrary(DllName);
	if (hGfxDll==NULL) return FALSE;
	GetDllInfo=(void (__cdecl*)(PLUGIN_INFO*))GetProcAddress(hGfxDll,"GetDllInfo");
	if (GetDllInfo==NULL) return FALSE;
	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo)||PluginInfo.MemoryBswaped==FALSE) return FALSE;
	GFXCloseDLL=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"CloseDLL");
	if (GFXCloseDLL==NULL) return FALSE;
	ChangeWindow=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"ChangeWindow");
	if (ChangeWindow==NULL) return FALSE;
	GFXDllConfig=(void (__cdecl*)(HWND))GetProcAddress(hGfxDll,"DllConfig");
	DrawScreen=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"DrawScreen");
	if (DrawScreen==NULL) return FALSE;
	InitiateGFX=(BOOL (__cdecl*)(GFX_INFO))GetProcAddress(hGfxDll,"InitiateGFX");
	if (InitiateGFX==NULL) return FALSE;
	MoveScreen=(void (__cdecl*)(int,int))GetProcAddress(hGfxDll,"MoveScreen");
	if (MoveScreen==NULL) return FALSE;
	ProcessDList=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"ProcessDList");
	if (ProcessDList==NULL) return FALSE;
	GfxRomClosed=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"RomClosed");
	if (GfxRomClosed==NULL) return FALSE;
	GfxRomOpen=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"RomOpen");
	if (GfxRomOpen==NULL) return FALSE;
	UpdateScreen=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"UpdateScreen");
	if (UpdateScreen==NULL) return FALSE;
	ViStatusChanged=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"ViStatusChanged");
	if (ViStatusChanged==NULL) return FALSE;
	ViWidthChanged=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"ViWidthChanged");
	if (ViWidthChanged==NULL) return FALSE;
	if (PluginInfo.Version>=0x0103) {
		ProcessRDPList=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"ProcessRDPList");
		if (ProcessRDPList==NULL) return FALSE;
		CaptureScreen=(void (__cdecl*)(char*))GetProcAddress(hGfxDll,"CaptureScreen");
		if (CaptureScreen==NULL) return FALSE;
		ShowCFB=(void (__cdecl*)(void))GetProcAddress(hGfxDll,"ShowCFB");
		if (ShowCFB==NULL) return FALSE;
	} else {
		ProcessRDPList=NULL;
		CaptureScreen=NULL;
		ShowCFB=NULL;
	}
	return TRUE;
}
BOOL LoadRSPDLL(char*RSPDLL) {
	PLUGIN_INFO PluginInfo;
	char DllName[300];
	GetPluginDir(DllName);
	strcat(DllName,RSPDLL);
	hRSPDLL=LoadLibrary(DllName);
	if (hRSPDLL==NULL) return FALSE;
	GetDllInfo=(void (__cdecl*)(PLUGIN_INFO*))GetProcAddress(hRSPDLL,"GetDllInfo");
	if (GetDllInfo==NULL) return FALSE;
	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo)||PluginInfo.MemoryBswaped==FALSE) return FALSE;
	RSPVersion=PluginInfo.Version;
	if (RSPVersion==1) { RSPVersion=0x0100; }
	DoRspCycles=(DWORD (__cdecl*)(DWORD))GetProcAddress(hRSPDLL,"DoRspCycles");
	if (DoRspCycles==NULL) return FALSE;
	InitiateRSP_1_0=NULL;
	InitiateRSP_1_1=NULL;
	if (RSPVersion==0x100) {
		InitiateRSP_1_0=(void (__cdecl*)(RSP_INFO_1_0,DWORD*))GetProcAddress(hRSPDLL,"InitiateRSP");
		if (InitiateRSP_1_0==NULL) return FALSE;
	}
	if (RSPVersion==0x101) {
		InitiateRSP_1_1=(void (__cdecl*)(RSP_INFO_1_1,DWORD*))GetProcAddress(hRSPDLL,"InitiateRSP");
		if (InitiateRSP_1_1==NULL) return FALSE;
	}
	RSPRomClosed=(void (__cdecl*)(void))GetProcAddress(hRSPDLL,"RomClosed");
	if (RSPRomClosed==NULL) return FALSE;
	RSPCloseDLL=(void (__cdecl*)(void))GetProcAddress(hRSPDLL,"CloseDLL");
	if (RSPCloseDLL==NULL) return FALSE;
	return TRUE;
}
void SetupPlugins (HWND hWnd) {
	static DWORD AI_DUMMY=0;
	DWORD NewRAMsize;
	ShutdownPlugins();
	if (CPURunning) {
		ReadRomOptions();
		NewRAMsize=0x800000;
		if (RomJumperPak) NewRAMsize=0x400000;
		if (VirtualAlloc(RecompCode,LargeCompileBufferSize,MEM_COMMIT,PAGE_EXECUTE_READWRITE)==NULL) {
			DisplayError(GS(MSG_MEM_ALLOC_ERROR));
			DisplayThreadExit("SetupPlugins-VirtualAlloc(RecompCode,LargeCompileBufferSize,MEM_COMMIT,PAGE_EXECUTE_READWRITE)==NULL");
		}
		if (NewRAMsize!=RDRAMsize) {
			if (RDRAMsize==0x400000) {
				if (VirtualAlloc(N64MEM+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					DisplayThreadExit("SetupPlugins-VirtualAlloc(N64MEM+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL");
				}
				if (VirtualAlloc((BYTE*)JumpTable+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					DisplayThreadExit("SetupPlugins-VirtualAlloc((BYTE*)JumpTable+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL");
				}
				if (VirtualAlloc((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_COMMIT,PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					DisplayThreadExit("SetupPlugins-VirtualAlloc((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_COMMIT,PAGE_READWRITE)==NULL");
				}
			} else {
				VirtualFree(N64MEM+0x400000,0x400000,MEM_DECOMMIT);
				VirtualFree((BYTE*)JumpTable+0x400000,0x400000,MEM_DECOMMIT);
				VirtualFree((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_DECOMMIT);
			}
		}
		RDRAMsize=NewRAMsize;
		CpuRecompiler=RomCpuRecompiler;
		CountPerOp=SystemCF;
		if (RomCF!=-1) CountPerOp=RomCF;
		UseCache=SystemUseCache;
		if (RomUseCache!=UseCache_Default) UseCache=RomUseCache;
		SaveUsing=RomSaveUsing;
		if (RomSaveUsing==Auto) {
			if (strcmp(RomName,"CONKER BFD")==0||strcmp(RomName,"DRACULA MOKUSHIROKU")==0||strcmp(RomName,"DRACULA MOKUSHIROKU2")==0||strcmp(RomName,"BANJO KAZOOIE 2")==0||strcmp(RomName,"BANJO TOOIE")==0||strcmp(RomName,"CRUIS'N WORLD")==0||strcmp(RomName,"CUSTOMROBOV2")==0||strcmp(RomName,"DONKEY KONG 64")==0||strcmp(RomName,"ÄÞ×´ÓÝ2 Ë¶ØÉ¼ÝÃÞÝ")==0||strcmp(RomName,"ÄÞ×´ÓÝ3 ÉËÞÀÉÏÁSOS!")==0||strcmp(RomName,"EXCITEBIKE64")==0||strcmp(RomName,"²ÃÞÖ³½¹ÉÏ°¼Þ¬Ý¼Þ­¸")==0||strcmp(RomName,"NBA COURTSIDE")==0||strcmp(RomName,"Madden NFL 2002")==0||strcmp(RomName,"MarioParty3")==0||strcmp(RomName,"MarioTennis")==0||strcmp(RomName,"MarioTennis64")==0||strcmp(RomName,"EVANGELION")==0||strcmp(RomName,"Parlor PRO 64")==0||strcmp(RomName,"Ultraman Battle JAPA")==0||strcmp(RomName,"Perfect Dark")==0||strcmp(RomName,"RIDGE RACER 64")==0||strcmp(RomName,"Robopon 64")==0||strcmp(RomName,"STAR WARS EP1 RACER")==0||strcmp(RomName,"YOSHI STORY")==0) SaveUsing=EEPROM_16K;
			if (strcmp(RomName,"THE LEGEND OF ZELDA")==0) SaveUsing=SRAM;
			if (strcmp(RomName,"NBA Courtside 2")==0||strcmp(RomName,"POKEMON STADIUM")==0) SaveUsing=FlashRAM;
			// This masks most auto save type issues to extend compatibility for ROMhacks. The reason why it is stil set manually per Game.ini is for hacks with custom internal name and shared CRC.
		}
		ClearFrame=RomClearFrame;
		AudioSignal=RomAudioSignal;
		UseTLB=RomUseTLB;
		DelaySI=RomDelaySI;
		CF1CF0=RomCF1CF0;
		DelayRDP=RomDelayRDP;
		DelayRSP=RomDelayRSP;
		AlignDMA=RomAlignDMA;
		FiftyNineHertz=RomFiftyNineHertz;
		SyncGametoAudio=RomSyncGametoAudio;
		ProtectMemory=RomProtectMemory;
		if (SyncGametoAudio) LimitFPS=TRUE;
		EmulateAI=FALSE;
		if (strcmp(AudioDLL,"No Audio.dll")==0||RomJAI||RomShankleAziAI) EmulateAI=TRUE;
		LoadCheats();
		HandleWindowTitle();
		if (!LimitFPS) SyncGametoAudio=FALSE;
	} else GetCurrentDlls();
	PluginsInitialized=TRUE;
	if (!inFullScreen) {
		if (!LoadGFXDll(GfxDLL)) {
			PluginsInitialized=FALSE;
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,GS(MSG_FAIL_INIT_GFX),AppName,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDCANCEL) PluginsInitialized=TRUE;
			HandleModal2(hMainWindow);
		} else {
			GFX_INFO GfxInfo;
			GfxInfo.MemoryBswaped=TRUE;
			GfxInfo.CheckInterrupts=CheckInterrupts;
			GfxInfo.hStatusBar=hStatusWnd;
			GfxInfo.hWnd=hWnd;
			GfxInfo.HEADER=RomHeader;
			GfxInfo.RDRAM=N64MEM;
			GfxInfo.DMEM=DMEM;
			GfxInfo.IMEM=IMEM;
			GfxInfo.MI__INTR_REG=&MI_INTR_REG;
			GfxInfo.DPC__START_REG=&DPC_START_REG;
			GfxInfo.DPC__END_REG=&DPC_END_REG;
			GfxInfo.DPC__CURRENT_REG=&DPC_CURRENT_REG;
			GfxInfo.DPC__STATUS_REG=&DPC_STATUS_REG;
			GfxInfo.DPC__CLOCK_REG=&DPC_CLOCK_REG;
			GfxInfo.DPC__BUFBUSY_REG=&DPC_BUFBUSY_REG;
			GfxInfo.DPC__PIPEBUSY_REG=&DPC_PIPEBUSY_REG;
			GfxInfo.DPC__TMEM_REG=&DPC_TMEM_REG;
			GfxInfo.VI__STATUS_REG=&VI_STATUS_REG;
			GfxInfo.VI__ORIGIN_REG=&VI_ORIGIN_REG;
			GfxInfo.VI__WIDTH_REG=&VI_WIDTH_REG;
			GfxInfo.VI__INTR_REG=&VI_INTR_REG;
			GfxInfo.VI__V_CURRENT_LINE_REG=&VI_CURRENT_REG;
			GfxInfo.VI__TIMING_REG=&VI_TIMING_REG;
			GfxInfo.VI__V_SYNC_REG=&VI_V_SYNC_REG;
			GfxInfo.VI__H_SYNC_REG=&VI_H_SYNC_REG;
			GfxInfo.VI__LEAP_REG=&VI_LEAP_REG;
			GfxInfo.VI__H_START_REG=&VI_H_START_REG;
			GfxInfo.VI__V_START_REG=&VI_V_START_REG;
			GfxInfo.VI__V_BURST_REG=&VI_V_BURST_REG;
			GfxInfo.VI__X_SCALE_REG=&VI_X_SCALE_REG;
			GfxInfo.VI__Y_SCALE_REG=&VI_Y_SCALE_REG;
			if (!InitiateGFX(GfxInfo)) {
				if (GLideN64NeedsToBeSetupFirst) {
					GLideN64HasBeenSetupFirst=TRUE;
					GLideN64NeedsToBeSetupFirst=FALSE;
					strcpy(GfxDLL,"Icepir8sLegacyLLE.dll");
					SetupPlugins(hMainWindow);
					return;
				}
				PluginsInitialized=FALSE;
				HandleModal1(hMainWindow);
				if (MessageBox(NULL,GS(MSG_FAIL_INIT_GFX),AppName,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDCANCEL) PluginsInitialized=TRUE;
				HandleModal2(hMainWindow);
			}
		}
	}
	if (!GLideN64NeedsToBeSetupFirst) {
		if (!LoadAudioDll(AudioDLL)) {
			AiCloseDLL=NULL;
			AiDacrateChanged=NULL;
			AiLenChanged=NULL;
			AiReadLength=NULL;
			AiUpdate=NULL;
			InitiateAudio=NULL;
			ProcessAList=NULL;
			AiRomClosed=NULL;
			PluginsInitialized=FALSE;
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,GS(MSG_FAIL_INIT_AUDIO),AppName,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDCANCEL) PluginsInitialized=TRUE;
			HandleModal2(hMainWindow);
		} else {
			AUDIO_INFO AudioInfo;
			AudioInfo.hwnd=hWnd;
			AudioInfo.hinst=hInst;
			AudioInfo.MemoryBswaped=TRUE;
			AudioInfo.HEADER=RomHeader;
			AudioInfo.RDRAM=N64MEM;
			AudioInfo.DMEM=DMEM;
			AudioInfo.IMEM=IMEM;
			if (EmulateAI) AudioInfo.MI__INTR_REG=&AI_DUMMY;
			else AudioInfo.MI__INTR_REG=&AudioIntrReg;
			AudioInfo.AI__DRAM_ADDR_REG=&AI_DRAM_ADDR_REG;
			AudioInfo.AI__LEN_REG=&AI_LEN_REG;
			AudioInfo.AI__CONTROL_REG=&AI_CONTROL_REG;
			if (EmulateAI||SyncGametoAudio) AudioInfo.AI__STATUS_REG=&AI_DUMMY;
			else AudioInfo.AI__STATUS_REG=&AI_STATUS_REG;
			AudioInfo.AI__DACRATE_REG=&AI_DACRATE_REG;
			AudioInfo.AI__BITRATE_REG=&AI_BITRATE_REG;
			AudioInfo.CheckInterrupts=AiCheckInterrupts;
			if (EmulateAI) EmuAI_InitializePluginHook();
			if (!InitiateAudio(AudioInfo)) {
				AiCloseDLL=NULL;
				AiDacrateChanged=NULL;
				AiLenChanged=NULL;
				AiReadLength=NULL;
				AiUpdate=NULL;
				InitiateAudio=NULL;
				ProcessAList=NULL;
				AiRomClosed=NULL;
				PluginsInitialized=FALSE;
				HandleModal1(hMainWindow);
				if (MessageBox(NULL,GS(MSG_FAIL_INIT_AUDIO),AppName,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDCANCEL) PluginsInitialized=TRUE;
				HandleModal2(hMainWindow);
			}
			if (AiUpdate) {
				hAudioThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AudioThread,(LPVOID)NULL,0,NULL);
			}
		}
		if (!LoadControllerDll(ControllerDLL)) {
			PluginsInitialized=FALSE;
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,GS(MSG_FAIL_INIT_CONTROL),AppName,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDCANCEL) PluginsInitialized=TRUE;
			HandleModal2(hMainWindow);
		} else {
			Controllers[0].Present=FALSE;
			Controllers[0].RawData=FALSE;
			Controllers[0].Plugin=PLUGIN_NONE;
			Controllers[1].Present=FALSE;
			Controllers[1].RawData=FALSE;
			Controllers[1].Plugin=PLUGIN_NONE;
			Controllers[2].Present=FALSE;
			Controllers[2].RawData=FALSE;
			Controllers[2].Plugin=PLUGIN_NONE;
			Controllers[3].Present=FALSE;
			Controllers[3].RawData=FALSE;
			Controllers[3].Plugin=PLUGIN_NONE;
			if (ContVersion==0x0100) {
				InitiateControllers_1_0(hWnd,Controllers);
			}
			if (ContVersion==0x0101) {
				CONTROL_INFO ControlInfo;
				ControlInfo.Controls=Controllers;
				ControlInfo.HEADER=RomHeader;
				ControlInfo.hinst=hInst;
				ControlInfo.hMainWindow=hWnd;
				ControlInfo.MemoryBswaped=TRUE;
				InitiateControllers_1_1(ControlInfo);
			}
		}
		if (!LoadRSPDLL(RSPDLL)) {
			PluginsInitialized=FALSE;
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,GS(MSG_FAIL_INIT_RSP),AppName,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND)==IDCANCEL) PluginsInitialized=TRUE;
			HandleModal2(hMainWindow);
		} else {
			DWORD dwDataListsFromPluginInterpreterCore=0x00000000,dwDataListsToPluginRecompilerCore=0x00000001,dwDisposition;
			const char*regPath="PJ64 V 1.6.2\\Configuration\\1.6 RSP";
			HKEY hKey;
			LONG lRet;
			lRet=RegCreateKeyExA(HKEY_CURRENT_USER,regPath,0,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hKey,&dwDisposition);
			if (lRet==ERROR_SUCCESS&&CPURunning&&strcmp(RSPDLL,"RSP.dll")==0) {
				if (RomRspRecompiler) RegSetValueEx(hKey,"RSP Core",0,REG_DWORD,(const BYTE*)&dwDataListsToPluginRecompilerCore,sizeof(dwDataListsToPluginRecompilerCore));
				else RegSetValueEx(hKey,"RSP Core",0,REG_DWORD,(const BYTE*)&dwDataListsFromPluginInterpreterCore,sizeof(dwDataListsFromPluginInterpreterCore));
				if (strcmp(AudioDLL,"Jabo_Dsound.dll")==0) RegSetValueEx(hKey,"List Type",0,REG_DWORD,(const BYTE*)&dwDataListsFromPluginInterpreterCore,sizeof(dwDataListsFromPluginInterpreterCore));
				else RegSetValueEx(hKey,"List Type",0,REG_DWORD,(const BYTE*)&dwDataListsToPluginRecompilerCore,sizeof(dwDataListsToPluginRecompilerCore));
				RegCloseKey(hKey);
			}
			RSP_INFO_1_0 RspInfo10;
			RSP_INFO_1_1 RspInfo11;
			RspInfo10.CheckInterrupts=CheckInterrupts;
			RspInfo11.CheckInterrupts=CheckInterrupts;
			RspInfo10.ProcessDlist=ProcessDList;
			RspInfo11.ProcessDlist=ProcessDList;
			RspInfo10.ProcessAlist=ProcessAList;
			RspInfo11.ProcessAlist=ProcessAList;
			RspInfo10.ProcessRdpList=ProcessRDPList;
			RspInfo11.ProcessRdpList=ProcessRDPList;
			RspInfo11.ShowCFB=ShowCFB;
			RspInfo10.hInst=hInst;
			RspInfo11.hInst=hInst;
			RspInfo10.RDRAM=N64MEM;
			RspInfo11.RDRAM=N64MEM;
			RspInfo10.DMEM=DMEM;
			RspInfo11.DMEM=DMEM;
			RspInfo10.IMEM=IMEM;
			RspInfo11.IMEM=IMEM;
			RspInfo10.MemoryBswaped=FALSE;
			RspInfo11.MemoryBswaped=FALSE;
			RspInfo10.MI__INTR_REG=&MI_INTR_REG;
			RspInfo11.MI__INTR_REG=&MI_INTR_REG;
			RspInfo10.SP__MEM_ADDR_REG=&SP_MEM_ADDR_REG;
			RspInfo11.SP__MEM_ADDR_REG=&SP_MEM_ADDR_REG;
			RspInfo10.SP__DRAM_ADDR_REG=&SP_DRAM_ADDR_REG;
			RspInfo11.SP__DRAM_ADDR_REG=&SP_DRAM_ADDR_REG;
			RspInfo10.SP__RD_LEN_REG=&SP_RD_LEN_REG;
			RspInfo11.SP__RD_LEN_REG=&SP_RD_LEN_REG;
			RspInfo10.SP__WR_LEN_REG=&SP_WR_LEN_REG;
			RspInfo11.SP__WR_LEN_REG=&SP_WR_LEN_REG;
			RspInfo10.SP__STATUS_REG=&SP_STATUS_REG;
			RspInfo11.SP__STATUS_REG=&SP_STATUS_REG;
			RspInfo10.SP__DMA_FULL_REG=&SP_DMA_FULL_REG;
			RspInfo11.SP__DMA_FULL_REG=&SP_DMA_FULL_REG;
			RspInfo10.SP__DMA_BUSY_REG=&SP_DMA_BUSY_REG;
			RspInfo11.SP__DMA_BUSY_REG=&SP_DMA_BUSY_REG;
			RspInfo10.SP__PC_REG=&SP_PC_REG;
			RspInfo11.SP__PC_REG=&SP_PC_REG;
			RspInfo10.SP__SEMAPHORE_REG=&SP_SEMAPHORE_REG;
			RspInfo11.SP__SEMAPHORE_REG=&SP_SEMAPHORE_REG;
			RspInfo10.DPC__START_REG=&DPC_START_REG;
			RspInfo11.DPC__START_REG=&DPC_START_REG;
			RspInfo10.DPC__END_REG=&DPC_END_REG;
			RspInfo11.DPC__END_REG=&DPC_END_REG;
			RspInfo10.DPC__CURRENT_REG=&DPC_CURRENT_REG;
			RspInfo11.DPC__CURRENT_REG=&DPC_CURRENT_REG;
			RspInfo10.DPC__STATUS_REG=&DPC_STATUS_REG;
			RspInfo11.DPC__STATUS_REG=&DPC_STATUS_REG;
			RspInfo10.DPC__CLOCK_REG=&DPC_CLOCK_REG;
			RspInfo11.DPC__CLOCK_REG=&DPC_CLOCK_REG;
			RspInfo10.DPC__BUFBUSY_REG=&DPC_BUFBUSY_REG;
			RspInfo11.DPC__BUFBUSY_REG=&DPC_BUFBUSY_REG;
			RspInfo10.DPC__PIPEBUSY_REG=&DPC_PIPEBUSY_REG;
			RspInfo11.DPC__PIPEBUSY_REG=&DPC_PIPEBUSY_REG;
			RspInfo10.DPC__TMEM_REG=&DPC_TMEM_REG;
			RspInfo11.DPC__TMEM_REG=&DPC_TMEM_REG;
			if (RSPVersion==0x0100) { InitiateRSP_1_0(RspInfo10,&RspTaskValue); }
			if (RSPVersion==0x0101) { InitiateRSP_1_1(RspInfo11,&RspTaskValue); }
		}
	}
	if (!PluginsInitialized) {
		if (inFullScreen) SendMessage(hMainWindow,WM_COMMAND,ID_OPTIONS_FULLSCREEN,0);
		SendMessage(hMainWindow,WM_COMMAND,ID_OPTIONS_SETTINGS,0);
		if (CPURunning) {
			GetCurrentDlls();
			HideRomBrowser();
		} else SetupPlugins(hMainWindow);
		return;
	}
	if (!GLideN64NeedsToBeSetupFirst) SetupMenu(hMainWindow);
	if (CPURunning) {
		DWORD count;
		memset(&CPU_Action,0,sizeof(CPU_Action));
		CPU_Action.hStepping=CreateEvent(NULL,FALSE,FALSE,NULL);
		WrittenToRom=FALSE;
		memset(N64MEM,0,RDRAMsize);
		InitializeTLB();
		InitializeR4300iRegisters(LoadPifRom(*(ROM+0x3D)),*(ROM+0x3D),GetCicChipID(ROM));
		BuildInterpreter();
		RecompPos=RecompCode;
		Timers.CurrentTimerType=-1;
		Timers.Timer=0;
		CurrentFrame=0;
		for (count=0; count<MaxTimers; count++) { Timers.Active[count]=FALSE; }
		ChangeTimer(ViTimer,5000);
		ChangeCompareTimer();
		ViFieldSerration=0;
		DMAUsed=FALSE;
		CPU_Paused=FALSE;
		ManualPaused=FALSE;
		Timer_Start();
		if (CpuRecompiler) hCPU=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartRecompilerCPU,NULL,0,NULL);
		else hCPU=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartInterpreterCPU,NULL,0,NULL);
		if (strcmp(GfxDLL,"GLideN64.dll")==0&&!GLideN64HasBeenSetupFirst) GLideN64HasBeenSetupFirst=TRUE;
		if (GLideN64NeedsToBeSetupFirst) {
			EndEmulation();
			GLideN64NeedsToBeSetupFirst=FALSE;
			strcpy(GfxDLL,"Icepir8sLegacyLLE.dll");
			SetupPlugins(hMainWindow);
			return;
		}
		if (EmulateAI&&!LimitFPS) {
			LimitFPS=TRUE;
			HandleTimers();
			LimitFPS=FALSE;
			if (SpeedCap) HandleTimers();
		} else HandleTimers();
		if (strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0) SetWindowLong(hMainWindow,GWL_EXSTYLE,GetWindowLong(hMainWindow,GWL_EXSTYLE)|WS_EX_COMPOSITED);
	}
}
void SetupPluginScreen (HWND hDlg) {
	WIN32_FIND_DATA FindData;
	PLUGIN_INFO PluginInfo;
	char SearchsStr[300],Plugin[300];
	HANDLE hFind,hLib;
	int index;
	SetDlgItemText(hDlg,RSP_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,GFX_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,AUDIO_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,CONT_ABOUT,GS(PLUG_ABOUT));
	SetDlgItemText(hDlg,IDC_RSP_NAME,GS(PLUG_RSP));
	SetDlgItemText(hDlg,IDC_GFX_NAME,GS(PLUG_GFX));
	SetDlgItemText(hDlg,IDC_AUDIO_NAME,GS(PLUG_AUDIO));
	SetDlgItemText(hDlg,IDC_CONT_NAME,GS(PLUG_CTRL));
	GetPluginDir(SearchsStr);
	strcat(SearchsStr,"*.dll");
	hFind=FindFirstFile(SearchsStr,&FindData);
	if (hFind==INVALID_HANDLE_VALUE) return;
	PluginCount=0;
	for (;;) {
		PluginNames[PluginCount]=malloc(strlen(FindData.cFileName)+1);
		strcpy(PluginNames[PluginCount],FindData.cFileName);
		GetPluginDir(Plugin);
		strcat(Plugin,PluginNames[PluginCount]);
		hLib=LoadLibrary(Plugin);
		if (hLib==NULL&&!BootupSettings) {
			DisplayError("%s\n %s",GS(MSG_FAIL_LOAD_PLUGIN),Plugin);
			if (FindNextFile(hFind,&FindData)==0) return;
			continue;
		}
		GetDllInfo=(void (__cdecl*)(PLUGIN_INFO*))GetProcAddress(hLib,"GetDllInfo");
		if (GetDllInfo==NULL) {
			FreeLibrary(hLib);
			if (FindNextFile(hFind,&FindData)==0) return;
			continue;
		}
		GetDllInfo(&PluginInfo);
		if (!ValidPluginVersion(&PluginInfo)||
			(PluginInfo.Type!=PLUGIN_TYPE_CONTROLLER&&PluginInfo.MemoryBswaped==FALSE))
		{
			FreeLibrary(hLib);
			if (FindNextFile(hFind,&FindData)==0) return;
			continue;
		}
		switch(PluginInfo.Type) {
		case PLUGIN_TYPE_RSP:
			index=SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_ADDSTRING,(WPARAM)0,(LPARAM)&PluginInfo.Name);
			SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_SETITEMDATA,(WPARAM)index,(LPARAM)PluginCount);
			if(_stricmp(RSPDLL,PluginNames[PluginCount])==0) {
				SendMessage(GetDlgItem(hDlg,RSP_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
				RSPDLLAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,RSP_ABOUT),RSPDLLAbout!=NULL?TRUE:FALSE);
			} else {
				FreeLibrary(hLib);
			}
			break;
		case PLUGIN_TYPE_GFX:
			index=SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_ADDSTRING,(WPARAM)0,(LPARAM)&PluginInfo.Name);
			SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_SETITEMDATA,(WPARAM)index,(LPARAM)PluginCount);
			if(_stricmp(GfxDLL,PluginNames[PluginCount])==0) {
				SendMessage(GetDlgItem(hDlg,GFX_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
				GFXDllAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,GFX_ABOUT),GFXDllAbout!=NULL?TRUE:FALSE);
			} else {
				FreeLibrary(hLib);
			}
			break;
		case PLUGIN_TYPE_AUDIO:
			index=SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_ADDSTRING,(WPARAM)0,(LPARAM)&PluginInfo.Name);
			SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_SETITEMDATA,(WPARAM)index,(LPARAM)PluginCount);
			if(_stricmp(AudioDLL,PluginNames[PluginCount])==0) {
				SendMessage(GetDlgItem(hDlg,AUDIO_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
				AiDllAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,AUDIO_ABOUT),AiDllAbout!=NULL?TRUE:FALSE);
			} else {
				FreeLibrary(hLib);
			}
			break;
		case PLUGIN_TYPE_CONTROLLER:
			index=SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_ADDSTRING,(WPARAM)0,(LPARAM)&PluginInfo.Name);
			SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_SETITEMDATA,(WPARAM)index,(LPARAM)PluginCount);
			if(_stricmp(ControllerDLL,PluginNames[PluginCount])==0) {
				SendMessage(GetDlgItem(hDlg,CONT_LIST),CB_SETCURSEL,(WPARAM)index,(LPARAM)0);
				ContDllAbout=(void (__cdecl*)(HWND))GetProcAddress(hLib,"DllAbout");
				EnableWindow(GetDlgItem(hDlg,CONT_ABOUT),ContDllAbout!=NULL?TRUE:FALSE);
			} else {
				FreeLibrary(hLib);
			}
			break;
		}
		PluginCount+=1;
		if (FindNextFile(hFind,&FindData)==0) return;
	}
}
void ShutdownPlugins (void) {
	if (!inFullScreen) {
		if (GFXCloseDLL!=NULL) { GFXCloseDLL(); }
		FreeLibrary(hGfxDll);
	}
	if (!GLideN64NeedsToBeSetupFirst) {
		TerminateThread(hAudioThread,0);
		if (AiCloseDLL!=NULL) { AiCloseDLL(); }
		FreeLibrary(hAudioDll);
		if (ContCloseDLL!=NULL) { ContCloseDLL(); }
		FreeLibrary(hControllerDll);
		if (RSPCloseDLL!=NULL) { RSPCloseDLL(); }
		FreeLibrary(hRSPDLL);
	}
	PluginsInitialized=FALSE;
}
BOOL ValidPluginVersion (PLUGIN_INFO*PluginInfo) {
	switch (PluginInfo->Type) {
	case PLUGIN_TYPE_RSP:
		if (PluginInfo->Version==0x0001||PluginInfo->Version==0x0100||PluginInfo->Version==0x0101) return TRUE;
	case PLUGIN_TYPE_GFX:
		if (PluginInfo->Version==0x0102||PluginInfo->Version==0x0103) return TRUE;
	case PLUGIN_TYPE_AUDIO:
		if (PluginInfo->Version==0x0101) return TRUE;
	case PLUGIN_TYPE_CONTROLLER:
		if (PluginInfo->Version==0x0100||PluginInfo->Version==0x0101) return TRUE;
	}
	return FALSE;
}