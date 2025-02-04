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
#include "unzip.h"
#include "zip.h"
#include "cpu.h"
#include "cheats.h"
#include "plugin.h"
#include "resource.h"
char CommandName[100];
char strLabelName[100];
int NextInstruction, JumpToLocation, ManualPaused, CPU_Paused, CountPerOp;
char SaveAsFileName[255], LoadFileName[255];
int DlistCount, AlistCount, CurrentSaveSlot;
enum SaveType SaveUsing;
CPU_ACTION CPU_Action;
SYSTEM_TIMERS Timers;
HANDLE hPauseMutex;
OPCODE Opcode;
HANDLE hCPU;
BOOL inFullScreen, CPURunning, SPHack;
DWORD MemoryStack;
char *TimeName[MaxTimers] = { "CompareTimer","SiTimer","PiTimer","ViTimer" };
void INITIALIZECPUFlags (void) {
	inFullScreen = FALSE;
	CPURunning   = FALSE;
	CurrentSaveSlot = ID_CURRENTSAVE_DEFAULT;
	SPHack       = FALSE;
}
void ChangeCompareTimer(void) {
	DWORD NextCompare = COMPARE_REGISTER - COUNT_REGISTER;
	if ((NextCompare & 0x80000000) != 0) {  NextCompare = 0x7FFFFFFF; }
	if (NextCompare == 0) { NextCompare = 0x1; }
	ChangeTimer(CompareTimer,NextCompare);
}
void ChangeTimer(int Type, int Value) {
	if (Value == 0) {
		Timers.NextTimer[Type] = 0;
		Timers.Active[Type] = FALSE;
		return;
	}
	Timers.NextTimer[Type] = Value - Timers.Timer;
	Timers.Active[Type] = TRUE;
	CheckTimer();
}
void CheckTimer (void) {
	int count;
	for (count = 0; count < MaxTimers; count++) {
		if (!Timers.Active[count]) { continue; }
		if (!(count == CompareTimer && Timers.NextTimer[count] == 0x7FFFFFFF)) {
			Timers.NextTimer[count] += Timers.Timer;
		}
	}
	Timers.CurrentTimerType = -1;
	Timers.Timer = 0x7FFFFFFF;
	for (count = 0; count < MaxTimers; count++) {
		if (!Timers.Active[count]) { continue; }
		if (Timers.NextTimer[count] >= Timers.Timer) { continue; }
		Timers.Timer = Timers.NextTimer[count];
		Timers.CurrentTimerType = count;
	}
	if (Timers.CurrentTimerType == -1) ExitThread(0);
	for (count = 0; count < MaxTimers; count++) {
		if (!Timers.Active[count]) { continue; }
		if (!(count == CompareTimer && Timers.NextTimer[count] == 0x7FFFFFFF)) {
			Timers.NextTimer[count] -= Timers.Timer;
		}
	}
	if (Timers.NextTimer[CompareTimer] == 0x7FFFFFFF) {
		DWORD NextCompare = COMPARE_REGISTER - COUNT_REGISTER;
		if ((NextCompare & 0x80000000) == 0 && NextCompare != 0x7FFFFFFF) {
			ChangeCompareTimer();
		}
	}
}
void CloseCpu (void) {
	DWORD ExitCode, count, OldProtect;
	if (!CPURunning) { return; }
	ManualPaused = FALSE;
	if (CPU_Paused) { PauseCpu (); }
	{
		BOOL Temp = AlwaysOnTop;
		AlwaysOnTop = FALSE;
		AlwaysOnTopWindow(hMainWindow);
		AlwaysOnTop = Temp;
	}
	for (count = 0; count < 50;/*Note to self, setting this any higher than 50 can cause plugin hot swaps to crash*/count ++ ) {
		CPU_Action.CloseCPU = TRUE;
		CPU_Action.Stepping = FALSE;
		CPU_Action.DoSomething = TRUE;
		PulseEvent( CPU_Action.hStepping );
		if (WaitForSingleObject(hCPU,1) == WAIT_OBJECT_0) {
			GetExitCodeThread(hCPU, &ExitCode);
			if (ExitCode != STILL_ACTIVE) hCPU = NULL;
		}
	}
	if (hCPU != NULL) {  TerminateThread(hCPU,0); hCPU = NULL; }
	CPURunning = FALSE;
	VirtualProtect(N64MEM,RDRAMsize,PAGE_READWRITE,&OldProtect);
	VirtualProtect(N64MEM + 0x04000000,0x2000,PAGE_READWRITE,&OldProtect);
	Timer_Stop();
	CloseeepROM();
	CloseMempak();
	CloseSRAM();
	if (GfxRomClosed != NULL)  { GfxRomClosed(); }
	if (AiRomClosed != NULL)   { AiRomClosed(); }
	if (ContRomClosed != NULL) { ContRomClosed(); }
	if (RSPRomClosed) { RSPRomClosed(); }
	CloseHandle(CPU_Action.hStepping);
}
char * LabelName (DWORD Address) {
	sprintf(strLabelName,"0x%08X",Address);
	return strLabelName;
}
char * R4300iRegImmName ( DWORD OpCode, DWORD PC ) {
	OPCODE command;
	command.Hex = OpCode;
	switch (command.rt) {
	case R4300i_REGIMM_BLTZ:
		sprintf(CommandName,"bltz\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_REGIMM_BGEZ:
		if (command.rs == 0) {
			sprintf(CommandName,"b\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
		} else {
			sprintf(CommandName,"bgez\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		}
		break;
	case R4300i_REGIMM_BLTZL:
		sprintf(CommandName,"bltzl\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_REGIMM_BGEZL:
		sprintf(CommandName,"bgezl\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_REGIMM_TGEI:
		sprintf(CommandName,"tgei\t%s, 0x%X",GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_REGIMM_TGEIU:
		sprintf(CommandName,"tgeiu\t%s, 0x%X",GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_REGIMM_TLTI:
		sprintf(CommandName,"tlti\t%s, 0x%X",GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_REGIMM_TLTIU:
		sprintf(CommandName,"tltiu\t%s, 0x%X",GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_REGIMM_TEQI:
		sprintf(CommandName,"teqi\t%s, 0x%X",GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_REGIMM_TNEI:
		sprintf(CommandName,"tnei\t%s, 0x%X",GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_REGIMM_BLTZAL:
		sprintf(CommandName,"bltzal\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_REGIMM_BGEZAL:
		if (command.rs == 0) {
			sprintf(CommandName,"bal\t%s",LabelName(PC + ((short)command.offset << 2) + 4));
		} else {
			sprintf(CommandName,"bgezal\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		}
		break;
	case R4300i_REGIMM_BLTZALL:
		sprintf(CommandName,"bltzall\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_REGIMM_BGEZALL:
		sprintf(CommandName,"bgezall\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	default:
		sprintf(CommandName,"Unknown\t%02X %02X %02X %02X",
			command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
	}
	return CommandName;
}
char * R4300iSpecialName ( DWORD OpCode, DWORD PC ) {
	OPCODE command;
	command.Hex = OpCode;
	switch (command.funct) {
	case R4300i_SPECIAL_SLL:
		if (command.Hex != 0) {
			sprintf(CommandName,"sll\t%s, %s, 0x%X",GPR_Name[command.rd],
			GPR_Name[command.rt], command.sa);
		} else {
			sprintf(CommandName,"nop");
		}
		break;
	case R4300i_SPECIAL_SRL:
		sprintf(CommandName,"srl\t%s, %s, 0x%X",GPR_Name[command.rd], GPR_Name[command.rt],
			command.sa);
		break;
	case R4300i_SPECIAL_SRA:
		sprintf(CommandName,"sra\t%s, %s, 0x%X",GPR_Name[command.rd], GPR_Name[command.rt],
				command.sa);
		break;
	case R4300i_SPECIAL_SLLV:
		sprintf(CommandName,"sllv\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rt],
			GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_SRLV:
		sprintf(CommandName,"srlv\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rt],
			GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_SRAV:
		sprintf(CommandName,"srav\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rt],
			GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_JR:
		sprintf(CommandName,"jr\t%s",GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_JALR:
		sprintf(CommandName,"jalr\t%s, %s",GPR_Name[command.rd],GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_SYSCALL:
		sprintf(CommandName,"system call");
		break;
	case R4300i_SPECIAL_BREAK:
		sprintf(CommandName,"break");
		break;
	case R4300i_SPECIAL_SYNC:
		sprintf(CommandName,"sync");
		break;
	case R4300i_SPECIAL_MFHI:
		sprintf(CommandName,"mfhi\t%s",GPR_Name[command.rd]);
		break;
	case R4300i_SPECIAL_MTHI:
		sprintf(CommandName,"mthi\t%s",GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_MFLO:
		sprintf(CommandName,"mflo\t%s",GPR_Name[command.rd]);
		break;
	case R4300i_SPECIAL_MTLO:
		sprintf(CommandName,"mtlo\t%s",GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_DSLLV:
		sprintf(CommandName,"dsllv\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rt],
			GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_DSRLV:
		sprintf(CommandName,"dsrlv\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rt],
			GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_DSRAV:
		sprintf(CommandName,"dsrav\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rt],
			GPR_Name[command.rs]);
		break;
	case R4300i_SPECIAL_MULT:
		sprintf(CommandName,"mult\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_MULTU:
		sprintf(CommandName,"multu\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DIV:
		sprintf(CommandName,"div\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DIVU:
		sprintf(CommandName,"divu\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DMULT:
		sprintf(CommandName,"dmult\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DMULTU:
		sprintf(CommandName,"dmultu\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DDIV:
		sprintf(CommandName,"ddiv\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DDIVU:
		sprintf(CommandName,"ddivu\t%s, %s",GPR_Name[command.rs], GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_ADD:
		sprintf(CommandName,"add\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_ADDU:
		sprintf(CommandName,"addu\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_SUB:
		sprintf(CommandName,"sub\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_SUBU:
		sprintf(CommandName,"subu\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_AND:
		sprintf(CommandName,"and\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_OR:
		sprintf(CommandName,"or\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_XOR:
		sprintf(CommandName,"xor\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_NOR:
		sprintf(CommandName,"nor\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_SLT:
		sprintf(CommandName,"slt\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_SLTU:
		sprintf(CommandName,"sltu\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DADD:
		sprintf(CommandName,"dadd\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DADDU:
		sprintf(CommandName,"daddu\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DSUB:
		sprintf(CommandName,"dsub\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DSUBU:
		sprintf(CommandName,"dsubu\t%s, %s, %s",GPR_Name[command.rd], GPR_Name[command.rs],
			GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_TGE:
		sprintf(CommandName,"tge\t%s, %s",GPR_Name[command.rs],GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_TGEU:
		sprintf(CommandName,"tgeu\t%s, %s",GPR_Name[command.rs],GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_TLT:
		sprintf(CommandName,"tlt\t%s, %s",GPR_Name[command.rs],GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_TLTU:
		sprintf(CommandName,"tltu\t%s, %s",GPR_Name[command.rs],GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_TEQ:
		sprintf(CommandName,"teq\t%s, %s",GPR_Name[command.rs],GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_TNE:
		sprintf(CommandName,"tne\t%s, %s",GPR_Name[command.rs],GPR_Name[command.rt]);
		break;
	case R4300i_SPECIAL_DSLL:
		sprintf(CommandName,"dsll\t%s, %s, 0x%X",GPR_Name[command.rd],
			GPR_Name[command.rt], command.sa);
		break;
	case R4300i_SPECIAL_DSRL:
		sprintf(CommandName,"dsrl\t%s, %s, 0x%X",GPR_Name[command.rd], GPR_Name[command.rt],
			command.sa);
		break;
	case R4300i_SPECIAL_DSRA:
		sprintf(CommandName,"dsra\t%s, %s, 0x%X",GPR_Name[command.rd], GPR_Name[command.rt],
			command.sa);
		break;
	case R4300i_SPECIAL_DSLL32:
		sprintf(CommandName,"dsll32\t%s, %s, 0x%X",GPR_Name[command.rd],GPR_Name[command.rt], command.sa);
		break;
	case R4300i_SPECIAL_DSRL32:
		sprintf(CommandName,"dsrl32\t%s, %s, 0x%X",GPR_Name[command.rd], GPR_Name[command.rt], command.sa);
		break;
	case R4300i_SPECIAL_DSRA32:
		sprintf(CommandName,"dsra32\t%s, %s, 0x%X",GPR_Name[command.rd], GPR_Name[command.rt], command.sa);
		break;
	default:
		sprintf(CommandName,"Unknown\t%02X %02X %02X %02X",
			command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
	}
	return CommandName;
}
char * R4300iCop1Name ( DWORD OpCode, DWORD PC ) {
	OPCODE command;
	command.Hex = OpCode;
	switch (command.fmt) {
	case R4300i_COP1_MF:
		sprintf(CommandName,"mfc1\t%s, %s",GPR_Name[command.rt], FPR_Name[command.fs]);
		break;
	case R4300i_COP1_DMF:
		sprintf(CommandName,"dmfc1\t%s, %s",GPR_Name[command.rt], FPR_Name[command.fs]);
		break;
	case R4300i_COP1_CF:
		sprintf(CommandName,"cfc1\t%s, %s",GPR_Name[command.rt], FPR_Ctrl_Name[command.fs]);
		break;
	case R4300i_COP1_MT:
		sprintf(CommandName,"mtc1\t%s, %s",GPR_Name[command.rt], FPR_Name[command.fs]);
		break;
	case R4300i_COP1_DMT:
		sprintf(CommandName,"dmtc1\t%s, %s",GPR_Name[command.rt], FPR_Name[command.fs]);
		break;
	case R4300i_COP1_CT:
		sprintf(CommandName,"ctc1\t%s, %s",GPR_Name[command.rt], FPR_Ctrl_Name[command.fs]);
		break;
	case R4300i_COP1_BC:
		switch (command.ft) {
		case R4300i_COP1_BC_BCF:
			sprintf(CommandName,"BC1F\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
			break;
		case R4300i_COP1_BC_BCT:
			sprintf(CommandName,"BC1T\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
			break;
		case R4300i_COP1_BC_BCFL:
			sprintf(CommandName,"BC1FL\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
			break;
		case R4300i_COP1_BC_BCTL:
			sprintf(CommandName,"BC1TL\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
			break;
		default:
			sprintf(CommandName,"Unknown Cop1\t%02X %02X %02X %02X",
				command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
		}
		break;
	case R4300i_COP1_S:
	case R4300i_COP1_D:
	case R4300i_COP1_W:
	case R4300i_COP1_L:
		switch (command.funct) {
		case R4300i_COP1_FUNCT_ADD:
			sprintf(CommandName,"ADD.%s\t%s, %s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs],
				FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_SUB:
			sprintf(CommandName,"SUB.%s\t%s, %s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs],
				FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_MUL:
			sprintf(CommandName,"MUL.%s\t%s, %s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs],
				FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_DIV:
			sprintf(CommandName,"DIV.%s\t%s, %s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs],
				FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_SQRT:
			sprintf(CommandName,"SQRT.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_ABS:
			sprintf(CommandName,"ABS.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_MOV:
			sprintf(CommandName,"MOV.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_NEG:
			sprintf(CommandName,"NEG.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_ROUND_L:
			sprintf(CommandName,"ROUND.L.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_TRUNC_L:
			sprintf(CommandName,"TRUNC.L.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_CEIL_L:
			sprintf(CommandName,"CEIL.L.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_FLOOR_L:
			sprintf(CommandName,"FLOOR.L.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_ROUND_W:
			sprintf(CommandName,"ROUND.W.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_TRUNC_W:
			sprintf(CommandName,"TRUNC.W.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_CEIL_W:
			sprintf(CommandName,"CEIL.W.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_FLOOR_W:
			sprintf(CommandName,"FLOOR.W.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_CVT_S:
			sprintf(CommandName,"CVT.S.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_CVT_D:
			sprintf(CommandName,"CVT.D.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_CVT_W:
			sprintf(CommandName,"CVT.W.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_CVT_L:
			sprintf(CommandName,"CVT.L.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fd], FPR_Name[command.fs]);
			break;
		case R4300i_COP1_FUNCT_C_F:
			sprintf(CommandName,"C.F.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_UN:
			sprintf(CommandName,"C.UN.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_EQ:
			sprintf(CommandName,"C.EQ.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_UEQ:
			sprintf(CommandName,"C.UEQ.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_OLT:
			sprintf(CommandName,"C.OLT.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_ULT:
			sprintf(CommandName,"C.ULT.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_OLE:
			sprintf(CommandName,"C.OLE.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_ULE:
			sprintf(CommandName,"C.ULE.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_SF:
			sprintf(CommandName,"C.SF.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_NGLE:
			sprintf(CommandName,"C.NGLE.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_SEQ:
			sprintf(CommandName,"C.SEQ.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_NGL:
			sprintf(CommandName,"C.NGL.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_LT:
			sprintf(CommandName,"C.LT.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_NGE:
			sprintf(CommandName,"C.NGE.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_LE:
			sprintf(CommandName,"C.LE.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		case R4300i_COP1_FUNCT_C_NGT:
			sprintf(CommandName,"C.NGT.%s\t%s, %s",FPR_Type(command.fmt),
				FPR_Name[command.fs], FPR_Name[command.ft]);
			break;
		default:
			sprintf(CommandName,"Unknown Cop1\t%02X %02X %02X %02X",
				command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
		}
		break;
	default:
		sprintf(CommandName,"Unknown Cop1\t%02X %02X %02X %02X",
			command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
	}
	return CommandName;
}
char * R4300iOpcodeName ( DWORD OpCode, DWORD PC ) {
	OPCODE command;
	command.Hex = OpCode;
	switch (command.op) {
	case R4300i_SPECIAL:
		return R4300iSpecialName ( OpCode, PC );
		break;
	case R4300i_REGIMM:
		return R4300iRegImmName ( OpCode, PC );
		break;
	case R4300i_J:
		sprintf(CommandName,"j\t%s",LabelName((PC & 0xF0000000) + (command.target << 2)));
		break;
	case R4300i_JAL:
		sprintf(CommandName,"jal\t%s",LabelName((PC & 0xF0000000) + (command.target << 2)));
		break;
	case R4300i_BEQ:
		if (command.rs == 0 && command.rt == 0) {
			sprintf(CommandName,"b\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
		} else if (command.rs == 0 || command.rt == 0) {
			sprintf(CommandName,"beqz\t%s, %s", GPR_Name[command.rs == 0 ? command.rt : command.rs ],
				LabelName(PC + ((short)command.offset << 2) + 4));
		} else {
			sprintf(CommandName,"beq\t%s, %s, %s", GPR_Name[command.rs], GPR_Name[command.rt],
				LabelName(PC + ((short)command.offset << 2) + 4));
		}
		break;
	case R4300i_BNE:
		if ((command.rs == 0) ^ (command.rt == 0)){
			sprintf(CommandName,"bnez\t%s, %s", GPR_Name[command.rs == 0 ? command.rt : command.rs ],
				LabelName(PC + ((short)command.offset << 2) + 4));
		} else {
			sprintf(CommandName,"bne\t%s, %s, %s", GPR_Name[command.rs], GPR_Name[command.rt],
				LabelName(PC + ((short)command.offset << 2) + 4));
		}
		break;
	case R4300i_BLEZ:
		sprintf(CommandName,"blez\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_BGTZ:
		sprintf(CommandName,"bgtz\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_ADDI:
		sprintf(CommandName,"addi\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_ADDIU:
		sprintf(CommandName,"addiu\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_SLTI:
		sprintf(CommandName,"slti\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_SLTIU:
		sprintf(CommandName,"sltiu\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_ANDI:
		sprintf(CommandName,"andi\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_ORI:
		sprintf(CommandName,"ori\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_XORI:
		sprintf(CommandName,"xori\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_LUI:
		sprintf(CommandName,"lui\t%s, 0x%X",GPR_Name[command.rt], command.immediate);
		break;
	case R4300i_CP0:
		switch (command.rs) {
		case R4300i_COP0_MF:
			sprintf(CommandName,"mfc0\t%s, %s",GPR_Name[command.rt], Cop0_Name[command.rd]);
			break;
		case R4300i_COP0_MT:
			sprintf(CommandName,"mtc0\t%s, %s",GPR_Name[command.rt], Cop0_Name[command.rd]);
			break;
		default:
			if ( (command.rs & 0x10 ) != 0 ) {
				switch( command.funct ) {
				case R4300i_COP0_CO_TLBR:  sprintf(CommandName,"tlbr"); break;
				case R4300i_COP0_CO_TLBWI: sprintf(CommandName,"tlbwi"); break;
				case R4300i_COP0_CO_TLBWR: sprintf(CommandName,"tlbwr"); break;
				case R4300i_COP0_CO_TLBP:  sprintf(CommandName,"tlbp"); break;
				case R4300i_COP0_CO_ERET:  sprintf(CommandName,"eret"); break;
				default:
					sprintf(CommandName,"Unknown\t%02X %02X %02X %02X",
						command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
				}
			} else {
				sprintf(CommandName,"Unknown\t%02X %02X %02X %02X",
				command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
			}
			break;
		}
		break;
	case R4300i_CP1:
		return R4300iCop1Name ( OpCode, PC );
	case R4300i_BEQL:
		if (command.rs == command.rt) {
			sprintf(CommandName,"b\t%s", LabelName(PC + ((short)command.offset << 2) + 4));
		} else if ((command.rs == 0) ^ (command.rt == 0)){
			sprintf(CommandName,"beqzl\t%s, %s", GPR_Name[command.rs == 0 ? command.rt : command.rs ],
				LabelName(PC + ((short)command.offset << 2) + 4));
		} else {
			sprintf(CommandName,"beql\t%s, %s, %s", GPR_Name[command.rs], GPR_Name[command.rt],
				LabelName(PC + ((short)command.offset << 2) + 4));
		}
		break;
	case R4300i_BNEL:
		if ((command.rs == 0) ^ (command.rt == 0)){
			sprintf(CommandName,"bnezl\t%s, %s", GPR_Name[command.rs == 0 ? command.rt : command.rs ],
				LabelName(PC + ((short)command.offset << 2) + 4));
		} else {
			sprintf(CommandName,"bnel\t%s, %s, %s", GPR_Name[command.rs], GPR_Name[command.rt],
				LabelName(PC + ((short)command.offset << 2) + 4));
		}
		break;
	case R4300i_BLEZL:
		sprintf(CommandName,"blezl\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_BGTZL:
		sprintf(CommandName,"bgtzl\t%s, %s",GPR_Name[command.rs], LabelName(PC + ((short)command.offset << 2) + 4));
		break;
	case R4300i_DADDI:
		sprintf(CommandName,"daddi\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_DADDIU:
		sprintf(CommandName,"daddiu\t%s, %s, 0x%X",GPR_Name[command.rt], GPR_Name[command.rs],command.immediate);
		break;
	case R4300i_LDL:
		sprintf(CommandName,"ldl\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LDR:
		sprintf(CommandName,"ldr\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LB:
		sprintf(CommandName,"lb\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LH:
		sprintf(CommandName,"lh\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LWL:
		sprintf(CommandName,"lwl\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LW:
		sprintf(CommandName,"lw\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LBU:
		sprintf(CommandName,"lbu\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LHU:
		sprintf(CommandName,"lhu\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LWR:
		sprintf(CommandName,"lwr\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LWU:
		sprintf(CommandName,"lwu\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SB:
		sprintf(CommandName,"sb\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SH:
		sprintf(CommandName,"sh\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SWL:
		sprintf(CommandName,"swl\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SW:
		sprintf(CommandName,"sw\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SDL:
		sprintf(CommandName,"sdl\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SDR:
		sprintf(CommandName,"sdr\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SWR:
		sprintf(CommandName,"swr\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_CACHE:
		sprintf(CommandName,"cache\t%d, 0x%X (%s)",command.rt, command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LL:
		sprintf(CommandName,"ll\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LWC1:
		sprintf(CommandName,"lwc1\t%s, 0x%X (%s)",FPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LDC1:
		sprintf(CommandName,"ldc1\t%s, 0x%X (%s)",FPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_LD:
		sprintf(CommandName,"ld\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SC:
		sprintf(CommandName,"sc\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SWC1:
		sprintf(CommandName,"swc1\t%s, 0x%X (%s)",FPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SDC1:
		sprintf(CommandName,"sdc1\t%s, 0x%X (%s)",FPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	case R4300i_SD:
		sprintf(CommandName,"sd\t%s, 0x%X (%s)",GPR_Name[command.rt], command.offset, GPR_Name[command.base]);
		break;
	default:
		sprintf(CommandName,"Unknown\t%02X %02X %02X %02X",
			command.Ascii[3],command.Ascii[2],command.Ascii[1],command.Ascii[0]);
	}
	return CommandName;
}
int DelaySlotEffectsCompare (DWORD PC, DWORD Reg1, DWORD Reg2) {
	OPCODE Command;
	if (!r4300i_LW_VAddr(PC + 4, &Command.Hex)) {
		ExitThread(0);
		return TRUE;
	}
	if (SelfModCheck == ModCode_ChangeMemory) {
		if ( (Command.Hex >> 16) == 0x7C7C) {
			Command.Hex = OrigMem[(Command.Hex & 0xFFFF)].OriginalValue;
		}
	}
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
			if (Command.rd == 0) { return FALSE; }
			if (Command.rd == Reg1) { return TRUE; }
			if (Command.rd == Reg2) { return TRUE; }
			break;
		case R4300i_SPECIAL_MULT:
		case R4300i_SPECIAL_MULTU:
		case R4300i_SPECIAL_DIV:
		case R4300i_SPECIAL_DIVU:
		case R4300i_SPECIAL_DMULT:
		case R4300i_SPECIAL_DMULTU:
		case R4300i_SPECIAL_DDIV:
		case R4300i_SPECIAL_DDIVU:
			break;
		default:
	return TRUE;
		}
		break;
	case R4300i_CP0:
		switch (Command.rs) {
		case R4300i_COP0_MT: break;
		case R4300i_COP0_MF:
			if (Command.rt == 0) { return FALSE; }
			if (Command.rt == Reg1) { return TRUE; }
			if (Command.rt == Reg2) { return TRUE; }
			break;
		default:
			if ( (Command.rs & 0x10 ) != 0 ) {
				switch( Opcode.funct ) {
				case R4300i_COP0_CO_TLBR: break;
				case R4300i_COP0_CO_TLBWI: break;
				case R4300i_COP0_CO_TLBWR: break;
				case R4300i_COP0_CO_TLBP: break;
				default:
					return TRUE;
				}
			}
		}
		break;
	case R4300i_CP1:
		switch (Command.fmt) {
		case R4300i_COP1_MF:
			if (Command.rt == 0) { return FALSE; }
			if (Command.rt == Reg1) { return TRUE; }
			if (Command.rt == Reg2) { return TRUE; }
			break;
		case R4300i_COP1_CF: break;
		case R4300i_COP1_MT: break;
		case R4300i_COP1_CT: break;
		case R4300i_COP1_S: break;
		case R4300i_COP1_D: break;
		case R4300i_COP1_W: break;
		case R4300i_COP1_L: break;
			return TRUE;
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
		if (Command.rt == 0) { return FALSE; }
		if (Command.rt == Reg1) { return TRUE; }
		if (Command.rt == Reg2) { return TRUE; }
		break;
	case R4300i_CACHE: break;
	case R4300i_SB: break;
	case R4300i_SH: break;
	case R4300i_SW: break;
	case R4300i_SWR: break;
	case R4300i_SWL: break;
	case R4300i_SWC1: break;
	case R4300i_SDC1: break;
	case R4300i_SD: break;
	default:
		return TRUE;
	}
	return FALSE;
}
int DelaySlotEffectsJump (DWORD JumpPC) {
	OPCODE Command;
	if (!r4300i_LW_VAddr(JumpPC, &Command.Hex)) { return TRUE; }
	if (SelfModCheck == ModCode_ChangeMemory) {
		if ( (Command.Hex >> 16) == 0x7C7C) {
			Command.Hex = OrigMem[(Command.Hex & 0xFFFF)].OriginalValue;
		}
	}
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
	case R4300i_SPECIAL_JALR: return DelaySlotEffectsCompare(JumpPC,31,0); break;
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
					if (!r4300i_LW_VAddr(JumpPC + 4, &NewCommand.Hex)) { return TRUE; }
					EffectDelaySlot = FALSE;
					if (NewCommand.op == R4300i_CP1) {
						if (NewCommand.fmt == R4300i_COP1_S && (NewCommand.funct & 0x30) == 0x30 ) {
							EffectDelaySlot = TRUE;
						}
						if (NewCommand.fmt == R4300i_COP1_D && (NewCommand.funct & 0x30) == 0x30 ) {
							EffectDelaySlot = TRUE;
						}
					}
					return EffectDelaySlot;
				}
				break;
			}
			break;
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
	hEvent =  CreateEvent(NULL,FALSE,FALSE,NULL);
	MsgWaitForMultipleObjects(1,&hEvent,FALSE,1000,QS_ALLINPUT);
	CloseHandle(hEvent);
	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) {
			PostMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
			return;
		}
	}
}
void DoSomething ( void ) {
	if (CPU_Action.CloseCPU) {
		CoUninitialize();
		ExitThread(0);
	}
	if (CPU_Action.CheckInterrupts) {
		CPU_Action.CheckInterrupts = FALSE;
		CheckInterrupts();
	}
	if (CPU_Action.DoInterrupt) {
		CPU_Action.DoInterrupt = FALSE;
		DoIntrException(FALSE);
	}
	if (CPU_Action.ChangeWindow) {
		CPU_Action.ChangeWindow = FALSE;
		CPU_Paused = TRUE;
		SendMessage(hMainWindow,WM_COMMAND,ID_OPTIONS_FULLSCREEN,0);
		CPU_Paused = FALSE;
	}
	if (CPU_Action.Pause) {
		WaitForSingleObject(hPauseMutex, INFINITE);
		if (CPU_Action.Pause) {
			HMENU hMenu = GetMenu(hMainWindow);
			HMENU hSubMenu = GetSubMenu(hMenu,1);
			MenuSetText(hSubMenu, 1, GS(MENU_RESUME),"F2/Shift+Caps Lock");
			CurrentFrame = 0;
			CPU_Paused = TRUE;
			CPU_Action.Pause = FALSE;
			ReleaseMutex(hPauseMutex);
			SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MSG_CPU_PAUSED));
			DisplayFPS ();
			if (DrawScreen != NULL) { DrawScreen(); }
			WaitForSingleObject(hPauseMutex, INFINITE);
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
	CPU_Action.DoSomething = FALSE;
	if (CPU_Action.SaveState) {
		//test if allowed
		CPU_Action.SaveState = FALSE;
		if (!Machine_SaveState()) {
			CPU_Action.SaveState = TRUE;
			CPU_Action.DoSomething = TRUE;
		}
	}
	if (CPU_Action.RestoreState) {
		CPU_Action.RestoreState = FALSE;
		Machine_LoadState();
	}
	if (CPU_Action.DoInterrupt == TRUE) { CPU_Action.DoSomething = TRUE; }
}
void GetAutoSaveDir( char * Directory ) {
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath( path_buffer, drive, dir, fname, ext );
	sprintf(Directory,"%s%sSave Data\\",drive,dir);
	sprintf(Group,"N64 Software\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Value, Bytes;
		Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"AppPath Save Data",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult == ERROR_SUCCESS && Value == FALSE) {
			Bytes = sizeof(Dir);
			lResult = RegQueryValueEx(hKeyResults,"CustomPath Save Data",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult == ERROR_SUCCESS) { strcpy(Directory,Dir); }
		}
	}
	RegCloseKey(hKeyResults);
}
void GetInstantSaveDir( char * Directory ) {
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	char Dir[255], Group[200];
	long lResult;
	HKEY hKeyResults = 0;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath( path_buffer, drive, dir, fname, ext );
	sprintf(Directory,"%s%sSave States\\",drive,dir);
	sprintf(Group,"N64 Software\\%s",AppName);
	lResult = RegOpenKeyEx( HKEY_CURRENT_USER,Group,0,KEY_ALL_ACCESS,
		&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Value, Bytes;
		Bytes = 4;
		lResult = RegQueryValueEx(hKeyResults,"AppPath Save States",0,&Type,(LPBYTE)(&Value),&Bytes);
		if (lResult == ERROR_SUCCESS && Value == FALSE) {
			Bytes = sizeof(Dir);
			lResult = RegQueryValueEx(hKeyResults,"CustomPath SaveStates",0,&Type,(LPBYTE)Dir,&Bytes);
			if (lResult == ERROR_SUCCESS) { strcpy(Directory,Dir); }
		}
	}
	RegCloseKey(hKeyResults);
}
void InPermLoop (void) {
	// *** Changed ***/
	if (CPU_Action.DoInterrupt) { return; }
	/* Interrupts enabled */
	if (( STATUS_REGISTER & STATUS_IE  ) == 0 ) { goto InterruptsDisabled; }
	if (( STATUS_REGISTER & STATUS_EXL ) != 0 ) { goto InterruptsDisabled; }
	if (( STATUS_REGISTER & STATUS_ERL ) != 0 ) { goto InterruptsDisabled; }
	if (( STATUS_REGISTER & 0xFF00) == 0) { goto InterruptsDisabled; }
	/* check sound playing */
	/* check RSP running */
	/* check RDP running */
	if (Timers.Timer > 0) {
		COUNT_REGISTER += Timers.Timer + 1;
		Timers.Timer = -1;
	}
	return;
InterruptsDisabled:
	if (UpdateScreen != NULL) { UpdateScreen(); }
	CurrentFrame = 0;
	DisplayFPS();
	DisplayError(GS(MSG_PERM_LOOP));
	ExitThread(0);
}
BOOL Machine_LoadState(void) {
	char Directory[255], FileName[255], ZipFile[255], LoadHeader[64], String[100];
	char drive[_MAX_DRIVE] ,dir[_MAX_DIR], ext[_MAX_EXT];
	DWORD dwRead, Value, count, SaveRDRAMsize;
	BOOL LoadedZipFile = FALSE;
	HANDLE hSaveFile;
	unzFile file;
	if (strlen(LoadFileName) == 0) {
		GetInstantSaveDir(Directory);
		sprintf(FileName,"%s%s",Directory,CurrentSave);
	} else {
		strcpy(FileName,LoadFileName);
		strcpy(ZipFile,LoadFileName);
	}
	file = unzOpen(ZipFile);
	if (file != NULL) {
	    unz_file_info info;
		char zname[132];
		int port = 0;
		port = unzGoToFirstFile(file);
		while (port == UNZ_OK && LoadedZipFile == FALSE) {
			unzGetCurrentFileInfo(file, &info, zname, 128, NULL,0, NULL,0);
		    if (unzLocateFile(file, zname, 1) != UNZ_OK ) {
				unzClose(file);
				port = -1;
				continue;
			}
			if( unzOpenCurrentFile(file) != UNZ_OK ) {
				unzClose(file);
				port = -1;
				continue;
			}
			unzReadCurrentFile(file,&Value,4);
			if (Value != 0x23D8A6C8) {
				unzCloseCurrentFile(file);
				continue;
			}
			unzReadCurrentFile(file,&SaveRDRAMsize,sizeof(SaveRDRAMsize));
			unzReadCurrentFile(file,LoadHeader,0x40);
			if (CPU_Type != CPU_Interpreter) {
				ResetRecompCode();
			}
			Timers.CurrentTimerType = -1;
			Timers.Timer = 0;
			for (count = 0; count < MaxTimers; count ++) { Timers.Active[count] = FALSE; }
			//fix RDRAM size
			if (SaveRDRAMsize != RDRAMsize) {
				if (RDRAMsize == 0x400000) {
					if (VirtualAlloc(N64MEM + 0x400000, 0x400000, MEM_COMMIT, PAGE_READWRITE)==NULL) {
						DisplayError(GS(MSG_MEM_ALLOC_ERROR));
						ExitThread(0);
					}
					if (VirtualAlloc((BYTE *)JumpTable + 0x400000, 0x400000, MEM_COMMIT, PAGE_READWRITE)==NULL) {
						DisplayError(GS(MSG_MEM_ALLOC_ERROR));
						ExitThread(0);
					}
					if (VirtualAlloc((BYTE *)DelaySlotTable + (0x400000 >> 0xA), (0x400000 >> 0xA), MEM_COMMIT, PAGE_READWRITE)==NULL) {
						DisplayError(GS(MSG_MEM_ALLOC_ERROR));
						ExitThread(0);
					}
				} else {
					VirtualFree(N64MEM + 0x400000, 0x400000,MEM_DECOMMIT);
					VirtualFree((BYTE *)JumpTable + 0x400000, 0x400000,MEM_DECOMMIT);
					VirtualFree((BYTE *)DelaySlotTable + (0x400000 >> 0xA), (0x400000 >> 0xA),MEM_DECOMMIT);
				}
			}
			RDRAMsize = SaveRDRAMsize;
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
			LoadedZipFile = TRUE;
			_splitpath( ZipFile, drive, dir, ZipFile, ext );
			sprintf(FileName,"%s%s",ZipFile,ext);
		}
	}
	if (!LoadedZipFile) {
		hSaveFile = CreateFile(FileName,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		if (hSaveFile == INVALID_HANDLE_VALUE) {
			_splitpath( FileName, drive, dir, ZipFile, ext );
			sprintf(String,"%s %s%s",GS(MSG_UNABLED_LOAD_STATE),ZipFile,ext);
			SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)String );
			return FALSE;
		}
		SetFilePointer(hSaveFile,0,NULL,FILE_BEGIN);
		ReadFile( hSaveFile,&Value,sizeof(Value),&dwRead,NULL);
		if (Value != 0x23D8A6C8) { return FALSE; }
		ReadFile( hSaveFile,&SaveRDRAMsize,sizeof(SaveRDRAMsize),&dwRead,NULL);
		ReadFile( hSaveFile,LoadHeader,0x40,&dwRead,NULL);
		if (CPU_Type != CPU_Interpreter) {
			ResetRecompCode();
		}
		Timers.CurrentTimerType = -1;
		Timers.Timer = 0;
		for (count = 0; count < MaxTimers; count ++) { Timers.Active[count] = FALSE; }
		//fix RDRAM size
		if (SaveRDRAMsize != RDRAMsize) {
			if (RDRAMsize == 0x400000) {
				if (VirtualAlloc(N64MEM + 0x400000, 0x400000, MEM_COMMIT, PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					ExitThread(0);
				}
				if (VirtualAlloc((BYTE *)JumpTable + 0x400000, 0x400000, MEM_COMMIT, PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					ExitThread(0);
				}
				if (VirtualAlloc((BYTE *)DelaySlotTable + (0x400000 >> 0xA), (0x400000 >> 0xA), MEM_COMMIT, PAGE_READWRITE)==NULL) {
					DisplayError(GS(MSG_MEM_ALLOC_ERROR));
					ExitThread(0);
				}
			} else {
				VirtualFree(N64MEM + 0x400000, 0x400000,MEM_DECOMMIT);
				VirtualFree((BYTE *)JumpTable + 0x400000, 0x400000,MEM_DECOMMIT);
				VirtualFree((BYTE *)DelaySlotTable + (0x400000 >> 0xA), (0x400000 >> 0xA),MEM_DECOMMIT);
			}
		}
		RDRAMsize = SaveRDRAMsize;
		ReadFile( hSaveFile,&Value,sizeof(Value),&dwRead,NULL);
		ChangeTimer(ViTimer,Value);
		ReadFile( hSaveFile,&PROGRAM_COUNTER,sizeof(PROGRAM_COUNTER),&dwRead,NULL);
		ReadFile( hSaveFile,GPR,sizeof(_int64)*32,&dwRead,NULL);
		ReadFile( hSaveFile,FPR,sizeof(_int64)*32,&dwRead,NULL);
		ReadFile( hSaveFile,CP0,sizeof(DWORD)*32,&dwRead,NULL);
		ReadFile( hSaveFile,FPCR,sizeof(DWORD)*32,&dwRead,NULL);
		ReadFile( hSaveFile,&HI,sizeof(_int64),&dwRead,NULL);
		ReadFile( hSaveFile,&LO,sizeof(_int64),&dwRead,NULL);
		ReadFile( hSaveFile,RegRDRAM,sizeof(DWORD)*10,&dwRead,NULL);
		ReadFile( hSaveFile,RegSP,sizeof(DWORD)*10,&dwRead,NULL);
		ReadFile( hSaveFile,RegDPC,sizeof(DWORD)*10,&dwRead,NULL);
		ReadFile( hSaveFile,RegMI,sizeof(DWORD)*4,&dwRead,NULL);
		ReadFile( hSaveFile,RegVI,sizeof(DWORD)*14,&dwRead,NULL);
		ReadFile( hSaveFile,RegAI,sizeof(DWORD)*6,&dwRead,NULL);
		ReadFile( hSaveFile,RegPI,sizeof(DWORD)*13,&dwRead,NULL);
		ReadFile( hSaveFile,RegRI,sizeof(DWORD)*8,&dwRead,NULL);
		ReadFile( hSaveFile,RegSI,sizeof(DWORD)*4,&dwRead,NULL);
		ReadFile( hSaveFile,tlb,sizeof(TLB)*32,&dwRead,NULL);
		ReadFile( hSaveFile,PIF_Ram,0x40,&dwRead,NULL);
		ReadFile( hSaveFile,RDRAM,RDRAMsize,&dwRead,NULL);
		ReadFile( hSaveFile,DMEM,0x1000,&dwRead,NULL);
		ReadFile( hSaveFile,IMEM,0x1000,&dwRead,NULL);
		CloseHandle(hSaveFile);
		_splitpath( FileName, drive, dir, ZipFile, ext );
		sprintf(FileName,"%s%s",ZipFile,ext);
	}
	ChangeCompareTimer();
	if (AiRomClosed != NULL)   { AiRomClosed(); }
	if (RSPRomClosed) { RSPRomClosed(); }
	DlistCount = 0;
	AlistCount = 0;
	AI_STATUS_REG = 0;
	AiDacrateChanged(SYSTEM_NTSC);
	ViStatusChanged();
	ViWidthChanged();
	SetupTLB();
	//Fix up Memory stack location
	MemoryStack = GPR[29].W[0];
	TranslateVaddr(&MemoryStack);
	MemoryStack += (DWORD)N64MEM;
	CheckInterrupts();
	DMAUsed = TRUE;
	strcpy(SaveAsFileName,"");
	strcpy(LoadFileName,"");
	sprintf(String,"%s",FileName);
	SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)String );
	return TRUE;
}
BOOL Machine_SaveState(void) {
	char Directory[255], FileName[255], ZipFile[255], String[100];
	char drive[_MAX_DRIVE] ,dir[_MAX_DIR], ext[_MAX_EXT];
	DWORD dwWritten, Value;
	HANDLE hSaveFile;
	if (Timers.CurrentTimerType != CompareTimer &&  Timers.CurrentTimerType != ViTimer) {
		return FALSE;
	}
	if (strlen(SaveAsFileName) == 0) {
		GetInstantSaveDir(Directory);
		sprintf(FileName,"%s%s",Directory,CurrentSave);
	} else {
		sprintf(FileName,"%s",SaveAsFileName);
	}
	if (SelfModCheck == ModCode_ChangeMemory) { ResetRecompCode(); }
{
		hSaveFile = CreateFile(FileName,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		if (hSaveFile == INVALID_HANDLE_VALUE) {
			switch (GetLastError()) {
			case ERROR_PATH_NOT_FOUND:
				CreateDirectory(Directory,NULL);
				hSaveFile = CreateFile(FileName,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
				if (hSaveFile == INVALID_HANDLE_VALUE) {
					DisplayError(GS(MSG_FAIL_OPEN_SAVE));
					return TRUE;
				}
				break;
			default:
				DisplayError(GS(MSG_FAIL_OPEN_SAVE));
				return TRUE;
			}
		}
		while ((int)Registers.CP0[1] < (int)Registers.CP0[6]) {
			Registers.CP0[1] += 32 - Registers.CP0[6];
		}
		SetFilePointer(hSaveFile,0,NULL,FILE_BEGIN);
		Value = 0x23D8A6C8;
		WriteFile( hSaveFile,&Value,sizeof(Value),&dwWritten,NULL);
		WriteFile( hSaveFile,&RDRAMsize,sizeof(RDRAMsize),&dwWritten,NULL);
		WriteFile( hSaveFile,RomHeader,0x40,&dwWritten,NULL);
		Value = Timers.NextTimer[ViTimer] + Timers.Timer;
		WriteFile( hSaveFile,&Value,sizeof(Value),&dwWritten,NULL);
		WriteFile( hSaveFile,&PROGRAM_COUNTER,sizeof(PROGRAM_COUNTER),&dwWritten,NULL);
		WriteFile( hSaveFile,GPR,sizeof(_int64)*32,&dwWritten,NULL);
		WriteFile( hSaveFile,FPR,sizeof(_int64)*32,&dwWritten,NULL);
		WriteFile( hSaveFile,CP0,sizeof(DWORD)*32,&dwWritten,NULL);
		WriteFile( hSaveFile,FPCR,sizeof(DWORD)*32,&dwWritten,NULL);
		WriteFile( hSaveFile,&HI,sizeof(_int64),&dwWritten,NULL);
		WriteFile( hSaveFile,&LO,sizeof(_int64),&dwWritten,NULL);
		WriteFile( hSaveFile,RegRDRAM,sizeof(DWORD)*10,&dwWritten,NULL);
		WriteFile( hSaveFile,RegSP,sizeof(DWORD)*10,&dwWritten,NULL);
		WriteFile( hSaveFile,RegDPC,sizeof(DWORD)*10,&dwWritten,NULL);
		Value = MI_INTR_REG;
		if (AiReadLength() != 0) { MI_INTR_REG |= MI_INTR_AI; }
		WriteFile( hSaveFile,RegMI,sizeof(DWORD)*4,&dwWritten,NULL);
		MI_INTR_REG = Value;
		WriteFile( hSaveFile,RegVI,sizeof(DWORD)*14,&dwWritten,NULL);
		WriteFile( hSaveFile,RegAI,sizeof(DWORD)*6,&dwWritten,NULL);
		WriteFile( hSaveFile,RegPI,sizeof(DWORD)*13,&dwWritten,NULL);
		WriteFile( hSaveFile,RegRI,sizeof(DWORD)*8,&dwWritten,NULL);
		WriteFile( hSaveFile,RegSI,sizeof(DWORD)*4,&dwWritten,NULL);
		WriteFile( hSaveFile,tlb,sizeof(TLB)*32,&dwWritten,NULL);
		WriteFile( hSaveFile,PIF_Ram,0x40,&dwWritten,NULL);
		WriteFile( hSaveFile,RDRAM,RDRAMsize,&dwWritten,NULL);
		WriteFile( hSaveFile,DMEM,0x1000,&dwWritten,NULL);
		WriteFile( hSaveFile,IMEM,0x1000,&dwWritten,NULL);
		CloseHandle(hSaveFile);
		DeleteFile(ZipFile);
		_splitpath( FileName, drive, dir, ZipFile, ext );
		sprintf(FileName,"%s%s",ZipFile,ext);
	}
	strcpy(SaveAsFileName,"");
	strcpy(LoadFileName,"");
	sprintf(String,"%s",FileName);
	SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)String );
	return TRUE;
}
void PauseCpu (void) {
	DWORD Result;
	if (!CPURunning) { return; }
	do {
		Result = MsgWaitForMultipleObjects(1,&hPauseMutex,FALSE,INFINITE,QS_ALLINPUT);
		if (Result != WAIT_OBJECT_0) {
			MSG msg;
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE) != 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	} while (Result != WAIT_OBJECT_0);
	if (CPU_Paused || CPU_Action.Pause) {
		HMENU hMenu = GetMenu(hMainWindow);
		HMENU hSubMenu = GetSubMenu(hMenu,1);
		if (CPU_Action.Pause) {
			CPU_Action.Pause = FALSE;
			CPU_Paused = FALSE;
			ManualPaused = FALSE;
			SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MSG_CPU_RESUMED) );
			ReleaseMutex(hPauseMutex);
			return;
		}
		ResumeThread(hCPU);
		SendMessage( hStatusWnd, SB_SETTEXT, 0, (LPARAM)GS(MSG_CPU_RESUMED));
		MenuSetText(hSubMenu, 1, GS(MENU_PAUSE),"F2/Shift+Caps Lock");
		ManualPaused = FALSE;
		CPU_Paused = FALSE;
	} else {
		CPU_Action.Pause = TRUE;
		CPU_Action.DoSomething = TRUE;
	}
	ReleaseMutex(hPauseMutex);
}
void RefreshScreen (void ){
	static DWORD OLD_VI_V_SYNC_REG = 0, VI_INTR_TIME = 500000;
	LARGE_INTEGER Time;
	if (OLD_VI_V_SYNC_REG != VI_V_SYNC_REG) {
		if (VI_V_SYNC_REG == 0) {
			VI_INTR_TIME = 500000;
		} else {
			VI_INTR_TIME = (VI_V_SYNC_REG + 1) * 1500;
			if ((VI_V_SYNC_REG % 1) != 0) {
				VI_INTR_TIME -= 38;
			}
		}
	}
	ChangeTimer(ViTimer,Timers.Timer + Timers.NextTimer[ViTimer] + VI_INTR_TIME);
	UpdateFieldSerration((VI_STATUS_REG & 0x40) != 0);
	if (LimitFPS) {	Timer_Process(NULL); }
	if ((CurrentFrame & 7) == 0) {
		QueryPerformanceCounter(&Time);
		Frames[(CurrentFrame >> 3) % 8].QuadPart = Time.QuadPart - LastFrame.QuadPart;
		LastFrame.QuadPart = Time.QuadPart;
		DisplayFPS();
	}
        CurrentFrame += 1;
	__try {
		if (UpdateScreen != NULL) { UpdateScreen(); }
	}
	__except (r4300i_CPU_MemoryFilter(GetExceptionCode(), GetExceptionInformation())) { ExitThread(0); }
	if ((STATUS_REGISTER & STATUS_IE) != 0 ) { ApplyCheats(); }
}
void RunRsp (void) {
	if ( ( SP_STATUS_REG & SP_STATUS_HALT ) == 0) {
		if ( ( SP_STATUS_REG & SP_STATUS_BROKE ) == 0 ) {
			DWORD Task = *( DWORD *)(DMEM + 0xFC0);
			if (Task == 1 && (DPC_STATUS_REG & DPC_STATUS_FREEZE) != 0)
			{
				return;
			}
			switch (Task) {
			case 1:
				DlistCount += 1;
				break;
			case 2:
				AlistCount += 1;
				break;
			}
				DoRspCycles(100);
			} else {
				DoRspCycles(100);
			}
		}
	}
void SetCoreToRunning ( void ) {
	CPU_Action.Stepping = FALSE;
	PulseEvent( CPU_Action.hStepping );
}
void SetCoreToStepping ( void ) {
	CPU_Action.Stepping = TRUE;
}
void CheckRbRefresh ( void ) {
	if (RomBrowser) { ShowRomList(hMainWindow); RefreshRomBrowser();
	} else SetupMenu(hMainWindow);
}
void StepOpcode ( void ) {
	PulseEvent( CPU_Action.hStepping );
}
void TimerDone (void) {
	switch (Timers.CurrentTimerType) {
	case CompareTimer:
		FAKE_CAUSE_REGISTER |= CAUSE_IP7;
		CheckInterrupts();
		ChangeCompareTimer();
		break;
	case SiTimer:
		ChangeTimer(SiTimer,0);
		MI_INTR_REG |= MI_INTR_SI;
		SI_STATUS_REG |= SI_STATUS_INTERRUPT;
		CheckInterrupts();
		break;
	case PiTimer:
		ChangeTimer(PiTimer,0);
		PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
		MI_INTR_REG |= MI_INTR_PI;
		CheckInterrupts();
		break;
	case ViTimer:
		RefreshScreen();
		MI_INTR_REG |= MI_INTR_VI;
		CheckInterrupts();
		break;
	case RspTimer:
		ChangeTimer(RspTimer,0);
		RunRsp();
		CheckInterrupts();
		break;
	}
	CheckTimer();
}