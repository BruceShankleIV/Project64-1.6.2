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
#include "UnZIP.h"
#include "ZIP.h"
#include "CPU.h"
#include "Cheat.h"
#include "Plugin.h"
#include "Resource.h"
int NextInstruction,JumpToLocation,ManualPaused,CPU_Paused,CountPerOp;
char SaveAsFileName[255],LoadFileName[255];
double CountsPerByte;
int DlistCount,AlistCount,Start_COUNT,EmuAI_FrameRate,CurrentSaveSlot;
enum SaveType SaveUsing;
CPU_ACTION CPU_Action;
SYSTEM_TIMERS Timers;
HANDLE hPauseMutex;
OPCODE Opcode;
HANDLE hCPU;
BOOL inFullScreen,CPURunning;
DWORD MemoryStack,EmuAI_Frequency,EmuAI_VICntFrame,EmuAI_BitRate,EmuAI_Buffer[2],LastVICntFrame;
char *TimeName[MaxTimers]={ "CompareTimer","SiTimer","PiTimer","ViTimer" };
void (__cdecl *AiDacrateChangedPlugin) (int SystemType);
void (__cdecl *AiLenChangedPlugin)     (void);
DWORD (__cdecl *AiReadLengthPlugin)    (void);
void __cdecl EmuAI_AiLenChanged (void) {
	EmuAI_BitRate=AI_BITRATE_REG+1;
	if (AI_LEN_REG==0) return;
	if (LastVICntFrame!=EmuAI_VICntFrame&&EmuAI_Frequency>0) {
		CountsPerByte=(double)(EmuAI_FrameRate * EmuAI_VICntFrame) / (double)(EmuAI_Frequency * (EmuAI_BitRate / 4));
		LastVICntFrame=EmuAI_VICntFrame;
	}
	if (CountsPerByte==0)	CountsPerByte=(double)(EmuAI_FrameRate * EmuAI_VICntFrame) / (double)(22050 * (16 / 4));
	if (EmuAI_Buffer[0]==0) {
		Start_COUNT=COUNT_REGISTER;
		if (RomAltEmulateAI) EmuAI_Buffer[1]=AI_LEN_REG&0x3FFF8;
		else EmuAI_Buffer[0]=AI_LEN_REG&0x3FFF8;
		AI_STATUS_REG|=0x40000000;
		if (RomAltEmulateAI) ChangeTimer(AiTimer,(int)(CountsPerByte * 50));
		else ChangeTimer(AiTimer,(int)(CountsPerByte * (double)(double)(EmuAI_Buffer[0])));
	} else if (EmuAI_Buffer[1]==0) {
		EmuAI_Buffer[1]=AI_LEN_REG&0x3FFF8;
		AI_STATUS_REG|=0x80000000;
	} else AI_STATUS_REG|=0x80000000;
	if (AiLenChangedPlugin!=NULL) AiLenChangedPlugin();
}
DWORD __cdecl EmuAI_AiReadLength (void) {
	DWORD SetTimer,RemainingCount,retVal=0;
	SetTimer=(DWORD)(int)(CountsPerByte * (double)(EmuAI_Buffer[0]));
	RemainingCount=SetTimer-(COUNT_REGISTER-Start_COUNT);
	if (EmuAI_Buffer[0]==0) return 0;
	retVal=(DWORD)((double)RemainingCount / CountsPerByte);
	if (retVal<8) retVal=8;
	AiReadLengthPlugin();
	return retVal&~0x7;
}
void __cdecl EmuAI_AiDacrateChanged (int SystemType) {
	if (EmuAI_FrameRate==60) {
		EmuAI_Frequency=48681812 / (AI_DACRATE_REG+1);
		if (AiDacrateChangedPlugin!=NULL) AiDacrateChangedPlugin(SYSTEM_NTSC);
	} else {
		EmuAI_Frequency=49656530 / (AI_DACRATE_REG+1);
		if (AiDacrateChangedPlugin!=NULL) AiDacrateChangedPlugin(SYSTEM_PAL);
	}
}
void EmuAI_ClearAudio() {
	LastVICntFrame=0;
	CountsPerByte=0;
	EmuAI_Buffer[0]=EmuAI_Buffer[1]=0;
	EmuAI_Frequency=0;
	AI_STATUS_REG=0;
	if (AI_LEN_REG>0) {
		Start_COUNT=COUNT_REGISTER;
		AudioIntrReg|=MI_INTR_AI;
		AiCheckInterrupts();
	}
}
void EmuAI_InitializePluginHook() {
		LastVICntFrame=0;
		CountsPerByte=0;
		EmuAI_Buffer[0]=EmuAI_Buffer[1]=0;
		EmuAI_Frequency=0;
		if (AiDacrateChanged==EmuAI_AiDacrateChanged) return;
		AiDacrateChangedPlugin=AiDacrateChanged;
		AiLenChangedPlugin=AiLenChanged;
		AiReadLengthPlugin=AiReadLength;
		AiDacrateChanged=EmuAI_AiDacrateChanged;
		AiLenChanged=EmuAI_AiLenChanged;
		AiReadLength=EmuAI_AiReadLength;
}
void EmuAI_SetFrameRate (int frameRate) {
	EmuAI_FrameRate=frameRate;
}
void EmuAI_SetVICountPerFrame (DWORD value) {
	EmuAI_VICntFrame=value;
}
void EmuAI_SetNextTimer() {
	EmuAI_Buffer[0]=EmuAI_Buffer[1];
	EmuAI_Buffer[1]=0;
	AI_STATUS_REG&=~0x80000000;
	if (EmuAI_Buffer[0]>0) {
		AI_STATUS_REG|=0x40000000;
		ChangeTimer(AiTimer,(int)(CountsPerByte * (double)(EmuAI_Buffer[0])));
		Start_COUNT=COUNT_REGISTER;
	} else {
		AI_STATUS_REG&=~0x40000000;
		ChangeTimer(AiTimer,0);
	}
}
void ResetFunction (void) {
	if (strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0) {
		GetCurrentDlls();
		HideRomBrowser();
	} else {
		GetCurrentDlls();
		if (strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0) HideRomBrowser();
		else SetupPlugins(hMainWindow);
	}
}
void DisplayThreadExit (char * ExitPoint) {
	DisplayError(GS(THREAD_EXIT));
	if (UsuallyonTop) SetWindowPos(hMainWindow,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREPOSITION|SWP_NOSIZE);
	DisplayError("Exit Point: %s",ExitPoint);
	ExitThread(0);
}
void INITIALIZECPUFlags (void) {
	inFullScreen=FALSE;
	CPURunning =FALSE;
	CurrentSaveSlot=ID_CURRENTSAVE_DEFAULT;
}
void ChangeCompareTimer(void) {
	DWORD NextCompare=COMPARE_REGISTER-COUNT_REGISTER;
	if ((NextCompare&0x80000000)!=0) { NextCompare=0x7FFFFFFF; }
	if (NextCompare==0) { NextCompare=0x1; }
	ChangeTimer(CompareTimer,NextCompare);
}
void ChangeTimer(int Type,int Value) {
	if (Value==0) {
		Timers.NextTimer[Type]=0;
		Timers.Active[Type]=FALSE;
		return;
	}
	Timers.NextTimer[Type]=Value-Timers.Timer;
	Timers.Active[Type]=TRUE;
	CheckTimer();
}
void CheckTimer (void) {
	int count;
	for (count=0; count<MaxTimers; count++) {
		if (!Timers.Active[count]) continue;
		if (!(count==CompareTimer&&Timers.NextTimer[count]==0x7FFFFFFF)) {
			Timers.NextTimer[count] +=Timers.Timer;
		}
	}
	Timers.CurrentTimerType=-1;
	Timers.Timer=0x7FFFFFFF;
	for (count=0; count<MaxTimers; count++) {
		if (!Timers.Active[count]) continue;
		if (Timers.NextTimer[count]>=Timers.Timer) continue;
		Timers.Timer=Timers.NextTimer[count];
		Timers.CurrentTimerType=count;
	}
	if (Timers.CurrentTimerType==-1) DisplayThreadExit("CheckTimer-Timers.CurrentTimerType==-1");
	for (count=0; count<MaxTimers; count++) {
		if (!Timers.Active[count]) continue;
		if (!(count==CompareTimer&&Timers.NextTimer[count]==0x7FFFFFFF)) {
			Timers.NextTimer[count] -=Timers.Timer;
		}
	}
	if (Timers.NextTimer[CompareTimer]==0x7FFFFFFF) {
		DWORD NextCompare=COMPARE_REGISTER-COUNT_REGISTER;
		if ((NextCompare&0x80000000)==0&&NextCompare!=0x7FFFFFFF) {
			ChangeCompareTimer();
		}
	}
}
void EndEmulation (void) {
	DWORD ExitCode,count,OldProtect;
	if (SPECIAL_BREAK_Yes) SPECIAL_BREAK_Yes=FALSE;
	if (SPECIAL_BREAK_Trigger) SPECIAL_BREAK_Trigger=FALSE;
	if (BF_Trigger) BF_Trigger=FALSE;
	SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)"");
	ManualPaused=FALSE;
	CPU_Action.CloseCPU=TRUE;
	CPU_Action.Stepping=FALSE;
	CPU_Action.DoSomething=TRUE;
	PulseEvent(CPU_Action.hStepping);
	timeBeginPeriod(16);
	for (count=0; count<20; count++) {
		Sleep(100);
		GetExitCodeThread(hCPU,&ExitCode);
		if (ExitCode!=STILL_ACTIVE) {
			hCPU=NULL;
			count=100;
		}
	}
	timeEndPeriod(16);
	if (hCPU!=NULL) { TerminateThread(hCPU,0); hCPU=NULL; }
	CloseHandle(CPU_Action.hStepping);
	CloseEEPROM();
	CloseMempak();
	CloseSRAM();
	Timer_Stop();
	VirtualProtect(N64MEM,RDRAMsize,PAGE_READWRITE,&OldProtect);
	VirtualProtect(N64MEM+0x04000000,0x2000,PAGE_READWRITE,&OldProtect);
	if (GfxRomClosed!=NULL&&(!inFullScreen||strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0)) { GfxRomClosed(); }
	if (!GLideN64NeedsToBeSetupFirst) {
		if (AiRomClosed!=NULL) { AiRomClosed(); }
		if (ContRomClosed!=NULL) { ContRomClosed(); }
		if (RSPRomClosed!=NULL) { RSPRomClosed(); }
	}
}
int DelaySlotEffectsCompare (DWORD PC,DWORD Reg1,DWORD Reg2) {
	OPCODE Command;
	if (!r4300i_LW_VAddr(PC+4,&Command.Hex)) return TRUE;
	switch (Command.op) {
	case R4300i_SPECIAL:
		switch (Command.funct) {
		case R4300i_SPECIAL_SLL:
		case R4300i_SPECIAL_SRL:
		case R4300i_SPECIAL_SRA:
		case R4300i_SPECIAL_SLLV:
		case R4300i_SPECIAL_SRLV:
		case R4300i_SPECIAL_SRAV:
		case R4300i_SPECIAL_MFHI:
		case R4300i_SPECIAL_MTHI:
		case R4300i_SPECIAL_MFLO:
		case R4300i_SPECIAL_MTLO:
		case R4300i_SPECIAL_DSLLV:
		case R4300i_SPECIAL_DSRLV:
		case R4300i_SPECIAL_DSRAV:
		case R4300i_SPECIAL_ADD:
		case R4300i_SPECIAL_ADDU:
		case R4300i_SPECIAL_SUB:
		case R4300i_SPECIAL_SUBU:
		case R4300i_SPECIAL_AND:
		case R4300i_SPECIAL_OR:
		case R4300i_SPECIAL_XOR:
		case R4300i_SPECIAL_NOR:
		case R4300i_SPECIAL_SLT:
		case R4300i_SPECIAL_SLTU:
		case R4300i_SPECIAL_DADD:
		case R4300i_SPECIAL_DADDU:
		case R4300i_SPECIAL_DSUB:
		case R4300i_SPECIAL_DSUBU:
		case R4300i_SPECIAL_DSLL:
		case R4300i_SPECIAL_DSRL:
		case R4300i_SPECIAL_DSRA:
		case R4300i_SPECIAL_DSLL32:
		case R4300i_SPECIAL_DSRL32:
		case R4300i_SPECIAL_DSRA32:
			if (Command.rd==0) return FALSE;
			if (Command.rd==Reg1) return TRUE;
			if (Command.rd==Reg2) return TRUE;
		case R4300i_SPECIAL_MULT:
		case R4300i_SPECIAL_MULTU:
		case R4300i_SPECIAL_DIV:
		case R4300i_SPECIAL_DIVU:
		case R4300i_SPECIAL_DMULT:
		case R4300i_SPECIAL_DMULTU:
		case R4300i_SPECIAL_DDIV:
		case R4300i_SPECIAL_DDIVU:
			break;
		default: return TRUE;
		}
		break;
	case R4300i_CP0:
		switch (Command.rs) {
		case R4300i_COP0_MF:
			if (Command.rt==0) return FALSE;
			if (Command.rt==Reg1||Command.rt==Reg2) return TRUE;
		case R4300i_COP0_MT: break;
		default:
			if ((Command.rs&0x10)!=0&&(Opcode.funct==R4300i_COP0_CO_TLBR||Opcode.funct==R4300i_COP0_CO_TLBWI||Opcode.funct==R4300i_COP0_CO_TLBWR||Opcode.funct==R4300i_COP0_CO_TLBP)) break;
			return TRUE;
		}
		break;
	case R4300i_CP1:
		switch (Command.fmt) {
		case R4300i_COP1_MF:
			if (Command.rt==0) return FALSE;
			if (Command.rt==Reg1) return TRUE;
			if (Command.rt==Reg2) return TRUE;
		case R4300i_COP1_CF:
		case R4300i_COP1_MT:
		case R4300i_COP1_CT:
		case R4300i_COP1_S:
		case R4300i_COP1_D:
		case R4300i_COP1_W:
		case R4300i_COP1_L: break;
		default: return TRUE;
		}
		break;
	case R4300i_ANDI:
	case R4300i_ORI:
	case R4300i_XORI:
	case R4300i_LUI:
	case R4300i_ADDI:
	case R4300i_ADDIU:
	case R4300i_SLTI:
	case R4300i_SLTIU:
	case R4300i_DADDI:
	case R4300i_DADDIU:
	case R4300i_LB:
	case R4300i_LH:
	case R4300i_LW:
	case R4300i_LWL:
	case R4300i_LWR:
	case R4300i_LDL:
	case R4300i_LDR:
	case R4300i_LBU:
	case R4300i_LHU:
	case R4300i_LD:
	case R4300i_LWC1:
	case R4300i_LDC1:
		if (Command.rt==0) return FALSE;
		if (Command.rt==Reg1) return TRUE;
		if (Command.rt==Reg2) return TRUE;
	case R4300i_CACHE:
	case R4300i_SB:
	case R4300i_SH:
	case R4300i_SW:
	case R4300i_SWR:
	case R4300i_SWL:
	case R4300i_SWC1:
	case R4300i_SDC1:
	case R4300i_SD: break;
	default: return TRUE;
	}
	return FALSE;
}
int DelaySlotEffectsJump (DWORD JumpPC) {
	OPCODE Command;
	if (!r4300i_LW_VAddr(JumpPC,&Command.Hex)) return TRUE;
	switch (Command.op) {
	case R4300i_SPECIAL:
		switch (Command.funct) {
		case R4300i_SPECIAL_JR:	return DelaySlotEffectsCompare(JumpPC,Command.rs,0);
		case R4300i_SPECIAL_JALR: return DelaySlotEffectsCompare(JumpPC,Command.rs,31);
		}
		break;
	case R4300i_REGIMM:
		switch (Command.rt) {
		case R4300i_REGIMM_BLTZ:
		case R4300i_REGIMM_BGEZ:
		case R4300i_REGIMM_BLTZL:
		case R4300i_REGIMM_BGEZL:
		case R4300i_REGIMM_BLTZAL:
		case R4300i_REGIMM_BGEZAL:
			return DelaySlotEffectsCompare(JumpPC,Command.rs,0);
		}
		break;
	case R4300i_JAL:
	case R4300i_SPECIAL_JALR: return DelaySlotEffectsCompare(JumpPC,31,0);
	case R4300i_J: return FALSE;
	case R4300i_BEQ:
	case R4300i_BNE:
	case R4300i_BLEZ:
	case R4300i_BGTZ:
		return DelaySlotEffectsCompare(JumpPC,Command.rs,Command.rt);
	case R4300i_CP1:
		switch (Command.fmt) {
		case R4300i_COP1_BC:
			switch (Command.ft) {
			case R4300i_COP1_BC_BCF:
			case R4300i_COP1_BC_BCT:
			case R4300i_COP1_BC_BCFL:
			case R4300i_COP1_BC_BCTL:
				{
					int EffectDelaySlot;
					OPCODE NewCommand;
					if (!r4300i_LW_VAddr(JumpPC+4,&NewCommand.Hex)) return TRUE;
					EffectDelaySlot=FALSE;
					if (NewCommand.op==R4300i_CP1) {
						if (NewCommand.fmt==R4300i_COP1_S&&(NewCommand.funct&0x30)==0x30) {
							EffectDelaySlot=TRUE;
						}
						if (NewCommand.fmt==R4300i_COP1_D&&(NewCommand.funct&0x30)==0x30) {
							EffectDelaySlot=TRUE;
						}
					}
					return EffectDelaySlot;
				}
			}
		}
	break;
	case R4300i_BEQL:
	case R4300i_BNEL:
	case R4300i_BLEZL:
	case R4300i_BGTZL:
		return DelaySlotEffectsCompare(JumpPC,Command.rs,Command.rt);
	}
	return TRUE;
}
void ProcessMessages (void) {
	HANDLE hEvent;
	MSG msg;
	hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	MsgWaitForMultipleObjects(1,&hEvent,FALSE,1000,QS_ALLINPUT);
	CloseHandle(hEvent);
	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message==WM_QUIT) {
			PostMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
			return;
		}
	}
}
void DoSomething (void) {
	if (CPU_Action.CloseCPU) {
		CoUninitialize();
		ExitThread(0);
	}
	if (CPU_Action.CheckInterrupts) {
		CPU_Action.CheckInterrupts=FALSE;
		CheckInterrupts();
	}
	if (CPU_Action.DoInterrupt) {
		CPU_Action.DoInterrupt=FALSE;
		DoIntrException(FALSE);
	}
	if (CPU_Action.Pause) {
		WaitForSingleObject(hPauseMutex,INFINITE);
		if (CPU_Action.Pause) {
			HMENU hMenu=GetMenu(hMainWindow);
			HMENU hSubMenu=GetSubMenu(hMenu,1);
			MenuSetText(hSubMenu,1,GS(MENU_RESUME),"F2/Shift+Caps Lock");
			CurrentFrame=0;
			CPU_Paused=TRUE;
			CPU_Action.Pause=FALSE;
			ReleaseMutex(hPauseMutex);
			SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MSG_CPU_PAUSED));
			DisplayFPS();
			if (DrawScreen!=NULL) DrawScreen();
			WaitForSingleObject(hPauseMutex,INFINITE);
			if (CPU_Paused) {
				ReleaseMutex(hPauseMutex);
				SuspendThread(hCPU);
			} else {
				ReleaseMutex(hPauseMutex);
			}
		} else {
			ReleaseMutex(hPauseMutex);
		}
	}
	CPU_Action.DoSomething=FALSE;
	if (CPU_Action.SaveState) {
		//test if allowed
		CPU_Action.SaveState=FALSE;
		if (!Machine_SaveState()) {
			CPU_Action.SaveState=TRUE;
			CPU_Action.DoSomething=TRUE;
		}
	}
	if (CPU_Action.RestoreState) {
		CPU_Action.RestoreState=FALSE;
		Machine_LoadState();
	}
	if (CPU_Action.DoInterrupt==TRUE) { CPU_Action.DoSomething=TRUE; }
}
void GetAutoSaveDir(char * Directory) {
	char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT],Dir[255],Group[200];
	long lResult;
	HKEY hKeyResults=0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath(path_buffer,drive,dir,fname,ext);
	sprintf(Directory,"%s%sSave Data\\",drive,dir);
	sprintf(Group,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Value,Bytes;
		Bytes=4;
		lResult=RegQueryValueEx(hKeyResults,"AppPath Save Data",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult==ERROR_SUCCESS&&Value==FALSE) {
			Bytes=sizeof(Dir);
			lResult=RegQueryValueEx(hKeyResults,"CustomPath Save Data",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult==ERROR_SUCCESS) { strcpy(Directory,Dir); }
		}
	}
	RegCloseKey(hKeyResults);
}
void GetInstantSaveDir(char * Directory) {
	char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT],Dir[255],Group[200];
	long lResult;
	HKEY hKeyResults=0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath(path_buffer,drive,dir,fname,ext);
	sprintf(Directory,"%s%sSave States\\",drive,dir);
	sprintf(Group,"PJ64 V 1.6.2\\Configuration");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Value,Bytes;
		Bytes=4;
		lResult=RegQueryValueEx(hKeyResults,"AppPath Save States",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult==ERROR_SUCCESS&&Value==FALSE) {
			Bytes=sizeof(Dir);
			lResult=RegQueryValueEx(hKeyResults,"CustomPath SaveStates",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult==ERROR_SUCCESS) { strcpy(Directory,Dir); }
		}
	}
	RegCloseKey(hKeyResults);
}
void InPermLoop (void) {
	// *** Changed ***/
	if (CPU_Action.DoInterrupt) return;
	/* Interrupts enabled */
	if ((STATUS_REGISTER&STATUS_IE)==0||(STATUS_REGISTER&STATUS_EXL)!=0||(STATUS_REGISTER&STATUS_ERL)!=0||(STATUS_REGISTER&0xFF00)==0) {
		if (UpdateScreen!=NULL) { UpdateScreen(); }
		CurrentFrame=0;
		DisplayFPS();
		DisplayThreadExit("InPermLoop-(STATUS_REGISTER&STATUS_IE )==0||(STATUS_REGISTER&STATUS_EXL)!=0||(STATUS_REGISTER&STATUS_ERL)!=0||(STATUS_REGISTER&0xFF00)==0");
	}
	/* check sound playing */
	/* check RSP running */
	/* check RDP running */
	if (Timers.Timer>0) {
		COUNT_REGISTER +=Timers.Timer+1;
		Timers.Timer=-1;
	}
}
BOOL Machine_LoadState(void) {
	char Directory[255],FileName[255],ZipFile[255],LoadHeader[64],String[100];
	char drive[_MAX_DRIVE],dir[_MAX_DIR],ext[_MAX_EXT];
	DWORD dwRead,Value,count,SaveRDRAMsize;
	BOOL LoadedZipFile=FALSE;
	HANDLE hSaveFile;
	unzFile file;
	if (strlen(LoadFileName)==0) {
		GetInstantSaveDir(Directory);
		sprintf(FileName,"%s%s",Directory,CurrentSave);
	} else {
		strcpy(FileName,LoadFileName);
		strcpy(ZipFile,LoadFileName);
	}
	file=unzOpen(ZipFile);
	if (file!=NULL) {
		unz_file_info info;
		char zname[132];
		int port=0;
		port=unzGoToFirstFile(file);
		while (port==UNZ_OK&&LoadedZipFile==FALSE) {
			unzGetCurrentFileInfo(file,&info,zname,128,NULL,0,NULL,0);
			if (unzLocateFile(file,zname,1)!=UNZ_OK) {
				unzClose(file);
				port=-1;
				continue;
			}
			if(unzOpenCurrentFile(file)!=UNZ_OK) {
				unzClose(file);
				port=-1;
				continue;
			}
			unzReadCurrentFile(file,&Value,4);
			if (Value!=0x23D8A6C8) {
				unzCloseCurrentFile(file);
				continue;
			}
			unzReadCurrentFile(file,&SaveRDRAMsize,sizeof(SaveRDRAMsize));
			unzReadCurrentFile(file,LoadHeader,0x40);
			if (CpuRecompiler) {
				ResetRecompCode();
			}
			Timers.CurrentTimerType=-1;
			Timers.Timer=0;
			for (count=0; count<MaxTimers; count ++) { Timers.Active[count]=FALSE; }
			//fix RDRAM size
			if (SaveRDRAMsize!=RDRAMsize) {
				if (RDRAMsize==0x400000) {
					if (VirtualAlloc(N64MEM+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
						DisplayError(GS(MSG_MEM_ALLOC_ERROR));
						DisplayThreadExit("1 Machine_LoadState-VirtualAlloc(N64MEM+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL");
					}
					if (VirtualAlloc((BYTE*)JumpTable+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
						DisplayError(GS(MSG_MEM_ALLOC_ERROR));
						DisplayThreadExit("1 Machine_LoadState-VirtualAlloc((BYTE*)JumpTable+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL");
					}
					if (VirtualAlloc((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_COMMIT,PAGE_READWRITE)==NULL) {
						DisplayError(GS(MSG_MEM_ALLOC_ERROR));
						DisplayThreadExit("1 Machine_LoadState-VirtualAlloc((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_COMMIT,PAGE_READWRITE)==NULL");
					}
				} else {
					VirtualFree(N64MEM+0x400000,0x400000,MEM_DECOMMIT);
					VirtualFree((BYTE*)JumpTable+0x400000,0x400000,MEM_DECOMMIT);
					VirtualFree((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_DECOMMIT);
				}
			}
			RDRAMsize=SaveRDRAMsize;
			unzReadCurrentFile(file,&Value,sizeof(Value));
			ChangeTimer(ViTimer,Value);
			unzReadCurrentFile(file,&PROGRAM_COUNTER,sizeof(PROGRAM_COUNTER));
			unzReadCurrentFile(file,GPR,sizeof(_int64)*32);
			unzReadCurrentFile(file,FPR,sizeof(_int64)*32);
			unzReadCurrentFile(file,CP0,sizeof(DWORD)*32);
			unzReadCurrentFile(file,FPCR,sizeof(DWORD)*32);
			unzReadCurrentFile(file,&HI,sizeof(_int64));
			unzReadCurrentFile(file,&LO,sizeof(_int64));
			unzReadCurrentFile(file,RegRDRAM,sizeof(DWORD)*10);
			unzReadCurrentFile(file,RegSP,sizeof(DWORD)*10);
			unzReadCurrentFile(file,RegDPC,sizeof(DWORD)*10);
			unzReadCurrentFile(file,RegMI,sizeof(DWORD)*4);
			unzReadCurrentFile(file,RegVI,sizeof(DWORD)*14);
			unzReadCurrentFile(file,RegAI,sizeof(DWORD)*6);
			unzReadCurrentFile(file,RegPI,sizeof(DWORD)*13);
			unzReadCurrentFile(file,RegRI,sizeof(DWORD)*8);
			unzReadCurrentFile(file,RegSI,sizeof(DWORD)*4);
			unzReadCurrentFile(file,tlb,sizeof(TLB)*32);
			unzReadCurrentFile(file,PIF_Ram,0x40);
			unzReadCurrentFile(file,RDRAM,RDRAMsize);
			unzReadCurrentFile(file,DMEM,0x1000);
			unzReadCurrentFile(file,IMEM,0x1000);
			unzCloseCurrentFile(file);
			unzClose(file);
			LoadedZipFile=TRUE;
			_splitpath(ZipFile,drive,dir,ZipFile,ext);
			sprintf(FileName,"%s%s",ZipFile,ext);
		}
	}
	if (!LoadedZipFile) {
		hSaveFile=CreateFile(FileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
		if (hSaveFile==INVALID_HANDLE_VALUE) {
			DisplayError(GS(MSG_UNABLE_LOAD_STATE));
			_splitpath(FileName,drive,dir,ZipFile,ext);
			sprintf(String,"%s: %s%s",GS(MSG_UNABLE_LOAD_STATE),ZipFile,ext);
			SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)String);
			return FALSE;
		}
		SetFilePointer(hSaveFile,0,NULL,FILE_BEGIN);
		ReadFile(hSaveFile,&Value,sizeof(Value),&dwRead,NULL);
		if (Value!=0x23D8A6C8) return FALSE;
		ReadFile(hSaveFile,&SaveRDRAMsize,sizeof(SaveRDRAMsize),&dwRead,NULL);
		ReadFile(hSaveFile,LoadHeader,0x40,&dwRead,NULL);
		if (CpuRecompiler) {
			ResetRecompCode();
		}
		Timers.CurrentTimerType=-1;
		Timers.Timer=0;
		for (count=0; count<MaxTimers; count ++) { Timers.Active[count]=FALSE; }
		//fix RDRAM size
		if (SaveRDRAMsize!=RDRAMsize) {
			if (RDRAMsize==0x400000) {
				if (VirtualAlloc(N64MEM+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					DisplayThreadExit("2 Machine_LoadState-VirtualAlloc(N64MEM+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL");
				}
				if (VirtualAlloc((BYTE*)JumpTable+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					DisplayThreadExit("2 Machine_LoadState-VirtualAlloc((BYTE*)JumpTable+0x400000,0x400000,MEM_COMMIT,PAGE_READWRITE)==NULL");
				}
				if (VirtualAlloc((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_COMMIT,PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					DisplayThreadExit("2 Machine_LoadState-VirtualAlloc((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_COMMIT,PAGE_READWRITE)==NULL");
				}
			} else {
				VirtualFree(N64MEM+0x400000,0x400000,MEM_DECOMMIT);
				VirtualFree((BYTE*)JumpTable+0x400000,0x400000,MEM_DECOMMIT);
				VirtualFree((BYTE*)DelaySlotTable+(0x400000>>0xA),(0x400000>>0xA),MEM_DECOMMIT);
			}
		}
		RDRAMsize=SaveRDRAMsize;
		ReadFile(hSaveFile,&Value,sizeof(Value),&dwRead,NULL);
		ChangeTimer(ViTimer,Value);
		ReadFile(hSaveFile,&PROGRAM_COUNTER,sizeof(PROGRAM_COUNTER),&dwRead,NULL);
		ReadFile(hSaveFile,GPR,sizeof(_int64)*32,&dwRead,NULL);
		ReadFile(hSaveFile,FPR,sizeof(_int64)*32,&dwRead,NULL);
		ReadFile(hSaveFile,CP0,sizeof(DWORD)*32,&dwRead,NULL);
		ReadFile(hSaveFile,FPCR,sizeof(DWORD)*32,&dwRead,NULL);
		ReadFile(hSaveFile,&HI,sizeof(_int64),&dwRead,NULL);
		ReadFile(hSaveFile,&LO,sizeof(_int64),&dwRead,NULL);
		ReadFile(hSaveFile,RegRDRAM,sizeof(DWORD)*10,&dwRead,NULL);
		ReadFile(hSaveFile,RegSP,sizeof(DWORD)*10,&dwRead,NULL);
		ReadFile(hSaveFile,RegDPC,sizeof(DWORD)*10,&dwRead,NULL);
		ReadFile(hSaveFile,RegMI,sizeof(DWORD)*4,&dwRead,NULL);
		ReadFile(hSaveFile,RegVI,sizeof(DWORD)*14,&dwRead,NULL);
		ReadFile(hSaveFile,RegAI,sizeof(DWORD)*6,&dwRead,NULL);
		ReadFile(hSaveFile,RegPI,sizeof(DWORD)*13,&dwRead,NULL);
		ReadFile(hSaveFile,RegRI,sizeof(DWORD)*8,&dwRead,NULL);
		ReadFile(hSaveFile,RegSI,sizeof(DWORD)*4,&dwRead,NULL);
		ReadFile(hSaveFile,tlb,sizeof(TLB)*32,&dwRead,NULL);
		ReadFile(hSaveFile,PIF_Ram,0x40,&dwRead,NULL);
		ReadFile(hSaveFile,RDRAM,RDRAMsize,&dwRead,NULL);
		ReadFile(hSaveFile,DMEM,0x1000,&dwRead,NULL);
		ReadFile(hSaveFile,IMEM,0x1000,&dwRead,NULL);
		CloseHandle(hSaveFile);
		_splitpath(FileName,drive,dir,ZipFile,ext);
		sprintf(FileName,"%s%s",ZipFile,ext);
	}
	ChangeCompareTimer();
	if (AiRomClosed!=NULL&&!SyncGametoAudio) AiRomClosed();
	DlistCount=0;
	AlistCount=0;
	AI_STATUS_REG=0;
	EmuAI_ClearAudio();
	AiDacrateChanged(SYSTEM_NTSC);
	ViStatusChanged();
	ViWidthChanged();
	SetupTLB();
	//Fix up Memory stack location
	MemoryStack=GPR[29].W[0];
	TranslateVaddr(&MemoryStack);
	MemoryStack +=(DWORD)N64MEM;
	CheckInterrupts();
	DMAUsed=TRUE;
	strcpy(SaveAsFileName,"");
	strcpy(LoadFileName,"");
	sprintf(String,"%s: %s",GS(MSG_LOADED_STATE),FileName);
	SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)String);
	if (SPECIAL_BREAK_Yes) SPECIAL_BREAK_Yes=FALSE;
	if (SPECIAL_BREAK_Trigger) SPECIAL_BREAK_Trigger=FALSE;
	if (BF_Trigger) BF_Trigger=FALSE;
	return TRUE;
}
BOOL Machine_SaveState(void) {
	char Directory[255],FileName[255],ZipFile[255],String[100];
	char drive[_MAX_DRIVE],dir[_MAX_DIR],ext[_MAX_EXT];
	DWORD dwWritten,Value;
	HANDLE hSaveFile;
	if (Timers.CurrentTimerType!=CompareTimer&&Timers.CurrentTimerType!=ViTimer) return FALSE;
	if (strlen(SaveAsFileName)==0) {
		GetInstantSaveDir(Directory);
		sprintf(FileName,"%s%s",Directory,CurrentSave);
	} else sprintf(FileName,"%s",SaveAsFileName);
	{
		hSaveFile=CreateFile(FileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
		if (hSaveFile==INVALID_HANDLE_VALUE) {
			switch (GetLastError()) {
			case ERROR_PATH_NOT_FOUND:
				CreateDirectory(Directory,NULL);
				hSaveFile=CreateFile(FileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
				if (hSaveFile==INVALID_HANDLE_VALUE) {
					DisplayError(GS(MSG_UNABLE_SAVE_STATE));
					_splitpath(FileName,drive,dir,ZipFile,ext);
					sprintf(String,"%s %s%s",GS(MSG_UNABLE_SAVE_STATE),ZipFile,ext);
					SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)String);
					return TRUE;
				}
				break;
			default:
				DisplayError(GS(MSG_UNABLE_SAVE_STATE));
				_splitpath(FileName,drive,dir,ZipFile,ext);
				sprintf(String,"%s %s%s",GS(MSG_UNABLE_SAVE_STATE),ZipFile,ext);
				SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)String);
				return TRUE;
			}
		}
		while ((int)Registers.CP0[1]<(int)Registers.CP0[6]) {
			Registers.CP0[1] +=32-Registers.CP0[6];
		}
		SetFilePointer(hSaveFile,0,NULL,FILE_BEGIN);
		Value=0x23D8A6C8;
		WriteFile(hSaveFile,&Value,sizeof(Value),&dwWritten,NULL);
		WriteFile(hSaveFile,&RDRAMsize,sizeof(RDRAMsize),&dwWritten,NULL);
		WriteFile(hSaveFile,RomHeader,0x40,&dwWritten,NULL);
		Value=Timers.NextTimer[ViTimer]+Timers.Timer;
		WriteFile(hSaveFile,&Value,sizeof(Value),&dwWritten,NULL);
		WriteFile(hSaveFile,&PROGRAM_COUNTER,sizeof(PROGRAM_COUNTER),&dwWritten,NULL);
		WriteFile(hSaveFile,GPR,sizeof(_int64)*32,&dwWritten,NULL);
		WriteFile(hSaveFile,FPR,sizeof(_int64)*32,&dwWritten,NULL);
		WriteFile(hSaveFile,CP0,sizeof(DWORD)*32,&dwWritten,NULL);
		WriteFile(hSaveFile,FPCR,sizeof(DWORD)*32,&dwWritten,NULL);
		WriteFile(hSaveFile,&HI,sizeof(_int64),&dwWritten,NULL);
		WriteFile(hSaveFile,&LO,sizeof(_int64),&dwWritten,NULL);
		WriteFile(hSaveFile,RegRDRAM,sizeof(DWORD)*10,&dwWritten,NULL);
		WriteFile(hSaveFile,RegSP,sizeof(DWORD)*10,&dwWritten,NULL);
		WriteFile(hSaveFile,RegDPC,sizeof(DWORD)*10,&dwWritten,NULL);
		Value=MI_INTR_REG;
		if (AiReadLength()!=0) { MI_INTR_REG|=MI_INTR_AI; }
		WriteFile(hSaveFile,RegMI,sizeof(DWORD)*4,&dwWritten,NULL);
		MI_INTR_REG=Value;
		WriteFile(hSaveFile,RegVI,sizeof(DWORD)*14,&dwWritten,NULL);
		WriteFile(hSaveFile,RegAI,sizeof(DWORD)*6,&dwWritten,NULL);
		WriteFile(hSaveFile,RegPI,sizeof(DWORD)*13,&dwWritten,NULL);
		WriteFile(hSaveFile,RegRI,sizeof(DWORD)*8,&dwWritten,NULL);
		WriteFile(hSaveFile,RegSI,sizeof(DWORD)*4,&dwWritten,NULL);
		WriteFile(hSaveFile,tlb,sizeof(TLB)*32,&dwWritten,NULL);
		WriteFile(hSaveFile,PIF_Ram,0x40,&dwWritten,NULL);
		WriteFile(hSaveFile,RDRAM,RDRAMsize,&dwWritten,NULL);
		WriteFile(hSaveFile,DMEM,0x1000,&dwWritten,NULL);
		WriteFile(hSaveFile,IMEM,0x1000,&dwWritten,NULL);
		CloseHandle(hSaveFile);
		DeleteFile(ZipFile);
		_splitpath(FileName,drive,dir,ZipFile,ext);
		sprintf(FileName,"%s%s",ZipFile,ext);
	}
	strcpy(SaveAsFileName,"");
	strcpy(LoadFileName,"");
	static BOOL toggle=FALSE;
	sprintf(String,"%s: %s %s",GS(MSG_SAVED_STATE),FileName,toggle?"<<":">>");
	SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)String);
	toggle=!toggle;
	return TRUE;
}
void PauseCPU (void) {
	if (!CPURunning) return;
	if (CPU_Paused||CPU_Action.Pause) {
		HMENU hMenu=GetMenu(hMainWindow);
		HMENU hSubMenu=GetSubMenu(hMenu,1);
		if (CPU_Action.Pause) {
			CPU_Action.Pause=FALSE;
			CPU_Paused=FALSE;
			ManualPaused=FALSE;
			SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MSG_CPU_RESUMED));
			ReleaseMutex(hPauseMutex);
			return;
		}
		ResumeThread(hCPU);
		SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM)GS(MSG_CPU_RESUMED));
		MenuSetText(hSubMenu,1,GS(MENU_PAUSE),"F2/Shift+Caps Lock");
		ManualPaused=FALSE;
		CPU_Paused=FALSE;
		if (LimitFPS&&!SyncGametoAudio||!LimitFPS&&SpeedCap) Timer_Start();
	} else {
		CPU_Action.Pause=TRUE;
		CPU_Action.DoSomething=TRUE;
	}
	ReleaseMutex(hPauseMutex);
}
void RefreshScreen (void) {
	static DWORD OLD_VI_V_SYNC_REG=0,VI_INTR_TIME=500000;
	LARGE_INTEGER Time;
	if (OLD_VI_V_SYNC_REG!=VI_V_SYNC_REG) {
		if (VI_V_SYNC_REG==0) {
			VI_INTR_TIME=500000;
		} else {
			VI_INTR_TIME=(VI_V_SYNC_REG+1) * 1500;
			if ((VI_V_SYNC_REG % 1)!=0) {
				VI_INTR_TIME -=38;
			}
		}
	}
	ChangeTimer(ViTimer,Timers.Timer+Timers.NextTimer[ViTimer]+VI_INTR_TIME);
	EmuAI_SetVICountPerFrame(VI_INTR_TIME);
	UpdateFieldSerration((VI_STATUS_REG&0x40)!=0);
	if (LimitFPS||SpeedCap) { Timer_Process(NULL); }
	if ((CurrentFrame&7)==0) {
		QueryPerformanceCounter(&Time);
		Frames[(CurrentFrame>>3) % 9].QuadPart=Time.QuadPart-LastFrame.QuadPart;
		LastFrame.QuadPart=Time.QuadPart;
		DisplayFPS();
	}
	CurrentFrame +=1;
	__try {
		if (UpdateScreen!=NULL) UpdateScreen();
	} __except (r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())) { DisplayThreadExit("RefreshScreen-r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())"); }
	if ((STATUS_REGISTER&STATUS_IE)!=0) { ApplyCheats(); }
}
void RunRsp (void) {
	if ((SP_STATUS_REG&SP_STATUS_HALT)==0) {
		if ((SP_STATUS_REG&SP_STATUS_BROKE)==0) {
			DWORD Task=*(DWORD *)(DMEM+0xFC0);
			switch (Task) {
			case 1:
				if ((DPC_STATUS_REG&DPC_STATUS_FREEZE)!=0) return;
				DlistCount +=1;
				break;
			case 2:
				AlistCount +=1;
				break;
			}
		}
		DoRspCycles(100);
	}
}
void SetCoreToRunning (void) {
	CPU_Action.Stepping=FALSE;
	PulseEvent(CPU_Action.hStepping);
}
void SetCoreToStepping (void) {
	CPU_Action.Stepping=TRUE;
}
void StepOpcode (void) {
	PulseEvent(CPU_Action.hStepping);
}
void TimerDone (void) {
	switch (Timers.CurrentTimerType) {
	case CompareTimer:
		FAKE_CAUSE_REGISTER|=CAUSE_IP7;
		CheckInterrupts();
		ChangeCompareTimer();
		break;
	case SiTimer:
		ChangeTimer(SiTimer,0);
		MI_INTR_REG|=MI_INTR_SI;
		SI_STATUS_REG|=SI_STATUS_INTERRUPT;
		CheckInterrupts();
		break;
	case PiTimer:
		ChangeTimer(PiTimer,0);
		PI_STATUS_REG&=~PI_STATUS_DMA_BUSY;
		MI_INTR_REG|=MI_INTR_PI;
		CheckInterrupts();
		break;
	case ViTimer:
		RefreshScreen();
		MI_INTR_REG|=MI_INTR_VI;
		CheckInterrupts();
		break;
	case RspTimer:
		ChangeTimer(RspTimer,0);
		RunRsp();
#ifndef MIN_SIZE
		CheckInterrupts();
#endif
		break;
	case AiTimer:
		EmuAI_SetNextTimer();
		AudioIntrReg|=MI_INTR_AI;
		AiCheckInterrupts();
		break;
	}
	CheckTimer();
}