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
 * This software is provided 'as-is',without any express or implied
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
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
#include "x86.h"
#include "Plugin.h"
#include "SummerCart.h"
DWORD *TLB_ReadMap,*TLB_WriteMap,RDRAMsize,SystemRDRAMsize;
BYTE *N64MEM,*RDRAM,*DMEM,*IMEM,*ROM;
void ** JumpTable,** DelaySlotTable;
BYTE *RecompCode,*RecompPos;
BOOL WrittenToRom;
DWORD WroteToRom;
DWORD TempValue;
int Allocate_ROM (void) {
	if (ROM != NULL) { 	VirtualFree(ROM,0 ,MEM_RELEASE); }
	ROM = (BYTE *)VirtualAlloc(NULL,RomFileSize,MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,PAGE_READWRITE);
	WrittenToRom = FALSE;
	return ROM == NULL?FALSE:TRUE;
}
int Allocate_Memory (void) {
	RDRAMsize = 0x400000;
	N64MEM = (unsigned char *) VirtualAlloc(NULL,0x20000000,MEM_RESERVE | MEM_TOP_DOWN,PAGE_READWRITE);
	if(N64MEM==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	(BYTE *)JumpTable = (BYTE *) VirtualAlloc(NULL,0x10000000,MEM_RESERVE | MEM_TOP_DOWN,PAGE_READWRITE);
	if(JumpTable == NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	/* Recomp code */
	RecompCode=(BYTE *) VirtualAlloc(NULL,LargeCompileBufferSize + 4,MEM_RESERVE|MEM_TOP_DOWN,PAGE_EXECUTE_READWRITE);
	if(RecompCode==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	/* Memory */
	TLB_ReadMap = (DWORD *)VirtualAlloc(NULL,0xFFFFF * sizeof(DWORD),MEM_RESERVE|MEM_COMMIT| MEM_TOP_DOWN,PAGE_READWRITE);
	if (TLB_ReadMap == NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	TLB_WriteMap = (DWORD *)VirtualAlloc(NULL,0xFFFFF * sizeof(DWORD),MEM_RESERVE|MEM_COMMIT| MEM_TOP_DOWN,PAGE_READWRITE);
	if (TLB_WriteMap == NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	/* Delay Slot Table */
	(BYTE *)DelaySlotTable = (BYTE *) VirtualAlloc(NULL,(0x20000000 >> 0xA),MEM_RESERVE | MEM_TOP_DOWN,PAGE_READWRITE);
	if(DelaySlotTable == NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	if(VirtualAlloc(DelaySlotTable,(0x400000 >> 0xA),MEM_COMMIT,PAGE_READWRITE)==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	if(VirtualAlloc((BYTE *)DelaySlotTable + (0x04000000 >> 0xA),(0x2000 >> 0xA),MEM_COMMIT,PAGE_READWRITE)==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	if(VirtualAlloc(N64MEM,0x00400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	if(VirtualAlloc(N64MEM + 0x04000000,0x2000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	RDRAM = (unsigned char *)(N64MEM);
	DMEM  = (unsigned char *)(N64MEM+0x04000000);
	IMEM  = (unsigned char *)(N64MEM+0x04001000);
	ROM   = NULL;
	/* Jump Table */
	if (VirtualAlloc(JumpTable,0x00400000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	if (VirtualAlloc((BYTE *)JumpTable + 0x04000000,0x2000,MEM_COMMIT,PAGE_READWRITE)==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		return FALSE;
	}
	return TRUE;
}
void Compile_LB (int Reg,DWORD Addr,BOOL SignExtend) {
	if (!TranslateVaddr(&Addr)) goto Move;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
	case 0x10000000:
		if (SignExtend) {
			MoveSxVariableToX86regByte(Addr + N64MEM,Reg);
		} else {
			MoveZxVariableToX86regByte(Addr + N64MEM,Reg);
		}
		return;
	}
	Move:
	MoveConstToX86reg(0,Reg);
}
void Compile_LH (int Reg,DWORD Addr,BOOL SignExtend) {
	if (!TranslateVaddr(&Addr)) goto Move;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
	case 0x10000000:
		if (SignExtend) {
			MoveSxVariableToX86regHalf(Addr + N64MEM,Reg);
		} else {
			MoveZxVariableToX86regHalf(Addr + N64MEM,Reg);
		}
		return;
	}
	Move:
	MoveConstToX86reg(0,Reg);
}
void Compile_LW (int Reg,DWORD Addr) {
	if (!TranslateVaddr(&Addr)) {
		MoveConstToX86reg(0,Reg);
		return; // Original 1.6 did not have this,assuming it's a mistake
	}
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
	case 0x06000000: // N64DD IPL (J)
	case 0x10000000:
		MoveVariableToX86reg(Addr + N64MEM,Reg);
		break;
	case 0x04000000:
		if (Addr < 0x04002000) {
			MoveVariableToX86reg(Addr + N64MEM,Reg);
			break;
		}
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x04040000: MoveVariableToX86reg(&SP_MEM_ADDR_REG,Reg); break;
		case 0x04040004: MoveVariableToX86reg(&SP_DRAM_ADDR_REG,Reg); break;
		case 0x04040008: MoveVariableToX86reg(&SP_RD_LEN_REG,Reg); break;
		case 0x0404000C: MoveVariableToX86reg(&SP_WR_LEN_REG,Reg); break;
#endif
		case 0x04040010: MoveVariableToX86reg(&SP_STATUS_REG,Reg); break;
		case 0x04040014: MoveVariableToX86reg(&SP_DMA_FULL_REG,Reg); break;
		case 0x04040018: MoveVariableToX86reg(&SP_DMA_BUSY_REG,Reg); break;
#ifndef MIN_SIZE
		case 0x0404001C: MoveVariableToX86reg(&SP_SEMAPHORE_REG,Reg); break;
#endif
		case 0x04080000: MoveVariableToX86reg(&SP_PC_REG,Reg); break;
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
	case 0x04100000:
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x0410000C: MoveVariableToX86reg(&DPC_STATUS_REG,Reg); break;
		case 0x04100010: MoveVariableToX86reg(&DPC_CLOCK_REG,Reg); break;
		case 0x04100014: MoveVariableToX86reg(&DPC_BUFBUSY_REG,Reg); break;
		case 0x04100018: MoveVariableToX86reg(&DPC_PIPEBUSY_REG,Reg); break;
		case 0x0410001C: MoveVariableToX86reg(&DPC_TMEM_REG,Reg); break;
#endif
		default:
			MoveVariableToX86reg(Addr + N64MEM,Reg);
		}
		break;
	case 0x04300000:
		switch (Addr) {
		case 0x04300000: MoveVariableToX86reg(&MI_MODE_REG,Reg); break;
		case 0x04300004: MoveVariableToX86reg(&MI_VERSION_REG,Reg); break;
		case 0x04300008: MoveVariableToX86reg(&MI_INTR_REG,Reg); break;
		case 0x0430000C: MoveVariableToX86reg(&MI_INTR_MASK_REG,Reg); break;
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
	case 0x04400000:
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x04400000: MoveVariableToX86reg(&VI_STATUS_REG,Reg); break;
		case 0x04400004: MoveVariableToX86reg(&VI_ORIGIN_REG,Reg); break;
		case 0x04400008: MoveVariableToX86reg(&VI_WIDTH_REG,Reg); break;
		case 0x0440000C: MoveVariableToX86reg(&VI_INTR_REG,Reg); break;
#endif
		case 0x04400010:
			Pushad();
			Call_Direct(&UpdateCurrentHalfLine);
			Popad();
			MoveVariableToX86reg(&HalfLine,Reg);
			break;
#ifndef MIN_SIZE
		case 0x04400014: MoveVariableToX86reg(&VI_BURST_REG,Reg); break;
		case 0x04400018: MoveVariableToX86reg(&VI_V_SYNC_REG,Reg); break;
		case 0x0440001C: MoveVariableToX86reg(&VI_H_SYNC_REG,Reg); break;
		case 0x04400020: MoveVariableToX86reg(&VI_LEAP_REG,Reg); break;
		case 0x04400024: MoveVariableToX86reg(&VI_H_START_REG,Reg); break;
		case 0x04400028: MoveVariableToX86reg(&VI_V_START_REG,Reg); break;
		case 0x0440002C: MoveVariableToX86reg(&VI_V_BURST_REG,Reg); break;
		case 0x04400030: MoveVariableToX86reg(&VI_X_SCALE_REG,Reg); break;
		case 0x04400034: MoveVariableToX86reg(&VI_Y_SCALE_REG,Reg); break;
#endif
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
	case 0x04500000: /* AI registers */
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x04500000: MoveVariableToX86reg(&AI_DRAM_ADDR_REG,Reg); break;
#endif
		case 0x04500004:
			if (AiReadLength != NULL) {
				Pushad();
				Call_Direct(AiReadLength);
				MoveX86regToVariable(x86_EAX,&TempValue);
				Popad();
				MoveVariableToX86reg(&TempValue,Reg);
			} else {
				MoveConstToX86reg(0,Reg);
			}
			break;
		case 0x0450000C: MoveVariableToX86reg(&AI_STATUS_REG,Reg); break;
#ifndef MIN_SIZE
		case 0x04500014: MoveVariableToX86reg(&AI_BITRATE_REG,Reg); break;
#endif
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
	case 0x04600000:
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x04600000: MoveVariableToX86reg(&PI_DRAM_ADDR_REG,Reg); break;
		case 0x04600004: MoveVariableToX86reg(&PI_CART_ADDR_REG,Reg); break;
		case 0x04600008: MoveVariableToX86reg(&PI_RD_LEN_REG,Reg); break;
		case 0x0460000C: MoveVariableToX86reg(&PI_WR_LEN_REG,Reg); break;
#endif
		case 0x04600010: MoveVariableToX86reg(&PI_STATUS_REG,Reg); break;
		case 0x04600014: MoveVariableToX86reg(&PI_DOMAIN1_REG,Reg); break;
		case 0x04600018: MoveVariableToX86reg(&PI_BSD_DOM1_PWD_REG,Reg); break;
		case 0x0460001C: MoveVariableToX86reg(&PI_BSD_DOM1_PGS_REG,Reg); break;
		case 0x04600020: MoveVariableToX86reg(&PI_BSD_DOM1_RLS_REG,Reg); break;
		case 0x04600024: MoveVariableToX86reg(&PI_DOMAIN2_REG,Reg); break;
		case 0x04600028: MoveVariableToX86reg(&PI_BSD_DOM2_PWD_REG,Reg); break;
		case 0x0460002C: MoveVariableToX86reg(&PI_BSD_DOM2_PGS_REG,Reg); break;
		case 0x04600030: MoveVariableToX86reg(&PI_BSD_DOM2_RLS_REG,Reg); break;
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
	case 0x04700000:
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x04700000: MoveVariableToX86reg(&RI_MODE_REG,Reg); break;
		case 0x04700004: MoveVariableToX86reg(&RI_CONFIG_REG,Reg); break;
		case 0x04700008: MoveVariableToX86reg(&RI_CURRENT_LOAD_REG,Reg); break;
#endif
		case 0x0470000C: MoveVariableToX86reg(&RI_SELECT_REG,Reg); break;
		case 0x04700010: MoveVariableToX86reg(&RI_REFRESH_REG,Reg); break;
#ifndef MIN_SIZE
		case 0x04700014: MoveVariableToX86reg(&RI_LATENCY_REG,Reg); break;
		case 0x04700018: MoveVariableToX86reg(&RI_RERROR_REG,Reg); break;
		case 0x0470001C: MoveVariableToX86reg(&RI_WERROR_REG,Reg); break;
#endif
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
	case 0x04800000:
		switch (Addr) {
#ifndef MIN_SIZE
		case 0x04800000: MoveVariableToX86reg(&SI_DRAM_ADDR_REG,Reg); break;
		case 0x04800004: MoveVariableToX86reg(&SI_PIF_ADDR_RD64B_REG,Reg); break;
		case 0x04800010: MoveVariableToX86reg(&SI_PIF_ADDR_WR64B_REG,Reg); break;
#endif
		case 0x04800018: MoveVariableToX86reg(&SI_STATUS_REG,Reg); break;
		default:
			MoveConstToX86reg(0,Reg);
		}
		break;
#ifndef MIN_SIZE
	case 0x05000000:
		MoveConstToX86reg(0,Reg);
		break;
#endif
	case 0x1FC00000:
		MoveVariableToX86reg(Addr + N64MEM,Reg);
		break;
	default:
		MoveConstToX86reg(((Addr & 0xFFFF) << 16) | (Addr & 0xFFFF),Reg);
	}
}
void Compile_SB_Const (BYTE Value,DWORD Addr) {
	if (!TranslateVaddr(&Addr)) return;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		MoveConstByteToVariable(Value,Addr + N64MEM);
	}
}
void Compile_SB_Register (int x86Reg,DWORD Addr) {
	if (!TranslateVaddr(&Addr)) return;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		MoveX86regByteToVariable(x86Reg,Addr + N64MEM);
	}
}
void Compile_SH_Const (WORD Value,DWORD Addr) {
	if (!TranslateVaddr(&Addr)) return;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		MoveConstHalfToVariable(Value,Addr + N64MEM);
	}
}
void Compile_SH_Register (int x86Reg,DWORD Addr) {
	if (!TranslateVaddr(&Addr)) return;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		MoveX86regHalfToVariable(x86Reg,Addr + N64MEM);
	}
}
void Compile_SW_Const (DWORD Value,DWORD Addr) {
	BYTE * Jump;
	if (!TranslateVaddr(&Addr)) return;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		MoveConstToVariable(Value,Addr + N64MEM);
		break;
	case 0x03F00000:
		switch (Addr) {
		case 0x03F00000: MoveConstToVariable(Value,&RDRAM_CONFIG_REG); break;
		case 0x03F00004: MoveConstToVariable(Value,&RDRAM_DEVICE_ID_REG); break;
		case 0x03F00008: MoveConstToVariable(Value,&RDRAM_DELAY_REG); break;
		case 0x03F0000C: MoveConstToVariable(Value,&RDRAM_MODE_REG); break;
		case 0x03F00010: MoveConstToVariable(Value,&RDRAM_REF_INTERVAL_REG); break;
		case 0x03F00014: MoveConstToVariable(Value,&RDRAM_REF_ROW_REG); break;
		case 0x03F00018: MoveConstToVariable(Value,&RDRAM_RAS_INTERVAL_REG); break;
		case 0x03F0001C: MoveConstToVariable(Value,&RDRAM_MIN_INTERVAL_REG); break;
		case 0x03F00020: MoveConstToVariable(Value,&RDRAM_ADDR_SELECT_REG); break;
		case 0x03F00024: MoveConstToVariable(Value,&RDRAM_DEVICE_MANUF_REG);
		}
		break;
	case 0x04000000:
		if (Addr < 0x04002000) {
			MoveConstToVariable(Value,Addr + N64MEM);
			break;
		}
		switch (Addr) {
		case 0x04040000: MoveConstToVariable(Value,&SP_MEM_ADDR_REG); break;
		case 0x04040004: MoveConstToVariable(Value,&SP_DRAM_ADDR_REG); break;
		case 0x04040008:
			MoveConstToVariable(Value,&SP_RD_LEN_REG);
			Pushad();
			Call_Direct(&SP_DMA_READ);
			Popad();
			break;
#ifndef MIN_SIZE
		case 0x0404000C:
			MoveConstToVariable(Value,&SP_WR_LEN_REG);
			Pushad();
			Call_Direct(&SP_DMA_WRITE);
			Popad();
			break;
#endif
		case 0x04040010:
			{
				DWORD ModValue;
				ModValue = 0;
				if ((Value & SP_CLR_HALT) != 0) { ModValue |= SP_STATUS_HALT; }
				if ((Value & SP_CLR_BROKE) != 0) { ModValue |= SP_STATUS_BROKE; }
				if ((Value & SP_CLR_SSTEP) != 0) { ModValue |= SP_STATUS_SSTEP; }
				if ((Value & SP_CLR_INTR_BREAK) != 0) { ModValue |= SP_STATUS_INTR_BREAK; }
				if ((Value & SP_CLR_SIG0) != 0) { ModValue |= SP_STATUS_SIG0; }
				if ((Value & SP_CLR_SIG1) != 0) { ModValue |= SP_STATUS_SIG1; }
				if ((Value & SP_CLR_SIG2) != 0) { ModValue |= SP_STATUS_SIG2; }
				if ((Value & SP_CLR_SIG3) != 0) { ModValue |= SP_STATUS_SIG3; }
				if ((Value & SP_CLR_SIG4) != 0) { ModValue |= SP_STATUS_SIG4; }
				if ((Value & SP_CLR_SIG5) != 0) { ModValue |= SP_STATUS_SIG5; }
				if ((Value & SP_CLR_SIG6) != 0) { ModValue |= SP_STATUS_SIG6; }
				if ((Value & SP_CLR_SIG7) != 0) { ModValue |= SP_STATUS_SIG7; }
				if (ModValue != 0) AndConstToVariable(~ModValue,&SP_STATUS_REG);
				ModValue = 0;
				if ((Value & SP_SET_HALT) != 0) { ModValue |= SP_STATUS_HALT; }
				if ((Value & SP_SET_SSTEP) != 0) { ModValue |= SP_STATUS_SSTEP; }
				if ((Value & SP_SET_INTR_BREAK) != 0) { ModValue |= SP_STATUS_INTR_BREAK;  }
				if ((Value & SP_SET_SIG0) != 0) { ModValue |= SP_STATUS_SIG0; }
				if ((Value & SP_SET_SIG1) != 0) { ModValue |= SP_STATUS_SIG1; }
				if ((Value & SP_SET_SIG2) != 0) { ModValue |= SP_STATUS_SIG2; }
				if ((Value & SP_SET_SIG3) != 0) { ModValue |= SP_STATUS_SIG3; }
				if ((Value & SP_SET_SIG4) != 0) { ModValue |= SP_STATUS_SIG4; }
				if ((Value & SP_SET_SIG5) != 0) { ModValue |= SP_STATUS_SIG5; }
				if ((Value & SP_SET_SIG6) != 0) { ModValue |= SP_STATUS_SIG6; }
				if ((Value & SP_SET_SIG7) != 0) { ModValue |= SP_STATUS_SIG7; }
				if (ModValue != 0) OrConstToVariable(ModValue,&SP_STATUS_REG);
				if ((Value & SP_SET_SIG0) != 0 && AudioSignal)
				{
					OrConstToVariable(MI_INTR_SP,&MI_INTR_REG);
					Pushad();
					Call_Direct(CheckInterrupts);
					Popad();
				}
				if ((Value & SP_CLR_INTR) != 0) {
					AndConstToVariable(~MI_INTR_SP,&MI_INTR_REG);
					Pushad();
					Call_Direct(RunRsp);
					Call_Direct(CheckInterrupts);
					Popad();
				} else {
					Pushad();
					Call_Direct(RunRsp);
					Popad();
				}
			}
			break;
		case 0x0404001C: MoveConstToVariable(0,&SP_SEMAPHORE_REG); break;
		case 0x04080000: MoveConstToVariable(Value & 0xFFC,&SP_PC_REG);
		}
		break;
	case 0x04300000:
		switch (Addr) {
		case 0x04300000:
			{
				DWORD ModValue;
				ModValue = 0x7F;
				if ((Value & MI_CLR_INIT) != 0) { ModValue |= MI_MODE_INIT; }
				if ((Value & MI_CLR_EBUS) != 0) { ModValue |= MI_MODE_EBUS; }
				if ((Value & MI_CLR_RDRAM) != 0) { ModValue |= MI_MODE_RDRAM; }
				if (ModValue != 0) AndConstToVariable(~ModValue,&MI_MODE_REG);
				ModValue = (Value & 0x7F);
				if ((Value & MI_SET_INIT) != 0) { ModValue |= MI_MODE_INIT; }
				if ((Value & MI_SET_EBUS) != 0) { ModValue |= MI_MODE_EBUS; }
				if ((Value & MI_SET_RDRAM) != 0) { ModValue |= MI_MODE_RDRAM; }
				if (ModValue != 0) OrConstToVariable(ModValue,&MI_MODE_REG);
				if ((Value & MI_CLR_DP_INTR) != 0) AndConstToVariable(~MI_INTR_DP,&MI_INTR_REG);
			}
			break;
		case 0x0430000C:
			{
				DWORD ModValue;
				ModValue = 0;
				if ((Value & MI_INTR_MASK_CLR_SP) != 0) { ModValue |= MI_INTR_MASK_SP; }
				if ((Value & MI_INTR_MASK_CLR_SI) != 0) { ModValue |= MI_INTR_MASK_SI; }
				if ((Value & MI_INTR_MASK_CLR_AI) != 0) { ModValue |= MI_INTR_MASK_AI; }
				if ((Value & MI_INTR_MASK_CLR_VI) != 0) { ModValue |= MI_INTR_MASK_VI; }
				if ((Value & MI_INTR_MASK_CLR_PI) != 0) { ModValue |= MI_INTR_MASK_PI; }
				if ((Value & MI_INTR_MASK_CLR_DP) != 0) { ModValue |= MI_INTR_MASK_DP; }
				if (ModValue != 0) AndConstToVariable(~ModValue,&MI_INTR_MASK_REG);
				ModValue = 0;
				if ((Value & MI_INTR_MASK_SET_SP) != 0) { ModValue |= MI_INTR_MASK_SP; }
				if ((Value & MI_INTR_MASK_SET_SI) != 0) { ModValue |= MI_INTR_MASK_SI; }
				if ((Value & MI_INTR_MASK_SET_AI) != 0) { ModValue |= MI_INTR_MASK_AI; }
				if ((Value & MI_INTR_MASK_SET_VI) != 0) { ModValue |= MI_INTR_MASK_VI; }
				if ((Value & MI_INTR_MASK_SET_PI) != 0) { ModValue |= MI_INTR_MASK_PI; }
				if ((Value & MI_INTR_MASK_SET_DP) != 0) { ModValue |= MI_INTR_MASK_DP; }
				if (ModValue != 0) OrConstToVariable(ModValue,&MI_INTR_MASK_REG);
			}
		}
		break;
	case 0x04400000:
		switch (Addr) {
		case 0x04400000:
			if (ViStatusChanged != NULL) {
				CompConstToVariable(Value,&VI_STATUS_REG);
				JeLabel8(0);
				Jump = RecompPos - 1;
				MoveConstToVariable(Value,&VI_STATUS_REG);
				Pushad();
				Call_Direct(ViStatusChanged);
				Popad();
				*((BYTE *)(Jump))=(BYTE)(RecompPos - Jump - 1);
			}
			break;
		case 0x04400004: MoveConstToVariable((Value & 0xFFFFFF),&VI_ORIGIN_REG); break;
		case 0x04400008:
			if (ViWidthChanged != NULL) {
				CompConstToVariable(Value,&VI_WIDTH_REG);
				JeLabel8(0);
				Jump = RecompPos - 1;
				MoveConstToVariable(Value,&VI_WIDTH_REG);
				Pushad();
				Call_Direct(ViWidthChanged);
				Popad();
				*((BYTE *)(Jump))=(BYTE)(RecompPos - Jump - 1);
			}
			break;
		case 0x0440000C: MoveConstToVariable(Value,&VI_INTR_REG); break;
		case 0x04400010:
			AndConstToVariable(~MI_INTR_VI,&MI_INTR_REG);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
			break;
		case 0x04400014: MoveConstToVariable(Value,&VI_BURST_REG); break;
		case 0x04400018: MoveConstToVariable(Value,&VI_V_SYNC_REG); break;
		case 0x0440001C: MoveConstToVariable(Value,&VI_H_SYNC_REG); break;
		case 0x04400020: MoveConstToVariable(Value,&VI_LEAP_REG); break;
		case 0x04400024: MoveConstToVariable(Value,&VI_H_START_REG); break;
		case 0x04400028: MoveConstToVariable(Value,&VI_V_START_REG); break;
		case 0x0440002C: MoveConstToVariable(Value,&VI_V_BURST_REG); break;
		case 0x04400030: MoveConstToVariable(Value,&VI_X_SCALE_REG); break;
		case 0x04400034: MoveConstToVariable(Value,&VI_Y_SCALE_REG);
		}
		break;
	case 0x04500000: /* AI registers */
		switch (Addr) {
		case 0x04500000: MoveConstToVariable(Value,&AI_DRAM_ADDR_REG); break;
		case 0x04500004:
			MoveConstToVariable(Value,&AI_LEN_REG);
			Pushad();
			Call_Direct(AiLenChanged);
			Popad();
			break;
		case 0x04500008: MoveConstToVariable((Value & 1),&AI_CONTROL_REG); break;
		case 0x0450000C:
			/* Clear Interrupt */;
			AndConstToVariable(~MI_INTR_AI,&MI_INTR_REG);
			AndConstToVariable(~MI_INTR_AI,&AudioIntrReg);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
			break;
		case 0x04500014: MoveConstToVariable(Value,&AI_BITRATE_REG); break;
		default:
			MoveConstToVariable(Value,Addr + N64MEM);
		}
		break;
	case 0x04600000:
		switch (Addr) {
		case 0x04600000: MoveConstToVariable(Value,&PI_DRAM_ADDR_REG); break;
		case 0x04600004: MoveConstToVariable(Value,&PI_CART_ADDR_REG); break;
		case 0x04600008:
			MoveConstToVariable(Value,&PI_RD_LEN_REG);
			Pushad();
			Call_Direct(&PI_DMA_READ);
			Popad();
			break;
		case 0x0460000C:
			MoveConstToVariable(Value,&PI_WR_LEN_REG);
			Pushad();
			Call_Direct(&PI_DMA_WRITE);
			Popad();
			break;
		case 0x04600010:
			if ((Value & PI_CLR_INTR) != 0) {
				AndConstToVariable(~MI_INTR_PI,&MI_INTR_REG);
				Pushad();
				Call_Direct(CheckInterrupts);
				Popad();
			}
			break;
		case 0x04600014: MoveConstToVariable((Value & 0xFF),&PI_DOMAIN1_REG); break;
		case 0x04600018: MoveConstToVariable((Value & 0xFF),&PI_BSD_DOM1_PWD_REG); break;
		case 0x0460001C: MoveConstToVariable((Value & 0xFF),&PI_BSD_DOM1_PGS_REG); break;
		case 0x04600020: MoveConstToVariable((Value & 0xFF),&PI_BSD_DOM1_RLS_REG); break;
#ifndef MIN_SIZE
		case 0x04600024: MoveConstToVariable((Value & 0xFF),&PI_DOMAIN2_REG); break;
		case 0x04600028: MoveConstToVariable((Value & 0xFF),&PI_BSD_DOM2_PWD_REG); break;
		case 0x0460002C: MoveConstToVariable((Value & 0xFF),&PI_BSD_DOM2_PGS_REG); break;
		case 0x04600030: MoveConstToVariable((Value & 0xFF),&PI_BSD_DOM2_RLS_REG);
#endif
		}
		break;
	case 0x04700000:
		switch (Addr) {
		case 0x04700000: MoveConstToVariable(Value,&RI_MODE_REG); break;
		case 0x04700004: MoveConstToVariable(Value,&RI_CONFIG_REG); break;
		case 0x04700008: MoveConstToVariable(Value,&RI_CURRENT_LOAD_REG); break;
		case 0x0470000C: MoveConstToVariable(Value,&RI_SELECT_REG); break;
#ifndef MIN_SIZE
		case 0x04700010: MoveConstToVariable(Value,&RI_REFRESH_REG); break;
		case 0x04700014: MoveConstToVariable(Value,&RI_LATENCY_REG); break;
		case 0x04700018: MoveConstToVariable(Value,&RI_RERROR_REG); break;
		case 0x0470001C: MoveConstToVariable(Value,&RI_WERROR_REG);
#endif
		}
		break;
	case 0x04800000:
		switch (Addr) {
		case 0x04800000: MoveConstToVariable(Value,&SI_DRAM_ADDR_REG); break;
		case 0x04800004:
			MoveConstToVariable(Value,&SI_PIF_ADDR_RD64B_REG);
			Pushad();
			Call_Direct(&SI_DMA_READ);
			Popad();
			break;
		case 0x04800010:
			MoveConstToVariable(Value,&SI_PIF_ADDR_WR64B_REG);
			Pushad();
			Call_Direct(&SI_DMA_WRITE);
			Popad();
			break;
		case 0x04800018:
			AndConstToVariable(~MI_INTR_SI,&MI_INTR_REG);
			AndConstToVariable(~SI_STATUS_INTERRUPT,&SI_STATUS_REG);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
		}
		break;
	}
}
void Compile_SW_Register (int x86Reg,DWORD Addr) {
	BYTE * Jump;
	if (!TranslateVaddr(&Addr)) return;
	switch (Addr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		MoveX86regToVariable(x86Reg,Addr + N64MEM);
		break;
	case 0x04000000:
		switch (Addr) {
		case 0x04040000: MoveX86regToVariable(x86Reg,&SP_MEM_ADDR_REG); break;
		case 0x04040004: MoveX86regToVariable(x86Reg,&SP_DRAM_ADDR_REG); break;
		case 0x04040008:
			MoveX86regToVariable(x86Reg,&SP_RD_LEN_REG);
			Pushad();
			Call_Direct(&SP_DMA_READ);
			Popad();
			break;
		case 0x0404000C:
			MoveX86regToVariable(x86Reg,&SP_WR_LEN_REG);
			Pushad();
			Call_Direct(&SP_DMA_WRITE);
			Popad();
			break;
		case 0x04040010:
			MoveX86regToVariable(x86Reg,&RegModValue);
			Pushad();
			Call_Direct(ChangeSpStatus);
			Popad();
			break;
		case 0x0404001C: MoveConstToVariable(0,&SP_SEMAPHORE_REG); break;
		case 0x04080000:
			MoveX86regToVariable(x86Reg,&SP_PC_REG);
			AndConstToVariable(0xFFC,&SP_PC_REG);
			break;
		default:
			if (Addr < 0x04002000) MoveX86regToVariable(x86Reg,Addr + N64MEM);
		}
		break;
	case 0x04100000:
#ifndef MIN_SIZE
		if (Addr == 0x0410000C) {
			MoveX86regToVariable(x86Reg,&RegModValue);
			Pushad();
			Call_Direct(ChangeDpcStatus);
			Popad();
		} else
#endif
		MoveX86regToVariable(x86Reg,Addr + N64MEM);
		break;
	case 0x04300000:
		switch (Addr) {
		case 0x04300000:
#ifndef MIN_SIZE
			MoveX86regToVariable(x86Reg,&RegModValue);
			Pushad();
			Call_Direct(ChangeMiModeReg);
			Popad();
			break;
#endif
		case 0x0430000C:
			MoveX86regToVariable(x86Reg,&RegModValue);
			Pushad();
			Call_Direct(ChangeMiIntrMask);
			Popad();
			break;
		}
		break;
	case 0x04400000:
		switch (Addr) {
		case 0x04400000:
			if (ViStatusChanged != NULL) {
				CompX86regToVariable(x86Reg,&VI_STATUS_REG);
				JeLabel8(0);
				Jump = RecompPos - 1;
				MoveX86regToVariable(x86Reg,&VI_STATUS_REG);
				Pushad();
				Call_Direct(ViStatusChanged);
				Popad();
				*((BYTE *)(Jump))=(BYTE)(RecompPos - Jump - 1);
			}
			break;
		case 0x04400004:
			MoveX86regToVariable(x86Reg,&VI_ORIGIN_REG);
			AndConstToVariable(0xFFFFFF,&VI_ORIGIN_REG);
			break;
		case 0x04400008:
			if (ViWidthChanged != NULL) {
				CompX86regToVariable(x86Reg,&VI_WIDTH_REG);
				JeLabel8(0);
				Jump = RecompPos - 1;
				MoveX86regToVariable(x86Reg,&VI_WIDTH_REG);
				Pushad();
				Call_Direct(ViWidthChanged);
				Popad();
				*((BYTE *)(Jump))=(BYTE)(RecompPos - Jump - 1);
			}
			break;
		case 0x0440000C: MoveX86regToVariable(x86Reg,&VI_INTR_REG); break;
		case 0x04400010:
			AndConstToVariable(~MI_INTR_VI,&MI_INTR_REG);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
			break;
		case 0x04400014: MoveX86regToVariable(x86Reg,&VI_BURST_REG); break;
		case 0x04400018: MoveX86regToVariable(x86Reg,&VI_V_SYNC_REG); break;
		case 0x0440001C: MoveX86regToVariable(x86Reg,&VI_H_SYNC_REG); break;
		case 0x04400020: MoveX86regToVariable(x86Reg,&VI_LEAP_REG); break;
		case 0x04400024: MoveX86regToVariable(x86Reg,&VI_H_START_REG); break;
		case 0x04400028: MoveX86regToVariable(x86Reg,&VI_V_START_REG); break;
		case 0x0440002C: MoveX86regToVariable(x86Reg,&VI_V_BURST_REG); break;
		case 0x04400030: MoveX86regToVariable(x86Reg,&VI_X_SCALE_REG); break;
		case 0x04400034: MoveX86regToVariable(x86Reg,&VI_Y_SCALE_REG);
		}
		break;
	case 0x04500000: /* AI registers */
		switch (Addr) {
		case 0x04500000: MoveX86regToVariable(x86Reg,&AI_DRAM_ADDR_REG); break;
		case 0x04500004:
			MoveX86regToVariable(x86Reg,&AI_LEN_REG);
			Pushad();
			Call_Direct(AiLenChanged);
			Popad();
			break;
		case 0x04500008:
			MoveX86regToVariable(x86Reg,&AI_CONTROL_REG);
			AndConstToVariable(1,&AI_CONTROL_REG);
		case 0x0450000C:
			/* Clear Interrupt */;
			AndConstToVariable(~MI_INTR_AI,&MI_INTR_REG);
			AndConstToVariable(~MI_INTR_AI,&AudioIntrReg);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
			break;
		case 0x04500014: MoveX86regToVariable(x86Reg,&AI_BITRATE_REG); break;
		default:
			MoveX86regToVariable(x86Reg,Addr + N64MEM);
		}
		break;
	case 0x04600000:
		switch (Addr) {
		case 0x04600000: MoveX86regToVariable(x86Reg,&PI_DRAM_ADDR_REG); break;
		case 0x04600004: MoveX86regToVariable(x86Reg,&PI_CART_ADDR_REG); break;
		case 0x04600008:
			MoveX86regToVariable(x86Reg,&PI_RD_LEN_REG);
			Pushad();
			Call_Direct(&PI_DMA_READ);
			Popad();
			break;
		case 0x0460000C:
			MoveX86regToVariable(x86Reg,&PI_WR_LEN_REG);
			Pushad();
			Call_Direct(&PI_DMA_WRITE);
			Popad();
			break;
		case 0x04600010:
			AndConstToVariable(~MI_INTR_PI,&MI_INTR_REG);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
			break;
		case 0x04600014:
			MoveX86regToVariable(x86Reg,&PI_DOMAIN1_REG);
			AndConstToVariable(0xFF,&PI_DOMAIN1_REG);
			break;
		case 0x04600018:
			MoveX86regToVariable(x86Reg,&PI_BSD_DOM1_PWD_REG);
			AndConstToVariable(0xFF,&PI_BSD_DOM1_PWD_REG);
			break;
		case 0x0460001C:
			MoveX86regToVariable(x86Reg,&PI_BSD_DOM1_PGS_REG);
			AndConstToVariable(0xFF,&PI_BSD_DOM1_PGS_REG);
			break;
		case 0x04600020:
			MoveX86regToVariable(x86Reg,&PI_BSD_DOM1_RLS_REG);
			AndConstToVariable(0xFF,&PI_BSD_DOM1_RLS_REG);
			break;
#ifndef MIN_SIZE
		case 0x04600024:
			MoveX86regToVariable(x86Reg,&PI_DOMAIN2_REG);
			AndConstToVariable(0xFF,&PI_DOMAIN2_REG);
			break;
		case 0x04600028:
			MoveX86regToVariable(x86Reg,&PI_BSD_DOM2_PWD_REG);
			AndConstToVariable(0xFF,&PI_BSD_DOM2_PWD_REG);
			break;
		case 0x0460002C:
			MoveX86regToVariable(x86Reg,&PI_BSD_DOM2_PGS_REG);
			AndConstToVariable(0xFF,&PI_BSD_DOM2_PGS_REG);
			break;
		case 0x04600030:
			MoveX86regToVariable(x86Reg,&PI_BSD_DOM2_RLS_REG);
			AndConstToVariable(0xFF,&PI_BSD_DOM2_RLS_REG);
#endif
		}
		break;
	case 0x04700000:
		switch (Addr) {
		case 0x04700000: MoveX86regToVariable(x86Reg,&RI_MODE_REG); break;
		case 0x04700004: MoveX86regToVariable(x86Reg,&RI_CONFIG_REG); break;
		case 0x04700008: MoveX86regToVariable(x86Reg,&RI_CURRENT_LOAD_REG); break;
		case 0x0470000C: MoveX86regToVariable(x86Reg,&RI_SELECT_REG);  break;
		case 0x04700010: MoveX86regToVariable(x86Reg,&RI_REFRESH_REG); break;
		case 0x04700014: MoveX86regToVariable(x86Reg,&RI_LATENCY_REG); break;
		case 0x04700018: MoveX86regToVariable(x86Reg,&RI_RERROR_REG); break;
		case 0x0470001C: MoveX86regToVariable(x86Reg,&RI_WERROR_REG);
		}
		break;
	case 0x04800000:
		switch (Addr) {
		case 0x04800000: MoveX86regToVariable(x86Reg,&SI_DRAM_ADDR_REG); break;
		case 0x04800004:
			MoveX86regToVariable(x86Reg,&SI_PIF_ADDR_RD64B_REG);
			Pushad();
			Call_Direct(&SI_DMA_READ);
			Popad();
			break;
		case 0x04800010:
			MoveX86regToVariable(x86Reg,&SI_PIF_ADDR_WR64B_REG);
			Pushad();
			Call_Direct(&SI_DMA_WRITE);
			Popad();
			break;
		case 0x04800018:
			AndConstToVariable(~MI_INTR_SI,&MI_INTR_REG);
			AndConstToVariable(~SI_STATUS_INTERRUPT,&SI_STATUS_REG);
			Pushad();
			Call_Direct(CheckInterrupts);
			Popad();
		}
		break;
	case 0x1FC00000:
		MoveX86regToVariable(x86Reg,Addr + N64MEM);
		break;
	}
}
int r4300i_Command_MemoryFilter(DWORD dwExptCode,LPEXCEPTION_POINTERS lpEP) {
	DWORD MemAddress = (char *)lpEP->ExceptionRecord->ExceptionInformation[1] - (char *)N64MEM;
	EXCEPTION_RECORD exRec;
	if (dwExptCode != EXCEPTION_ACCESS_VIOLATION) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	exRec = *lpEP->ExceptionRecord;
	int offset = (int)((char*)lpEP->ExceptionRecord->ExceptionInformation[1] - N64MEM);
	if (offset < 0 || offset > 0x1FFFFFFF) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	switch(*(unsigned char *)lpEP->ContextRecord->Eip) {
	case 0x8B:
		switch(*(unsigned char *)(lpEP->ContextRecord->Eip + 1)) {
		case 0x04:
			lpEP->ContextRecord->Eip += 3;
			r4300i_LW_NonMemory((char *)exRec.ExceptionInformation[1] - (char *)N64MEM,&lpEP->ContextRecord->Eax);
			return EXCEPTION_CONTINUE_EXECUTION;
		case 0x0C:
			lpEP->ContextRecord->Eip += 3;
			r4300i_LW_NonMemory((char *)exRec.ExceptionInformation[1] - (char *)N64MEM,&lpEP->ContextRecord->Ecx);
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
int r4300i_CPU_MemoryFilter (DWORD dwExptCode,LPEXCEPTION_POINTERS lpEP) {
	DWORD MemAddress = (char *)lpEP->ExceptionRecord->ExceptionInformation[1] - (char *)N64MEM;
	EXCEPTION_RECORD exRec;
	BYTE * ReadPos,* TypePos;
	void * Reg;
	if (dwExptCode != EXCEPTION_ACCESS_VIOLATION) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	TypePos = (unsigned char *)lpEP->ContextRecord->Eip;
	exRec = *lpEP->ExceptionRecord;
	if ((int)(MemAddress) < 0 || MemAddress > 0x1FFFFFFF) { return EXCEPTION_CONTINUE_SEARCH; }
	if (*TypePos == 0xF3 && *(TypePos + 1) == 0xA5) {
		DWORD Start,End,count,OldProtect;
		Start = (lpEP->ContextRecord->Edi - (DWORD)N64MEM);
		End = (Start + (lpEP->ContextRecord->Ecx << 2) - 1);
		if ((int)Start < 0) {
			return EXCEPTION_CONTINUE_SEARCH;
		}
		if ((int) End < RDRAMsize) {
			for (count = Start; count < End; count += 0x1000) {
				if (N64_Blocks.NoOfRDRAMBlocks[(count >> 12)] > 0) {
					N64_Blocks.NoOfRDRAMBlocks[(count >> 12)] = 0;
					memset(JumpTable + ((count & 0x00FFFFF0) >> 2),0,0x1000);
					*(DelaySlotTable + count) = NULL;
					VirtualProtect(N64MEM + count,4,PAGE_READWRITE,&OldProtect);
				}
			}
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		if (Start >= 0x04000000 && End < 0x04001000) {
			N64_Blocks.NoOfDMEMBlocks = 0;
			memset(JumpTable + (0x04000000 >> 2),0,0x1000);
			*(DelaySlotTable + (0x04000000 >> 12)) = NULL;
			VirtualProtect(N64MEM + 0x04000000,4,PAGE_READWRITE,&OldProtect);
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		if (Start >= 0x04001000 && End < 0x04002000) {
			N64_Blocks.NoOfIMEMBlocks = 0;
			memset(JumpTable + (0x04001000 >> 2),0,0x1000);
			*(DelaySlotTable + (0x04001000 >> 12)) = NULL;
			VirtualProtect(N64MEM + 0x04001000,4,PAGE_READWRITE,&OldProtect);
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}
	if (*TypePos == 0x0F && *(TypePos + 1) == 0xB6) {
		ReadPos = TypePos + 2;
	} else if (*TypePos == 0x0F && *(TypePos + 1) == 0xB7) {
		ReadPos = TypePos + 2;
	} else if (*TypePos == 0x0F && *(TypePos + 1) == 0xBE) {
		ReadPos = TypePos + 2;
	} else if (*TypePos == 0x0F && *(TypePos + 1) == 0xBF) {
		ReadPos = TypePos + 2;
	} else if (*TypePos == 0x66) {
		ReadPos = TypePos + 2;
	} else {
		ReadPos = TypePos + 1;
	}
	switch ((*ReadPos & 0x38)) {
	case 0x00: Reg = &lpEP->ContextRecord->Eax; break;
	case 0x08: Reg = &lpEP->ContextRecord->Ecx; break;
	case 0x10: Reg = &lpEP->ContextRecord->Edx; break;
	case 0x18: Reg = &lpEP->ContextRecord->Ebx; break;
	case 0x20: Reg = &lpEP->ContextRecord->Esp; break;
	case 0x28: Reg = &lpEP->ContextRecord->Ebp; break;
	case 0x30: Reg = &lpEP->ContextRecord->Esi; break;
	case 0x38: Reg = &lpEP->ContextRecord->Edi; break;
	}
	switch ((*ReadPos & 0xC7)) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 6:
	case 7: ReadPos += 1; break;
	case 4:
	case 64:
	case 65:
	case 66:
	case 67:
	case 70:
	case 71: ReadPos += 2; break;
	case 5:
	case 128:
	case 129:
	case 130:
	case 131:
	case 134:
	case 135: ReadPos += 5; break;
	default:
		return EXCEPTION_CONTINUE_SEARCH;
	}
	switch(*TypePos) {
	case 0x0F:
		switch(*(TypePos + 1)) {
		case 0xB6:
			r4300i_LB_NonMemory(MemAddress,Reg,FALSE);
			lpEP->ContextRecord->Eip = (DWORD)ReadPos;
			return EXCEPTION_CONTINUE_EXECUTION;
		case 0xB7:
			r4300i_LH_NonMemory(MemAddress,Reg,FALSE);
			lpEP->ContextRecord->Eip = (DWORD)ReadPos;
			return EXCEPTION_CONTINUE_EXECUTION;
		case 0xBE:
			r4300i_LB_NonMemory(MemAddress,Reg,TRUE);
			lpEP->ContextRecord->Eip = (DWORD)ReadPos;
			return EXCEPTION_CONTINUE_EXECUTION;
		case 0xBF:
			r4300i_LH_NonMemory(MemAddress,Reg,TRUE);
			lpEP->ContextRecord->Eip = (DWORD)ReadPos;
			return EXCEPTION_CONTINUE_EXECUTION;
		default:
			return EXCEPTION_CONTINUE_SEARCH;
		}
		break;
	case 0x66:
		switch(*(TypePos + 1)) {
		case 0x8B:
			r4300i_LH_NonMemory(MemAddress,Reg,FALSE);
			lpEP->ContextRecord->Eip = (DWORD)ReadPos;
			return EXCEPTION_CONTINUE_EXECUTION;
		case 0x89:
			r4300i_SH_NonMemory(MemAddress,*(WORD *)Reg);
			lpEP->ContextRecord->Eip = (DWORD)ReadPos;
			return EXCEPTION_CONTINUE_EXECUTION;
		case 0xC7:
			if (Reg != &lpEP->ContextRecord->Eax) { return EXCEPTION_CONTINUE_SEARCH; }
			r4300i_SH_NonMemory(MemAddress,*(WORD *)ReadPos);
			lpEP->ContextRecord->Eip = (DWORD)(ReadPos + 2);
			return EXCEPTION_CONTINUE_EXECUTION;
		default:
			return EXCEPTION_CONTINUE_SEARCH;
		}
		break;
	case 0x88:
		r4300i_SB_NonMemory(MemAddress,*(BYTE *)Reg);
		lpEP->ContextRecord->Eip = (DWORD)ReadPos;
		return EXCEPTION_CONTINUE_EXECUTION;
	case 0x8A:
		r4300i_LB_NonMemory(MemAddress,Reg,FALSE);
		lpEP->ContextRecord->Eip = (DWORD)ReadPos;
		return EXCEPTION_CONTINUE_EXECUTION;
	case 0x8B:
		r4300i_LW_NonMemory(MemAddress,Reg);
		lpEP->ContextRecord->Eip = (DWORD)ReadPos;
		return EXCEPTION_CONTINUE_EXECUTION;
	case 0x89:
		r4300i_SW_NonMemory(MemAddress,*(DWORD *)Reg);
		lpEP->ContextRecord->Eip = (DWORD)ReadPos;
		return EXCEPTION_CONTINUE_EXECUTION;
	case 0xC6:
		if (Reg != &lpEP->ContextRecord->Eax) { return EXCEPTION_CONTINUE_SEARCH; }
		r4300i_SB_NonMemory(MemAddress,*(BYTE *)ReadPos);
		lpEP->ContextRecord->Eip = (DWORD)(ReadPos + 1);
		return EXCEPTION_CONTINUE_EXECUTION;
	case 0xC7:
		if (Reg != &lpEP->ContextRecord->Eax) { return EXCEPTION_CONTINUE_SEARCH; }
		r4300i_SW_NonMemory(MemAddress,*(DWORD *)ReadPos);
		lpEP->ContextRecord->Eip = (DWORD)(ReadPos + 4);
		return EXCEPTION_CONTINUE_EXECUTION;
	default:
		return EXCEPTION_CONTINUE_SEARCH;
	}
}
int r4300i_LB_NonMemory (DWORD PAddr,DWORD * Value,BOOL SignExtend) {
	if (PAddr >= 0x10000000 && PAddr < 0x16000000) {
		if (WrittenToRom) return FALSE;
		if ((PAddr & 2) == 0) { PAddr = (PAddr + 4) ^ 2; }
		if ((PAddr - 0x10000000) < RomFileSize) {
			if (SignExtend) {
				(int)* Value = (char)ROM[PAddr - 0x10000000];
			} else {
				* Value = ROM[PAddr - 0x10000000];
			}
			return TRUE;
		}
	}
	* Value = 0;
	return FALSE;
}
BOOL r4300i_LB_VAddr (DWORD VAddr,BYTE * Value) {
	if (TLB_ReadMap[VAddr >> 12] == 0) return FALSE;
	* Value = *(BYTE *)(TLB_ReadMap[VAddr >> 12] + (VAddr ^ 3));
	return TRUE;
}
BOOL r4300i_LD_VAddr (DWORD VAddr,unsigned _int64 * Value) {
	if (TLB_ReadMap[VAddr >> 12] == 0) return FALSE;
	*((DWORD *)(Value) + 1) = *(DWORD *)(TLB_ReadMap[VAddr >> 12] + VAddr);
	*((DWORD *)(Value)) = *(DWORD *)(TLB_ReadMap[VAddr >> 12] + VAddr + 4);
	return TRUE;
}
int r4300i_LH_NonMemory (DWORD PAddr,DWORD * Value,int SignExtend) {
	* Value = 0;
	return FALSE;
}
BOOL r4300i_LH_VAddr (DWORD VAddr,WORD * Value) {
	if (TLB_ReadMap[VAddr >> 12] == 0) return FALSE;
	* Value = *(WORD *)(TLB_ReadMap[VAddr >> 12] + (VAddr ^ 2));
	return TRUE;
}
int r4300i_LW_NonMemory (DWORD PAddr,DWORD * Value) {
	DWORD base;
	// 0x06000000 0x08000000 N64DD IPL (J)
	if (PAddr >= 0x06000000 && PAddr < 0x08000000) {
		base = 0x06000000;
		goto HandleROM;
	}
	if (PAddr >= 0x10000000 && PAddr < 0x16000000) {
		base = 0x10000000;
		goto HandleROM;
	}
	if (PAddr >= 0x06000000 && PAddr < 0x08000000) {
		if (WrittenToRom) {
			* Value = WroteToRom;
			WrittenToRom = FALSE;
			return TRUE;
		}
		if ((PAddr - 0x06000000) < RomFileSize) {
			* Value = *(DWORD *)&ROM[PAddr - 0x06000000];
			return TRUE;
		} else {
			* Value = PAddr & 0xFFFF;
			* Value = (* Value << 16) | * Value;
			return FALSE;
		}
	}
	if (PAddr >= 0x10000000 && PAddr < 0x16000000) {
		if (WrittenToRom) {
			* Value = WroteToRom;
			WrittenToRom = FALSE;
			return TRUE;
		}
		if ((PAddr - 0x10000000) < RomFileSize) {
			* Value = *(DWORD *)&ROM[PAddr - 0x10000000];
			return TRUE;
		} else {
			* Value = PAddr & 0xFFFF;
			* Value = (* Value << 16) | * Value;
			return FALSE;
		}
	}
	switch (PAddr & 0xFFF00000) {
	case 0x03F00000:
		switch (PAddr) {
		case 0x03F00000: * Value = RDRAM_CONFIG_REG; break;
		case 0x03F00004: * Value = RDRAM_DEVICE_ID_REG; break;
		case 0x03F00008: * Value = RDRAM_DELAY_REG; break;
		case 0x03F0000C: * Value = RDRAM_MODE_REG; break;
		case 0x03F00010: * Value = RDRAM_REF_INTERVAL_REG; break;
		case 0x03F00014: * Value = RDRAM_REF_ROW_REG; break;
		case 0x03F00018: * Value = RDRAM_RAS_INTERVAL_REG; break;
		case 0x03F0001C: * Value = RDRAM_MIN_INTERVAL_REG; break;
		case 0x03F00020: * Value = RDRAM_ADDR_SELECT_REG; break;
		case 0x03F00024: * Value = RDRAM_DEVICE_MANUF_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04000000:
		switch (PAddr) {
#ifndef MIN_SIZE
		case 0x04040000: * Value = SP_MEM_ADDR_REG; break;
		case 0x04040004: * Value = SP_DRAM_ADDR_REG; break;
		case 0x04040008: * Value = SP_RD_LEN_REG; break;
		case 0x0404000C: * Value = SP_WR_LEN_REG; break;
#endif
		case 0x04040010: * Value = SP_STATUS_REG; break;
		case 0x04040014: * Value = SP_DMA_FULL_REG; break;
		case 0x04040018: * Value = SP_DMA_BUSY_REG; break;
#ifndef MIN_SIZE
		case 0x0404001C: * Value = SP_SEMAPHORE_REG; break;
#endif
		case 0x04080000: * Value = SP_PC_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04100000:
		switch (PAddr) {
		case 0x0410000C: * Value = DPC_STATUS_REG; break;
		case 0x04100010: * Value = DPC_CLOCK_REG; break;
		case 0x04100014: * Value = DPC_BUFBUSY_REG; break;
		case 0x04100018: * Value = DPC_PIPEBUSY_REG; break;
		case 0x0410001C: * Value = DPC_TMEM_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04300000:
		switch (PAddr) {
		case 0x04300000: * Value = MI_MODE_REG; break;
		case 0x04300004: * Value = MI_VERSION_REG; break;
		case 0x04300008: * Value = MI_INTR_REG; break;
		case 0x0430000C: * Value = MI_INTR_MASK_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04400000:
		switch (PAddr) {
		case 0x04400000: * Value = VI_STATUS_REG; break;
		case 0x04400004: * Value = VI_ORIGIN_REG; break;
		case 0x04400008: * Value = VI_WIDTH_REG; break;
		case 0x0440000C: * Value = VI_INTR_REG; break;
		case 0x04400010:
			UpdateCurrentHalfLine();
			* Value = HalfLine;
			break;
		case 0x04400014: * Value = VI_BURST_REG; break;
		case 0x04400018: * Value = VI_V_SYNC_REG; break;
		case 0x0440001C: * Value = VI_H_SYNC_REG; break;
		case 0x04400020: * Value = VI_LEAP_REG; break;
		case 0x04400024: * Value = VI_H_START_REG; break;
		case 0x04400028: * Value = VI_V_START_REG ; break;
		case 0x0440002C: * Value = VI_V_BURST_REG; break;
		case 0x04400030: * Value = VI_X_SCALE_REG; break;
		case 0x04400034: * Value = VI_Y_SCALE_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04500000:
		switch (PAddr) {
#ifndef MIN_SIZE
		case 0x04500000: * Value = AI_DRAM_ADDR_REG; break;
#endif
		case 0x04500004:
			if (AiReadLength != NULL) {
				* Value = AiReadLength();
			} else {
				* Value = 0;
			}
			break;
#ifndef MIN_SIZE
		case 0x04500008: * Value = AI_CONTROL_REG; break;
#endif
		case 0x0450000C: * Value = AI_STATUS_REG; break;
#ifndef MIN_SIZE
		case 0x04500010: * Value = AI_DACRATE_REG; break;
		case 0x04500014: * Value = AI_BITRATE_REG; break;
#endif
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04600000:
		switch (PAddr) {
#ifndef MIN_SIZE
		case 0x04600000: * Value = PI_DRAM_ADDR_REG; break;
		case 0x04600004: * Value = PI_CART_ADDR_REG; break;
		case 0x04600008: * Value = PI_RD_LEN_REG; break;
		case 0x0460000C: * Value = PI_WR_LEN_REG; break;
#endif
		case 0x04600010: * Value = PI_STATUS_REG; break;
		case 0x04600014: * Value = PI_DOMAIN1_REG; break;
		case 0x04600018: * Value = PI_BSD_DOM1_PWD_REG; break;
		case 0x0460001C: * Value = PI_BSD_DOM1_PGS_REG; break;
		case 0x04600020: * Value = PI_BSD_DOM1_RLS_REG; break;
		case 0x04600024: * Value = PI_DOMAIN2_REG; break;
		case 0x04600028: * Value = PI_BSD_DOM2_PWD_REG; break;
		case 0x0460002C: * Value = PI_BSD_DOM2_PGS_REG; break;
		case 0x04600030: * Value = PI_BSD_DOM2_RLS_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04700000:
		switch (PAddr) {
		case 0x04700000: * Value = RI_MODE_REG; break;
		case 0x04700004: * Value = RI_CONFIG_REG; break;
		case 0x04700008: * Value = RI_CURRENT_LOAD_REG; break;
		case 0x0470000C: * Value = RI_SELECT_REG; break;
		case 0x04700010: * Value = RI_REFRESH_REG; break;
		case 0x04700014: * Value = RI_LATENCY_REG; break;
		case 0x04700018: * Value = RI_RERROR_REG; break;
		case 0x0470001C: * Value = RI_WERROR_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x04800000:
		switch (PAddr) {
#ifndef MIN_SIZE
		case 0x04800000: * Value = SI_DRAM_ADDR_REG; break;
		case 0x04800004: * Value = SI_PIF_ADDR_RD64B_REG; break;
		case 0x04800010: * Value = SI_PIF_ADDR_WR64B_REG; break;
#endif
		case 0x04800018: * Value = SI_STATUS_REG; break;
		default:
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x08000000:
		if (SaveUsing == Auto) { SaveUsing = FlashRAM; }
		if (SaveUsing != FlashRAM) {
			* Value = PAddr & 0xFFFF;
			* Value = (* Value << 16) | * Value;
			return FALSE;
		}
		* Value = ReadFromFlashStatus();
		break;
	case 0x1FC00000:
		if (PAddr < 0x1FC007C0) {
			DWORD ToSwap = *(DWORD *)(&PifRom[PAddr - 0x1FC00000]);
			_asm {
				mov eax,ToSwap
				bswap eax
				mov ToSwap,eax
			}
			* Value = ToSwap;
			return TRUE;
		} else if (PAddr < 0x1FC00800) {
			DWORD ToSwap = *(DWORD *)(&PIF_Ram[PAddr - 0x1FC007C0]);
			_asm {
				mov eax,ToSwap
				bswap eax
				mov ToSwap,eax
			}
			* Value = ToSwap;
			return TRUE;
		} else {
			* Value = 0;
			return FALSE;
		}
		break;
	case 0x1FF00000:
		read_summercart_regs(NULL,PAddr,Value);
		break;
	default:
		* Value = PAddr & 0xFFFF;
		* Value = (* Value << 16) | * Value;
		return FALSE;
	}
	return TRUE;
	HandleROM:
		if (WrittenToRom) {
			* Value = WroteToRom;
			WrittenToRom = FALSE;
			return TRUE;
		}
		if ((PAddr - base) < RomFileSize) {
			* Value = *(DWORD *)&ROM[PAddr - base];
			return TRUE;
		}
		* Value = PAddr & 0xFFFF;
		* Value = (*Value << 16) | *Value;
		return FALSE;
}
BOOL r4300i_LW_VAddr(DWORD VAddr,DWORD* Value) {
	if (TLB_ReadMap[VAddr >> 12] == 0) return FALSE;
	* Value = *(DWORD *)(TLB_ReadMap[VAddr >> 12] + VAddr);
	if (* Value == 0x14200005 && (VAddr & 0xFF000000) == 0x80000000) {
		const DWORD sBusyLoopPrologue[] = { 0x3C02A440,0xA7380000,0x8C830000,0x34420010,0x3C0CA440,0x8C680008,0x8D090004,0xAC69000C,0x8C4A0000,0x2D41000B };
		int sCheckSize = sizeof(sBusyLoopPrologue) / sizeof(*sBusyLoopPrologue),i;
		for (i = 0; i < sCheckSize; i++) {
			DWORD ivaddr = VAddr - (sCheckSize - i) * 4,val;
			if (TLB_ReadMap[ivaddr >> 12] == 0) break;
			val = *(DWORD *)(TLB_ReadMap[ivaddr >> 12] + ivaddr);
			if (val != sBusyLoopPrologue[i]) break;
		}
		if (i == sCheckSize) * Value = 0x10000005;
	}
	return TRUE;
}
int r4300i_SB_NonMemory (DWORD PAddr,BYTE Value) {
	switch (PAddr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		if (PAddr < RDRAMsize) {
			DWORD OldProtect;
			VirtualProtect((N64MEM + PAddr),1,PAGE_READWRITE,&OldProtect);
			*(BYTE *)(N64MEM+PAddr) = Value;
			if (N64_Blocks.NoOfRDRAMBlocks[(PAddr & 0x00FFFFF0) >> 12] == 0) { break; }
			N64_Blocks.NoOfRDRAMBlocks[(PAddr & 0x00FFFFF0) >> 12] = 0;
			memset(JumpTable+((PAddr & 0xFFFFF000) >> 2),0,0x1000);
			*(DelaySlotTable + ((PAddr & 0xFFFFF000) >> 12)) = NULL;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL r4300i_SB_VAddr (DWORD VAddr,BYTE Value) {
	if (TLB_WriteMap[VAddr >> 12] == 0) return FALSE;
	*(BYTE *)(TLB_WriteMap[VAddr >> 12] + (VAddr ^ 3)) = Value;
	return TRUE;
}
int r4300i_SH_NonMemory (DWORD PAddr,WORD Value) {
	switch (PAddr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		if (PAddr < RDRAMsize) {
			DWORD OldProtect;
			VirtualProtect((N64MEM + PAddr),2,PAGE_READWRITE,&OldProtect);
			*(WORD *)(N64MEM+PAddr) = Value;
			if (N64_Blocks.NoOfRDRAMBlocks[(PAddr & 0x00FFFFF0) >> 12] == 0) { break; }
			N64_Blocks.NoOfRDRAMBlocks[(PAddr & 0x00FFFFF0) >> 12] = 0;
			memset(JumpTable+((PAddr & 0xFFFFF000) >> 2),0,0x1000);
			*(DelaySlotTable + ((PAddr & 0xFFFFF000) >> 12)) = NULL;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL r4300i_SD_VAddr (DWORD VAddr,unsigned _int64 Value) {
	if (TLB_WriteMap[VAddr >> 12] == 0) return FALSE;
	*(DWORD *)(TLB_WriteMap[VAddr >> 12] + VAddr) = *((DWORD *)(&Value) + 1);
	*(DWORD *)(TLB_WriteMap[VAddr >> 12] + VAddr + 4) = *((DWORD *)(&Value));
	return TRUE;
}
BOOL r4300i_SH_VAddr (DWORD VAddr,WORD Value) {
	if (TLB_WriteMap[VAddr >> 12] == 0) return FALSE;
	*(WORD *)(TLB_WriteMap[VAddr >> 12] + (VAddr ^ 2)) = Value;
	return TRUE;
}
int r4300i_SW_NonMemory (DWORD PAddr,DWORD Value) {
// This function is missing memory registers for ifndef MIN_SIZE!
	if (PAddr >= 0x10000000 && PAddr < 0x16000000) {
		if ((PAddr - 0x10000000) < RomFileSize) {
			WrittenToRom = TRUE;
			WroteToRom = Value;
		} else {
			return FALSE;
		}
	}
	switch (PAddr & 0xFFF00000) {
	case 0x00000000:
	case 0x00100000:
	case 0x00200000:
	case 0x00300000:
	case 0x00400000:
	case 0x00500000:
	case 0x00600000:
	case 0x00700000:
		if (PAddr < RDRAMsize) {
			DWORD OldProtect;
			VirtualProtect((N64MEM + PAddr),4,PAGE_READWRITE,&OldProtect);
			*(DWORD *)(N64MEM+PAddr) = Value;
			if (N64_Blocks.NoOfRDRAMBlocks[(PAddr & 0x00FFFFF0) >> 12] == 0) { break; }
			N64_Blocks.NoOfRDRAMBlocks[(PAddr & 0x00FFFFF0) >> 12] = 0;
			memset(JumpTable+((PAddr & 0xFFFFF000) >> 2),0,0x1000);
			*(DelaySlotTable + ((PAddr & 0xFFFFF000) >> 12)) = NULL;
		}
		break;
	case 0x03F00000:
		switch (PAddr) {
		case 0x03F00000: RDRAM_CONFIG_REG = Value; break;
		case 0x03F00004: RDRAM_DEVICE_ID_REG = Value; break;
		case 0x03F00008: RDRAM_DELAY_REG = Value; break;
		case 0x03F0000C: RDRAM_MODE_REG = Value; break;
		case 0x03F00010: RDRAM_REF_INTERVAL_REG = Value; break;
		case 0x03F00014: RDRAM_REF_ROW_REG = Value; break;
		case 0x03F00018: RDRAM_RAS_INTERVAL_REG = Value; break;
		case 0x03F0001C: RDRAM_MIN_INTERVAL_REG = Value; break;
		case 0x03F00020: RDRAM_ADDR_SELECT_REG = Value; break;
		case 0x03F00024: RDRAM_DEVICE_MANUF_REG = Value;
		case 0x03F04004:
		case 0x03F08004:
		case 0x03F80004:
		case 0x03F80008:
		case 0x03F8000C:
		case 0x03F80014: break;
		default:
			return FALSE;
		}
		break;
	case 0x04000000:
		if (PAddr < 0x04002000) {
			DWORD OldProtect;
			VirtualProtect((N64MEM + PAddr),4,PAGE_READWRITE,&OldProtect);
			*(DWORD *)(N64MEM+PAddr) = Value;
			if (PAddr < 0x04001000) {
				if (N64_Blocks.NoOfDMEMBlocks == 0) { break; }
				N64_Blocks.NoOfDMEMBlocks = 0;
			} else {
				if (N64_Blocks.NoOfIMEMBlocks == 0) { break; }
				N64_Blocks.NoOfIMEMBlocks = 0;
			}
			memset(JumpTable+((PAddr & 0xFFFFF000) >> 2),0,0x1000);
			*(DelaySlotTable + ((PAddr & 0xFFFFF000) >> 12)) = NULL;
			return TRUE;
		}
		switch (PAddr) {
		case 0x04040000: SP_MEM_ADDR_REG = Value; break;
		case 0x04040004: SP_DRAM_ADDR_REG = Value; break;
		case 0x04040008:
			SP_RD_LEN_REG = Value;
			SP_DMA_READ();
			break;
		case 0x0404000C:
			SP_WR_LEN_REG = Value;
			SP_DMA_WRITE();
			break;
		case 0x04040010:
			if ((Value & SP_CLR_HALT) != 0) { SP_STATUS_REG &= ~SP_STATUS_HALT; }
			if ((Value & SP_SET_HALT) != 0) { SP_STATUS_REG |= SP_STATUS_HALT;  }
			if ((Value & SP_CLR_BROKE) != 0) { SP_STATUS_REG &= ~SP_STATUS_BROKE; }
			if ((Value & SP_CLR_INTR) != 0) {
				MI_INTR_REG &= ~MI_INTR_SP;
				CheckInterrupts();
			}
			if ((Value & SP_CLR_SSTEP) != 0) { SP_STATUS_REG &= ~SP_STATUS_SSTEP; }
			if ((Value & SP_SET_SSTEP) != 0) { SP_STATUS_REG |= SP_STATUS_SSTEP;  }
			if ((Value & SP_CLR_INTR_BREAK) != 0) { SP_STATUS_REG &= ~SP_STATUS_INTR_BREAK; }
			if ((Value & SP_SET_INTR_BREAK) != 0) { SP_STATUS_REG |= SP_STATUS_INTR_BREAK;  }
			if ((Value & SP_CLR_SIG0) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG0; }
			if ((Value & SP_SET_SIG0) != 0) {
				SP_STATUS_REG |= SP_STATUS_SIG0;
				if (AudioSignal) {
					MI_INTR_REG |= MI_INTR_SP;
					CheckInterrupts();
				}
			}
			if ((Value & SP_CLR_SIG1) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG1; }
			if ((Value & SP_SET_SIG1) != 0) { SP_STATUS_REG |= SP_STATUS_SIG1;  }
			if ((Value & SP_CLR_SIG2) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG2; }
			if ((Value & SP_SET_SIG2) != 0) { SP_STATUS_REG |= SP_STATUS_SIG2;  }
			if ((Value & SP_CLR_SIG3) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG3; }
			if ((Value & SP_SET_SIG3) != 0) { SP_STATUS_REG |= SP_STATUS_SIG3;  }
			if ((Value & SP_CLR_SIG4) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG4; }
			if ((Value & SP_SET_SIG4) != 0) { SP_STATUS_REG |= SP_STATUS_SIG4;  }
			if ((Value & SP_CLR_SIG5) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG5; }
			if ((Value & SP_SET_SIG5) != 0) { SP_STATUS_REG |= SP_STATUS_SIG5;  }
			if ((Value & SP_CLR_SIG6) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG6; }
			if ((Value & SP_SET_SIG6) != 0) { SP_STATUS_REG |= SP_STATUS_SIG6;  }
			if ((Value & SP_CLR_SIG7) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG7; }
			if ((Value & SP_SET_SIG7) != 0) { SP_STATUS_REG |= SP_STATUS_SIG7;  }
			if (DelayRDP && *(DWORD *)(DMEM + 0xFC0) == 1 || DelayRSP && *(DWORD *)(DMEM + 0xFC0) == 2) {
				ChangeTimer(RspTimer,0x2000);
				break;
			}
			RunRsp();
			break;
		case 0x0404001C: SP_SEMAPHORE_REG = 0; break;
		case 0x04080000: SP_PC_REG = Value & 0xFFC; break;
		default:
			return FALSE;
		}
		break;
	case 0x04100000:
		switch (PAddr) {
		case 0x04100000:
			DPC_START_REG = Value;
			DPC_CURRENT_REG = Value;
			break;
		case 0x04100004:
			DPC_END_REG = Value;
			if (ProcessRDPList) { ProcessRDPList(); }
			break;
		case 0x0410000C:
			if ((Value & DPC_CLR_XBUS_DMEM_DMA) != 0) { DPC_STATUS_REG &= ~DPC_STATUS_XBUS_DMEM_DMA; }
			if ((Value & DPC_SET_XBUS_DMEM_DMA) != 0) { DPC_STATUS_REG |= DPC_STATUS_XBUS_DMEM_DMA;  }
			if ((Value & DPC_CLR_FREEZE) != 0) { DPC_STATUS_REG &= ~DPC_STATUS_FREEZE; }
			if ((Value & DPC_SET_FREEZE) != 0) { DPC_STATUS_REG |= DPC_STATUS_FREEZE;  }
			if ((Value & DPC_CLR_FLUSH) != 0) { DPC_STATUS_REG &= ~DPC_STATUS_FLUSH; }
			if ((Value & DPC_SET_FLUSH) != 0) { DPC_STATUS_REG |= DPC_STATUS_FLUSH;  }
			if ((Value & DPC_CLR_FREEZE) != 0)
			{
				if ((SP_STATUS_REG & SP_STATUS_HALT) == 0)
				{
					if ((SP_STATUS_REG & SP_STATUS_BROKE) == 0)
					{
						RunRsp();
					}
				}
			}
			break;
		default:
			return FALSE;
		}
		break;
	case 0x04300000:
		switch (PAddr) {
		case 0x04300000:
			MI_MODE_REG &= ~0x7F;
			MI_MODE_REG |= (Value & 0x7F);
			if ((Value & MI_CLR_INIT) != 0) { MI_MODE_REG &= ~MI_MODE_INIT; }
			if ((Value & MI_SET_INIT) != 0) { MI_MODE_REG |= MI_MODE_INIT; }
			if ((Value & MI_CLR_EBUS) != 0) { MI_MODE_REG &= ~MI_MODE_EBUS; }
			if ((Value & MI_SET_EBUS) != 0) { MI_MODE_REG |= MI_MODE_EBUS; }
			if ((Value & MI_CLR_DP_INTR) != 0) {
				MI_INTR_REG &= ~MI_INTR_DP;
				CheckInterrupts();
			}
			if ((Value & MI_CLR_RDRAM) != 0) { MI_MODE_REG &= ~MI_MODE_RDRAM; }
			if ((Value & MI_SET_RDRAM) != 0) { MI_MODE_REG |= MI_MODE_RDRAM; }
			break;
		case 0x0430000C:
			if ((Value & MI_INTR_MASK_CLR_SP) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_SP; }
			if ((Value & MI_INTR_MASK_SET_SP) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_SP; }
			if ((Value & MI_INTR_MASK_CLR_SI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_SI; }
			if ((Value & MI_INTR_MASK_SET_SI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_SI; }
			if ((Value & MI_INTR_MASK_CLR_AI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_AI; }
			if ((Value & MI_INTR_MASK_SET_AI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_AI; }
			if ((Value & MI_INTR_MASK_CLR_VI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_VI; }
			if ((Value & MI_INTR_MASK_SET_VI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_VI; }
			if ((Value & MI_INTR_MASK_CLR_PI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_PI; }
			if ((Value & MI_INTR_MASK_SET_PI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_PI; }
			if ((Value & MI_INTR_MASK_CLR_DP) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_DP; }
			if ((Value & MI_INTR_MASK_SET_DP) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_DP; }
			break;
		default:
			return FALSE;
		}
		break;
	case 0x04400000:
		switch (PAddr) {
		case 0x04400000:
			if (VI_STATUS_REG != Value) {
				VI_STATUS_REG = Value;
				if (ViStatusChanged != NULL) { ViStatusChanged(); }
			}
			break;
		case 0x04400004:
			VI_ORIGIN_REG = (Value & 0xFFFFFF);
			break;
		case 0x04400008:
			if (VI_WIDTH_REG != Value) {
				VI_WIDTH_REG = Value;
				if (ViWidthChanged != NULL) { ViWidthChanged(); }
			}
			break;
		case 0x0440000C: VI_INTR_REG = Value; break;
		case 0x04400010:
			MI_INTR_REG &= ~MI_INTR_VI;
			CheckInterrupts();
			break;
		case 0x04400014: VI_BURST_REG = Value; break;
		case 0x04400018: VI_V_SYNC_REG = Value; break;
		case 0x0440001C: VI_H_SYNC_REG = Value; break;
		case 0x04400020: VI_LEAP_REG = Value; break;
		case 0x04400024: VI_H_START_REG = Value; break;
		case 0x04400028: VI_V_START_REG = Value; break;
		case 0x0440002C: VI_V_BURST_REG = Value; break;
		case 0x04400030: VI_X_SCALE_REG = Value; break;
		case 0x04400034: VI_Y_SCALE_REG = Value; break;
		default:
			return FALSE;
		}
		break;
	case 0x04500000:
		switch (PAddr) {
		case 0x04500000: AI_DRAM_ADDR_REG = Value; break;
		case 0x04500004:
			AI_LEN_REG = Value;
			if (AiLenChanged != NULL) { AiLenChanged(); }
			break;
		case 0x04500008: AI_CONTROL_REG = (Value & 1); break;
		case 0x0450000C:
			/* Clear Interrupt */;
			MI_INTR_REG &= ~MI_INTR_AI;
			AudioIntrReg &= ~MI_INTR_AI;
			CheckInterrupts();
			break;
		case 0x04500010:
			AI_DACRATE_REG = Value;
			if (AiDacrateChanged != NULL) { AiDacrateChanged(SYSTEM_NTSC); }
			break;
		case 0x04500014: AI_BITRATE_REG = Value; break;
		default:
			return FALSE;
		}
		break;
	case 0x04600000:
		switch (PAddr) {
		case 0x04600000: PI_DRAM_ADDR_REG = Value; break;
		case 0x04600004: PI_CART_ADDR_REG = Value; break;
		case 0x04600008:
			PI_RD_LEN_REG = Value;
			PI_DMA_READ();
			break;
		case 0x0460000C:
			PI_WR_LEN_REG = Value;
			PI_DMA_WRITE();
			break;
		case 0x04600010:
			if ((Value & PI_CLR_INTR) != 0) {
				MI_INTR_REG &= ~MI_INTR_PI;
				CheckInterrupts();
			}
			break;
		case 0x04600014: PI_DOMAIN1_REG = (Value & 0xFF); break;
		case 0x04600018: PI_BSD_DOM1_PWD_REG = (Value & 0xFF); break;
		case 0x0460001C: PI_BSD_DOM1_PGS_REG = (Value & 0xFF); break;
		case 0x04600020: PI_BSD_DOM1_RLS_REG = (Value & 0xFF); break;
#ifndef MIN_SIZE
		case 0x04600024: PI_DOMAIN2_REG = (Value & 0xFF); break;
		case 0x04600028: PI_BSD_DOM2_PWD_REG = (Value & 0xFF); break;
		case 0x0460002C: PI_BSD_DOM2_PGS_REG = (Value & 0xFF); break;
		case 0x04600030: PI_BSD_DOM2_RLS_REG = (Value & 0xFF); break;
#endif
		default:
			return FALSE;
		}
		break;
	case 0x04700000:
		switch (PAddr) {
		case 0x04700000: RI_MODE_REG = Value; break;
		case 0x04700004: RI_CONFIG_REG = Value; break;
		case 0x04700008: RI_CURRENT_LOAD_REG = Value; break;
		case 0x0470000C: RI_SELECT_REG = Value; break;
		case 0x04700010: RI_REFRESH_REG = Value; break;
		case 0x04700014: RI_LATENCY_REG = Value; break;
		case 0x04700018: RI_RERROR_REG = Value; break;
		case 0x0470001C: RI_WERROR_REG = Value; break;
		default:
			return FALSE;
		}
		break;
	case 0x04800000:
		switch (PAddr) {
		case 0x04800000: SI_DRAM_ADDR_REG = Value; break;
		case 0x04800004:
			SI_PIF_ADDR_RD64B_REG = Value;
			SI_DMA_READ();
			break;
		case 0x04800010:
			SI_PIF_ADDR_WR64B_REG = Value;
			SI_DMA_WRITE();
			break;
		case 0x04800018:
			MI_INTR_REG &= ~MI_INTR_SI;
			SI_STATUS_REG &= ~SI_STATUS_INTERRUPT;
			CheckInterrupts();
			break;
		default:
			return FALSE;
		}
		break;
	case 0x08000000:
		if (PAddr != 0x08010000) return FALSE;
		if (SaveUsing == Auto) { SaveUsing = FlashRAM; }
		if (SaveUsing != FlashRAM) return TRUE;
		WriteToFlashCommand(Value);
		break;
	case 0x1FC00000:
		if (PAddr < 0x1FC007C0) {
			return FALSE;
		} else if (PAddr < 0x1FC00800) {
			_asm {
				mov eax,Value
				bswap eax
				mov Value,eax
			}
			*(DWORD *)(&PIF_Ram[PAddr - 0x1FC007C0]) = Value;
			if (PAddr == 0x1FC007FC) {
				PifRamWrite();
			}
			return TRUE;
		}
		return FALSE;
	case 0x1FF00000:
		write_summercart_regs(NULL,PAddr,Value,~0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL r4300i_SW_VAddr (DWORD VAddr,DWORD Value) {
	if (TLB_WriteMap[VAddr >> 12] == 0) return FALSE;
	*(DWORD *)(TLB_WriteMap[VAddr >> 12] + VAddr) = Value;
	return TRUE;
}
void Release_Memory (void) {
	if (OrigMem != NULL) { VirtualFree(OrigMem,0,MEM_RELEASE); }
	if (ROM != NULL) { 	VirtualFree(ROM,0 ,MEM_RELEASE); }
	VirtualFree(TLB_ReadMap,0 ,MEM_RELEASE);
	VirtualFree(TLB_WriteMap,0 ,MEM_RELEASE);
	VirtualFree(N64MEM,0 ,MEM_RELEASE);
	VirtualFree(DelaySlotTable,0 ,MEM_RELEASE);
	VirtualFree(JumpTable,0 ,MEM_RELEASE);
	VirtualFree(RecompCode,0 ,MEM_RELEASE);
}
void ResetMemoryStack (BLOCK_SECTION * Section) {
	int x86reg,TempReg;
	x86reg = Map_MemoryStack(Section,FALSE);
	if (x86reg >= 0) { UnMap_X86reg(Section,x86reg); }
	x86reg = Map_TempReg(Section,x86_Any,29,FALSE);
	if (UseTLB) {
		TempReg = Map_TempReg(Section,x86_Any,-1,FALSE);
		MoveX86RegToX86Reg(x86reg,TempReg);
		ShiftRightUnsignImmed(TempReg,12);
		MoveVariableDispToX86Reg(TLB_ReadMap,TempReg,TempReg,4);
		AddX86RegToX86Reg(x86reg,TempReg);
	} else {
		AndConstToX86Reg(x86reg,0x1FFFFFFF);
		AddConstToX86Reg(x86reg,(DWORD)N64MEM);
	}
	MoveX86regToVariable(x86reg,&MemoryStack);
}
void ResetRecompCode (void) {
	DWORD count,OldProtect;
	RecompPos = RecompCode;
	TargetIndex = 0;
	//Jump Table
	for (count = 0; count < (RDRAMsize >> 12); count ++) {
		if (N64_Blocks.NoOfRDRAMBlocks[count] > 0) {
			N64_Blocks.NoOfRDRAMBlocks[count] = 0;
			memset(JumpTable + (count << 10),0,0x1000);
			*(DelaySlotTable + count) = NULL;
			VirtualProtect((N64MEM + (count << 12)),4,PAGE_READWRITE,&OldProtect);
		}
	}
	if (N64_Blocks.NoOfDMEMBlocks > 0) {
		N64_Blocks.NoOfDMEMBlocks = 0;
		memset(JumpTable + (0x04000000 >> 2),0,0x1000);
		*(DelaySlotTable + (0x04000000 >> 12)) = NULL;
		VirtualProtect((N64MEM + 0x04000000),4,PAGE_READWRITE,&OldProtect);
	}
	if (N64_Blocks.NoOfIMEMBlocks > 0) {
		N64_Blocks.NoOfIMEMBlocks = 0;
		memset(JumpTable + (0x04001000 >> 2),0,0x1000);
		*(DelaySlotTable + (0x04001000 >> 12)) = NULL;
		VirtualProtect((N64MEM + 0x04001000),4,PAGE_READWRITE,&OldProtect);
	}
}