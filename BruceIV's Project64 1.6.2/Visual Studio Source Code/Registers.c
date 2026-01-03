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
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
#include "x86.h"
#include "ROM Tools Common.h"
DWORD PROGRAM_COUNTER,* CP0,*FPCR,*RegRDRAM,*RegSP,*RegDPC,*RegMI,*RegVI,*RegAI,*RegPI,
	*RegRI,*RegSI,HalfLine,RegModValue,ViFieldSerration,LLBit,LLAddr;
void * FPRDoubleLocation[32],* FPRFloatLocation[32];
MIPS_DWORD *GPR,*FPR,HI,LO;
N64_REGISTERS Registers;
int fpuControl;
int  UnMap_8BitTempReg (BLOCK_SECTION * Section);
int  UnMap_TempReg     (BLOCK_SECTION * Section);
BOOL UnMap_X86reg      (BLOCK_SECTION * Section,DWORD x86Reg);
void ChangeFPURegFormat (BLOCK_SECTION * Section,int Reg,int OldFormat,int NewFormat,int RoundingModel) {
	DWORD i;
	for (i = 0; i < 8; i++) {
		if (FpuMappedTo(i) == (DWORD)Reg) {
			if (FpuState(i) != (DWORD)OldFormat) {
				UnMap_FPR(Section,Reg,TRUE);
				Load_FPR_ToTop(Section,Reg,Reg,OldFormat);
				ChangeFPURegFormat(Section,Reg,OldFormat,NewFormat,RoundingModel);
			} else {
				FpuRoundingModel(i) = RoundingModel;
				FpuState(i) = NewFormat;
			}
			return;
		}
	}
}
void ChangeMiIntrMask (void) {
	if ((RegModValue & MI_INTR_MASK_CLR_SP) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_SP; }
	if ((RegModValue & MI_INTR_MASK_SET_SP) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_SP; }
	if ((RegModValue & MI_INTR_MASK_CLR_SI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_SI; }
	if ((RegModValue & MI_INTR_MASK_SET_SI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_SI; }
	if ((RegModValue & MI_INTR_MASK_CLR_AI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_AI; }
	if ((RegModValue & MI_INTR_MASK_SET_AI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_AI; }
	if ((RegModValue & MI_INTR_MASK_CLR_VI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_VI; }
	if ((RegModValue & MI_INTR_MASK_SET_VI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_VI; }
	if ((RegModValue & MI_INTR_MASK_CLR_PI) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_PI; }
	if ((RegModValue & MI_INTR_MASK_SET_PI) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_PI; }
	if ((RegModValue & MI_INTR_MASK_CLR_DP) != 0) { MI_INTR_MASK_REG &= ~MI_INTR_MASK_DP; }
	if ((RegModValue & MI_INTR_MASK_SET_DP) != 0) { MI_INTR_MASK_REG |= MI_INTR_MASK_DP; }
}
#ifndef MIN_SIZE
void ChangeMiModeReg(void) {
	MI_MODE_REG &= ~0x7F;
	MI_MODE_REG |= (RegModValue & 0x7F);
	if ((RegModValue & MI_CLR_INIT) != 0) { MI_MODE_REG &= ~MI_MODE_INIT; }
	if ((RegModValue & MI_SET_INIT) != 0) { MI_MODE_REG |= MI_MODE_INIT; }
	if ((RegModValue & MI_CLR_EBUS) != 0) { MI_MODE_REG &= ~MI_MODE_EBUS; }
	if ((RegModValue & MI_SET_EBUS) != 0) { MI_MODE_REG |= MI_MODE_EBUS; }
	if ((RegModValue & MI_CLR_DP_INTR) != 0) { MI_INTR_REG &= ~MI_INTR_DP; }
	if ((RegModValue & MI_CLR_RDRAM) != 0) { MI_MODE_REG &= ~MI_MODE_RDRAM; }
	if ((RegModValue & MI_SET_RDRAM) != 0) { MI_MODE_REG |= MI_MODE_RDRAM; }
}
void ChangeDpcStatus(void) {
	if ((RegModValue & DPC_CLR_XBUS_DMEM_DMA) != 0) { DPC_STATUS_REG &= ~DPC_STATUS_XBUS_DMEM_DMA; }
	if ((RegModValue & DPC_SET_XBUS_DMEM_DMA) != 0) { DPC_STATUS_REG |= DPC_STATUS_XBUS_DMEM_DMA;  }
	if ((RegModValue & DPC_CLR_FREEZE) != 0) { DPC_STATUS_REG &= ~DPC_STATUS_FREEZE; }
	if ((RegModValue & DPC_SET_FREEZE) != 0) { DPC_STATUS_REG |= DPC_STATUS_FREEZE;  }
	if ((RegModValue & DPC_CLR_FLUSH) != 0) { DPC_STATUS_REG &= ~DPC_STATUS_FLUSH; }
	if ((RegModValue & DPC_SET_FLUSH) != 0) { DPC_STATUS_REG |= DPC_STATUS_FLUSH;  }
	if ((RegModValue & DPC_CLR_FREEZE) != 0)
	{
		if ((SP_STATUS_REG & SP_STATUS_HALT) == 0)
		{
			if ((SP_STATUS_REG & SP_STATUS_BROKE) == 0)
			{
				RunRsp();
			}
		}
	}
}
#endif
void ChangeSpStatus (void) {
	if ((RegModValue & SP_CLR_HALT) != 0) { SP_STATUS_REG &= ~SP_STATUS_HALT; }
	if ((RegModValue & SP_SET_HALT) != 0) { SP_STATUS_REG |= SP_STATUS_HALT;  }
	if ((RegModValue & SP_CLR_BROKE) != 0) { SP_STATUS_REG &= ~SP_STATUS_BROKE; }
	if ((RegModValue & SP_CLR_INTR) != 0) {
		MI_INTR_REG &= ~MI_INTR_SP;
		CheckInterrupts();
	}
	if ((RegModValue & SP_CLR_SSTEP) != 0) { SP_STATUS_REG &= ~SP_STATUS_SSTEP; }
	if ((RegModValue & SP_SET_SSTEP) != 0) { SP_STATUS_REG |= SP_STATUS_SSTEP;  }
	if ((RegModValue & SP_CLR_INTR_BREAK) != 0) { SP_STATUS_REG &= ~SP_STATUS_INTR_BREAK; }
	if ((RegModValue & SP_SET_INTR_BREAK) != 0) { SP_STATUS_REG |= SP_STATUS_INTR_BREAK;  }
	if ((RegModValue & SP_CLR_SIG0) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG0; }
	if ((RegModValue & SP_SET_SIG0) != 0) {
		SP_STATUS_REG |= SP_STATUS_SIG0;
		if (AudioSignal) {
			MI_INTR_REG |= MI_INTR_SP;
			CheckInterrupts();
		}
	}
	if ((RegModValue & SP_CLR_SIG1) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG1; }
	if ((RegModValue & SP_SET_SIG1) != 0) { SP_STATUS_REG |= SP_STATUS_SIG1;  }
	if ((RegModValue & SP_CLR_SIG2) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG2; }
	if ((RegModValue & SP_SET_SIG2) != 0) { SP_STATUS_REG |= SP_STATUS_SIG2;  }
	if ((RegModValue & SP_CLR_SIG3) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG3; }
	if ((RegModValue & SP_SET_SIG3) != 0) { SP_STATUS_REG |= SP_STATUS_SIG3;  }
	if ((RegModValue & SP_CLR_SIG4) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG4; }
	if ((RegModValue & SP_SET_SIG4) != 0) { SP_STATUS_REG |= SP_STATUS_SIG4;  }
	if ((RegModValue & SP_CLR_SIG5) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG5; }
	if ((RegModValue & SP_SET_SIG5) != 0) { SP_STATUS_REG |= SP_STATUS_SIG5;  }
	if ((RegModValue & SP_CLR_SIG6) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG6; }
	if ((RegModValue & SP_SET_SIG6) != 0) { SP_STATUS_REG |= SP_STATUS_SIG6;  }
	if ((RegModValue & SP_CLR_SIG7) != 0) { SP_STATUS_REG &= ~SP_STATUS_SIG7; }
	if ((RegModValue & SP_SET_SIG7) != 0) { SP_STATUS_REG |= SP_STATUS_SIG7;  }
	if (DelayRDP && *(DWORD *)(DMEM + 0xFC0) == 1 || DelayRSP && *(DWORD *)(DMEM + 0xFC0) == 2) {
		ChangeTimer(RspTimer,0x2000);
		return;
	}
	RunRsp();
}
int Free8BitX86Reg (BLOCK_SECTION * Section) {
	int x86Reg,count,MapCount[10],MapReg[10];
	if (x86Mapped(x86_EBX) == NotMapped && !x86Protected(x86_EBX)) {return x86_EBX; }
	if (x86Mapped(x86_EAX) == NotMapped && !x86Protected(x86_EAX)) {return x86_EAX; }
	if (x86Mapped(x86_EDX) == NotMapped && !x86Protected(x86_EDX)) {return x86_EDX; }
	if (x86Mapped(x86_ECX) == NotMapped && !x86Protected(x86_ECX)) {return x86_ECX; }
	x86Reg = UnMap_8BitTempReg(Section);
	if (x86Reg > 0) { return x86Reg; }
	for (count = 0; count < 10; count ++) {
		MapCount[count] = x86MapOrder(count);
		MapReg[count] = count;
	}
	for (count = 0; count < 10; count ++) {
		int i;
		for (i = 0; i < 9; i ++) {
			int temp;
			if (MapCount[i] < MapCount[i+1]) {
				temp = MapCount[i];
				MapCount[i] = MapCount[i+1];
				MapCount[i+1] = temp;
				temp = MapReg[i];
				MapReg[i] = MapReg[i+1];
				MapReg[i+1] = temp;
			}
		}
	}
	for (count = 0; count < 10; count ++) {
		if (MapCount[count] > 0) {
			if (!Is8BitReg(count)) { continue; }
			if (UnMap_X86reg(Section,count)) {
				return count;
			}
		}
	}
	return -1;
}
int FreeX86Reg (BLOCK_SECTION * Section) {
	int x86Reg,count,MapCount[10],MapReg[10],StackReg;
	if (x86Mapped(x86_EDI) == NotMapped && !x86Protected(x86_EDI)) {return x86_EDI; }
	if (x86Mapped(x86_ESI) == NotMapped && !x86Protected(x86_ESI)) {return x86_ESI; }
	if (x86Mapped(x86_EBX) == NotMapped && !x86Protected(x86_EBX)) {return x86_EBX; }
	if (x86Mapped(x86_EAX) == NotMapped && !x86Protected(x86_EAX)) {return x86_EAX; }
	if (x86Mapped(x86_EDX) == NotMapped && !x86Protected(x86_EDX)) {return x86_EDX; }
	if (x86Mapped(x86_ECX) == NotMapped && !x86Protected(x86_ECX)) {return x86_ECX; }
	x86Reg = UnMap_TempReg(Section);
	if (x86Reg > 0) { return x86Reg; }
	for (count = 0; count < 10; count ++) {
		MapCount[count] = x86MapOrder(count);
		MapReg[count] = count;
	}
	for (count = 0; count < 10; count ++) {
		int i;
		for (i = 0; i < 9; i ++) {
			int temp;
			if (MapCount[i] < MapCount[i+1]) {
				temp = MapCount[i];
				MapCount[i] = MapCount[i+1];
				MapCount[i+1] = temp;
				temp = MapReg[i];
				MapReg[i] = MapReg[i+1];
				MapReg[i+1] = temp;
			}
		}
	}
	StackReg = -1;
	for (count = 0; count < 10; count ++) {
		if (MapCount[count] > 0 && x86Mapped(MapReg[count]) != Stack_Mapped) {
			if (UnMap_X86reg(Section,MapReg[count])) {
				return MapReg[count];
			}
		}
		if (x86Mapped(MapReg[count]) == Stack_Mapped) { StackReg = MapReg[count]; }
	}
	if (StackReg > 0) {
		UnMap_X86reg(Section,StackReg);
		return StackReg;
	}
	return -1;
}
void InitializeR4300iRegisters (int UsePif,int Country,int CIC_Chip) {
	memset(CP0,0,sizeof(Registers.CP0));
	memset(FPCR,0,sizeof(Registers.FPCR));
	memset(RegRDRAM,0,sizeof(Registers.RDRAM));
	memset(RegSP,0,sizeof(Registers.SP));
	memset(RegDPC,0,sizeof(Registers.DPC));
	memset(RegMI,0,sizeof(Registers.MI));
	memset(RegVI,0,sizeof(Registers.VI));
	memset(RegAI,0,sizeof(Registers.AI));
	memset(RegPI,0,sizeof(Registers.PI));
	memset(RegRI,0,sizeof(Registers.RI));
	memset(RegSI,0,sizeof(Registers.SI));
	memset(GPR,0,sizeof(Registers.GPR));
	memset(FPR,0,sizeof(Registers.FPR));
	if (CIC_Chip < 0) CIC_Chip = 1;
	LO.DW                 = 0x0;
	HI.DW                 = 0x0;
	RANDOM_REGISTER	  = 0x1F;
	COUNT_REGISTER	  = 0x5000;
	MI_VERSION_REG	  = 0x02020102;
	SP_STATUS_REG      = 0x00000001;
	CAUSE_REGISTER	  = 0x0000005C;
	CONTEXT_REGISTER   = 0x007FFFF0;
	EPC_REGISTER       = 0xFFFFFFFF;
	BAD_VADDR_REGISTER = 0xFFFFFFFF;
	ERROREPC_REGISTER  = 0xFFFFFFFF;
	CONFIG_REGISTER     = 0x0006E463;
	REVISION_REGISTER   = 0x00000511;
	STATUS_REGISTER     = 0x34000000;
	SetFpuLocations();
	if (UsePif) {
		PROGRAM_COUNTER = 0xBFC00000;
		switch (CIC_Chip) {
		case 1:
			PIF_Ram[36] = 0x00;
			PIF_Ram[37] = 0x06;
			PIF_Ram[38] = 0x3F;
			PIF_Ram[39] = 0x3F;
			break;
		case 2:
			PIF_Ram[36] = 0x00;
			PIF_Ram[37] = 0x02;
			PIF_Ram[38] = 0x3F;
			PIF_Ram[39] = 0x3F;
			break;
		case 3:
			PIF_Ram[36] = 0x00;
			PIF_Ram[37] = 0x02;
			PIF_Ram[38] = 0x78;
			PIF_Ram[39] = 0x3F;
			break;
		case 5:
			PIF_Ram[36] = 0x00;
			PIF_Ram[37] = 0x02;
			PIF_Ram[38] = 0x91;
			PIF_Ram[39] = 0x3F;
			break;
		case 6:
			PIF_Ram[36] = 0x00;
			PIF_Ram[37] = 0x02;
			PIF_Ram[38] = 0x85;
			PIF_Ram[39] = 0x3F;
		}
	} else {
		memcpy((N64MEM+0x4000040),(ROM + 0x040),0xFBC);
		PROGRAM_COUNTER = 0xA4000040;
		GPR[0].DW=0x0000000000000000;
		GPR[6].DW=0xFFFFFFFFA4001F0C;
		GPR[7].DW=0xFFFFFFFFA4001F08;
		GPR[8].DW=0x00000000000000C0;
		GPR[9].DW=0x0000000000000000;
		GPR[10].DW=0x0000000000000040;
		GPR[11].DW=0xFFFFFFFFA4000040;
		GPR[16].DW=0x0000000000000000;
		GPR[17].DW=0x0000000000000000;
		GPR[18].DW=0x0000000000000000;
		GPR[19].DW=0x0000000000000000;
		GPR[21].DW=0x0000000000000000;
		GPR[26].DW=0x0000000000000000;
		GPR[27].DW=0x0000000000000000;
		GPR[28].DW=0x0000000000000000;
		GPR[29].DW=0xFFFFFFFFA4001FF0;
		GPR[30].DW=0x0000000000000000;
		switch (RomRegion(Country)) {
		case PAL_Region:
			switch (CIC_Chip) {
			case 2:
				GPR[5].DW=0xFFFFFFFFC0F1D859;
				GPR[14].DW=0x000000002DE108EA;
				GPR[24].DW=0x0000000000000000;
				break;
			case 3:
				GPR[5].DW=0xFFFFFFFFD4646273;
				GPR[14].DW=0x000000001AF99984;
				GPR[24].DW=0x0000000000000000;
				break;
			case 5:
				*(DWORD *)&IMEM[0x04] = 0xBDA807FC;
				GPR[5].DW=0xFFFFFFFFDECAAAD1;
				GPR[14].DW=0x000000000CF85C13;
				GPR[24].DW=0x0000000000000002;
				break;
			case 6:
				GPR[5].DW=0xFFFFFFFFB04DC903;
				GPR[14].DW=0x000000001AF99984;
				GPR[24].DW=0x0000000000000002;
				break;
			}
			GPR[20].DW=0x0000000000000000;
			GPR[23].DW=0x0000000000000006;
			GPR[31].DW=0xFFFFFFFFA4001554;
			break;
		default:
			switch (CIC_Chip) {
			case 2:
				GPR[5].DW=0xFFFFFFFFC95973D5;
				GPR[14].DW=0x000000002449A366;
				break;
			case 3:
				GPR[5].DW=0xFFFFFFFF95315A28;
				GPR[14].DW=0x000000005BACA1DF;
				break;
			case 5:
				*(DWORD *)&IMEM[0x04] = 0x8DA807FC;
				GPR[5].DW=0x000000005493FB9A;
				GPR[14].DW=0xFFFFFFFFC2C20384;
			case 6:
				GPR[5].DW=0xFFFFFFFFE067221F;
				GPR[14].DW=0x000000005CD2B70F;
				break;
			}
			GPR[20].DW=0x0000000000000001;
			GPR[23].DW=0x0000000000000000;
			GPR[24].DW=0x0000000000000003;
			GPR[31].DW=0xFFFFFFFFA4001550;
		}
		switch (CIC_Chip) {
		case 1:
			GPR[22].DW=0x000000000000003F;
			break;
		case 2:
			GPR[1].DW=0x0000000000000001;
			GPR[2].DW=0x000000000EBDA536;
			GPR[3].DW=0x000000000EBDA536;
			GPR[4].DW=0x000000000000A536;
			GPR[12].DW=0xFFFFFFFFED10D0B3;
			GPR[13].DW=0x000000001402A4CC;
			GPR[15].DW=0x000000003103E121;
			GPR[22].DW=0x000000000000003F;
			GPR[25].DW=0xFFFFFFFF9DEBB54F;
			break;
		case 3:
			GPR[1].DW=0x0000000000000001;
			GPR[2].DW=0x0000000049A5EE96;
			GPR[3].DW=0x0000000049A5EE96;
			GPR[4].DW=0x000000000000EE96;
			GPR[12].DW=0xFFFFFFFFCE9DFBF7;
			GPR[13].DW=0xFFFFFFFFCE9DFBF7;
			GPR[15].DW=0x0000000018B63D28;
			GPR[22].DW=0x0000000000000078;
			GPR[25].DW=0xFFFFFFFF825B21C9;
			break;
		case 5:
			*(DWORD *)&IMEM[0x00] = 0x3C0DBFC0;
			*(DWORD *)&IMEM[0x08] = 0x25AD07C0;
			*(DWORD *)&IMEM[0x0C] = 0x31080080;
			*(DWORD *)&IMEM[0x10] = 0x5500FFFC;
			*(DWORD *)&IMEM[0x14] = 0x3C0DBFC0;
			*(DWORD *)&IMEM[0x18] = 0x8DA80024;
			*(DWORD *)&IMEM[0x1C] = 0x3C0BB000;
			GPR[1].DW=0x0000000000000000;
			GPR[2].DW=0xFFFFFFFFF58B0FBF;
			GPR[3].DW=0xFFFFFFFFF58B0FBF;
			GPR[4].DW=0x0000000000000FBF;
			GPR[12].DW=0xFFFFFFFF9651F81E;
			GPR[13].DW=0x000000002D42AAC5;
			GPR[15].DW=0x0000000056584D60;
			GPR[22].DW=0x0000000000000091;
			GPR[25].DW=0xFFFFFFFFCDCE565F;
			break;
		case 6:
			GPR[1].DW=0x0000000000000000;
			GPR[2].DW=0xFFFFFFFFA95930A4;
			GPR[3].DW=0xFFFFFFFFA95930A4;
			GPR[4].DW=0x00000000000030A4;
			GPR[12].DW=0xFFFFFFFFBCB59510;
			GPR[13].DW=0xFFFFFFFFBCB59510;
			GPR[15].DW=0x000000007A3C07F4;
			GPR[22].DW=0x0000000000000085;
			GPR[25].DW=0x00000000465E3F72;
			break;
		case 7:
		case 8:
			GPR[22].DW = 0x00000000000000DD;
			break;
		case 9:
			GPR[22].DW = 0x00000000000000DE;
		}
	}
	MemoryStack = (DWORD)(N64MEM+(GPR[29].W[0] & 0x1FFFFFFF));
}
BOOL Is8BitReg (int x86Reg) {
	if (x86Reg == x86_EAX) return TRUE;
	if (x86Reg == x86_EBX) return TRUE;
	if (x86Reg == x86_ECX) return TRUE;
	if (x86Reg == x86_EDX) return TRUE;
	return FALSE;
}
void Load_FPR_ToTop (BLOCK_SECTION * Section,int Reg,int RegToLoad,int Format) {
	int i;
	if (RegToLoad < 0 || Reg < 0) return;
	if (Format == FPU_Double || Format == FPU_Qword) {
		UnMap_FPR(Section,Reg + 1,TRUE);
		UnMap_FPR(Section,RegToLoad + 1,TRUE);
	} else {
		if ((Reg & 1) != 0) {
			for (i = 0; i < 8; i++) {
				if (FpuMappedTo(i) == (DWORD)(Reg - 1)) {
					if (FpuState(i) == FPU_Double || FpuState(i) == FPU_Qword) {
						UnMap_FPR(Section,Reg,TRUE);
					}
					i = 8;
				}
			}
		}
		if ((RegToLoad & 1) != 0) {
			for (i = 0; i < 8; i++) {
				if (FpuMappedTo(i) == (DWORD)(RegToLoad - 1)) {
					if (FpuState(i) == FPU_Double || FpuState(i) == FPU_Qword) {
						UnMap_FPR(Section,RegToLoad,TRUE);
					}
					i = 8;
				}
			}
		}
	}
	if (Reg == RegToLoad) {
		//if different format then unmap original reg from stack
		for (i = 0; i < 8; i++) {
			if (FpuMappedTo(i) == (DWORD)Reg) {
				if (FpuState(i) != (DWORD)Format) {
					UnMap_FPR(Section,Reg,TRUE);
				}
				i = 8;
			}
		}
	} else {
		UnMap_FPR(Section,Reg,FALSE);
	}
	if (RegInStack(Section,RegToLoad,Format)) {
		if (Reg != RegToLoad) {
			if (FpuMappedTo((StackTopPos - 1) & 7) != (DWORD)RegToLoad) {
				UnMap_FPR(Section,FpuMappedTo((StackTopPos - 1) & 7),TRUE);
				fpuLoadReg(&StackTopPos,StackPosition(Section,RegToLoad));
				FpuRoundingModel(StackTopPos) = RoundDefault;
				FpuMappedTo(StackTopPos)      = Reg;
				FpuState(StackTopPos)         = Format;
			} else {
				UnMap_FPR(Section,FpuMappedTo((StackTopPos - 1) & 7),TRUE);
				Load_FPR_ToTop (Section,Reg,RegToLoad,Format);
			}
		} else {
			DWORD RegPos,StackPos,i;
			for (i = 0; i < 8; i++) {
				if (FpuMappedTo(i) == (DWORD)Reg) {
					RegPos = i;
					i = 8;
				}
			}
			if (RegPos == StackTopPos) return;
			StackPos = StackPosition(Section,Reg);
			FpuRoundingModel(RegPos) = FpuRoundingModel(StackTopPos);
			FpuMappedTo(RegPos)      = FpuMappedTo(StackTopPos);
			FpuState(RegPos)         = FpuState(StackTopPos);
			fpuExchange(StackPos);
			FpuRoundingModel(StackTopPos) = RoundDefault;
			FpuMappedTo(StackTopPos)      = Reg;
			FpuState(StackTopPos)         = Format;
		}
	} else {
		int TempReg;
		UnMap_FPR(Section,FpuMappedTo((StackTopPos - 1) & 7),TRUE);
		for (i = 0; i < 8; i++) {
			if (FpuMappedTo(i) == (DWORD)RegToLoad) {
				UnMap_FPR(Section,RegToLoad,TRUE);
				i = 8;
			}
		}
		TempReg = Map_TempReg(Section,x86_Any,-1,FALSE);
		switch (Format) {
		case FPU_Dword:
			MoveVariableToX86reg(&FPRFloatLocation[RegToLoad],TempReg);
			fpuLoadIntegerDwordFromX86Reg(&StackTopPos,TempReg);
			break;
		case FPU_Qword:
			MoveVariableToX86reg(&FPRDoubleLocation[RegToLoad],TempReg);
			fpuLoadIntegerQwordFromX86Reg(&StackTopPos,TempReg);
			break;
		case FPU_Float:
			MoveVariableToX86reg(&FPRFloatLocation[RegToLoad],TempReg);
			fpuLoadDwordFromX86Reg(&StackTopPos,TempReg);
			break;
		case FPU_Double:
			MoveVariableToX86reg(&FPRDoubleLocation[RegToLoad],TempReg);
			fpuLoadQwordFromX86Reg(&StackTopPos,TempReg);
		}
		x86Protected(TempReg) = FALSE;
		FpuRoundingModel(StackTopPos) = RoundDefault;
		FpuMappedTo(StackTopPos)      = Reg;
		FpuState(StackTopPos)         = Format;
	}
}
void Map_GPR_32bit (BLOCK_SECTION * Section,int Reg,BOOL SignValue,int MipsRegToLoad) {
	int x86Reg,count;
	if (Reg == 0) return;
	if (IsUnknown(Reg) || IsConst(Reg)) {
		x86Reg = FreeX86Reg(Section);
		if (x86Reg < 0) return;
	} else {
		if (Is64Bit(Reg)) {
			x86MapOrder(MipsRegHi(Reg)) = 0;
			x86Mapped(MipsRegHi(Reg)) = NotMapped;
			x86Protected(MipsRegHi(Reg)) = FALSE;
			MipsRegHi(Reg) = 0;
		}
		x86Reg = MipsRegLo(Reg);
	}
	for (count = 0; count < 10; count ++) {
		if (x86MapOrder(count) > 0) {
			x86MapOrder(count) += 1;
		}
	}
	x86MapOrder(x86Reg) = 1;
	if (MipsRegToLoad > 0) {
		if (IsUnknown(MipsRegToLoad)) {
			MoveVariableToX86reg(&GPR[MipsRegToLoad].UW[0],x86Reg);
		} else if (IsMapped(MipsRegToLoad)) {
			if (Reg != MipsRegToLoad) {
				MoveX86RegToX86Reg(MipsRegLo(MipsRegToLoad),x86Reg);
			}
		} else {
			if (MipsRegLo(MipsRegToLoad) != 0) {
				MoveConstToX86reg(MipsRegLo(MipsRegToLoad),x86Reg);
			} else {
				XorX86RegToX86Reg(x86Reg,x86Reg);
			}
		}
	} else if (MipsRegToLoad == 0) {
		XorX86RegToX86Reg(x86Reg,x86Reg);
	}
	x86Mapped(x86Reg) = GPR_Mapped;
	x86Protected(x86Reg) = TRUE;
	MipsRegLo(Reg) = x86Reg;
	MipsRegState(Reg) = SignValue ? STATE_MAPPED_32_SIGN : STATE_MAPPED_32_ZERO;
}
void Map_GPR_64bit (BLOCK_SECTION * Section,int Reg,int MipsRegToLoad) {
	int x86Hi,x86lo,count;
	if (Reg == 0) return;
	ProtectGPR(Section,Reg);
	if (IsUnknown(Reg) || IsConst(Reg)) {
		x86Hi = FreeX86Reg(Section);
		if (x86Hi < 0) return;
		x86Protected(x86Hi) = TRUE;
		x86lo = FreeX86Reg(Section);
		if (x86lo < 0) return;
		x86Protected(x86lo) = TRUE;
	} else {
		x86lo = MipsRegLo(Reg);
		if (Is32Bit(Reg)) {
			x86Protected(x86lo) = TRUE;
			x86Hi = FreeX86Reg(Section);
			if (x86Hi < 0) return;
			x86Protected(x86Hi) = TRUE;
		} else {
			x86Hi = MipsRegHi(Reg);
		}
	}
	for (count = 0; count < 10; count ++) {
		if (x86MapOrder(count) > 0) { x86MapOrder(count) += 1; }
	}
	x86MapOrder(x86Hi) = 1;
	x86MapOrder(x86lo) = 1;
	if (MipsRegToLoad > 0) {
		if (IsUnknown(MipsRegToLoad)) {
			MoveVariableToX86reg(&GPR[MipsRegToLoad].UW[1],x86Hi);
			MoveVariableToX86reg(&GPR[MipsRegToLoad].UW[0],x86lo);
		} else if (IsMapped(MipsRegToLoad)) {
			if (Is32Bit(MipsRegToLoad)) {
				if (IsSigned(MipsRegToLoad)) {
					MoveX86RegToX86Reg(MipsRegLo(MipsRegToLoad),x86Hi);
					ShiftRightSignImmed(x86Hi,31);
				} else {
					XorX86RegToX86Reg(x86Hi,x86Hi);
				}
				if (Reg != MipsRegToLoad) {
					MoveX86RegToX86Reg(MipsRegLo(MipsRegToLoad),x86lo);
				}
			} else {
				if (Reg != MipsRegToLoad) {
					MoveX86RegToX86Reg(MipsRegHi(MipsRegToLoad),x86Hi);
					MoveX86RegToX86Reg(MipsRegLo(MipsRegToLoad),x86lo);
				}
			}
		} else {
			if (Is32Bit(MipsRegToLoad)) {
				if (IsSigned(MipsRegToLoad)) {
					if (MipsRegLo((int)MipsRegLo(MipsRegToLoad) >> 31) != 0) {
						MoveConstToX86reg((int)MipsRegLo(MipsRegToLoad) >> 31,x86Hi);
					} else {
						XorX86RegToX86Reg(x86Hi,x86Hi);
					}
				} else {
					XorX86RegToX86Reg(x86Hi,x86Hi);
				}
			} else {
				if (MipsRegHi(MipsRegToLoad) != 0) {
					MoveConstToX86reg(MipsRegHi(MipsRegToLoad),x86Hi);
				} else {
					XorX86RegToX86Reg(x86Hi,x86Hi);
				}
			}
			if (MipsRegLo(MipsRegToLoad) != 0) {
				MoveConstToX86reg(MipsRegLo(MipsRegToLoad),x86lo);
			} else {
				XorX86RegToX86Reg(x86lo,x86lo);
			}
		}
	} else if (MipsRegToLoad == 0) {
		XorX86RegToX86Reg(x86Hi,x86Hi);
		XorX86RegToX86Reg(x86lo,x86lo);
	}
	x86Mapped(x86Hi) = GPR_Mapped;
	x86Mapped(x86lo) = GPR_Mapped;
	MipsRegHi(Reg) = x86Hi;
	MipsRegLo(Reg) = x86lo;
	MipsRegState(Reg) = STATE_MAPPED_64;
}
int Map_MemoryStack (BLOCK_SECTION * Section,BOOL AutoMap) {
	int x86Reg;
	for (x86Reg = 0; x86Reg < 10; x86Reg ++) {
		if (x86Mapped(x86Reg) == Stack_Mapped) {
			return x86Reg;
		}
	}
	if (!AutoMap) { return -1; }
	x86Reg = FreeX86Reg(Section);
	x86Mapped(x86Reg) = Stack_Mapped;
	MoveVariableToX86reg(&MemoryStack,x86Reg);
	return x86Reg;
}
int Map_TempReg (BLOCK_SECTION * Section,int x86Reg,int MipsReg,BOOL LoadHiWord) {
	int count;
	if (x86Reg == x86_Any) {
		for (count = 0; count < 10; count ++) {
			if (x86Mapped(count) == Temp_Mapped) {
				if (x86Protected(count) == FALSE) { x86Reg = count; }
			}
		}
		if (x86Reg == x86_Any) {
			x86Reg = FreeX86Reg(Section);
			if (x86Reg < 0) {
				x86Reg = FreeX86Reg(Section);
				return -1;
			}
		}
	} else if (x86Reg == x86_Any8Bit) {
		if (x86Mapped(x86_EBX) == Temp_Mapped && !x86Protected(x86_EBX)) { x86Reg = x86_EBX; }
		if (x86Mapped(x86_EAX) == Temp_Mapped && !x86Protected(x86_EAX)) { x86Reg = x86_EAX; }
		if (x86Mapped(x86_EDX) == Temp_Mapped && !x86Protected(x86_EDX)) { x86Reg = x86_EDX; }
		if (x86Mapped(x86_ECX) == Temp_Mapped && !x86Protected(x86_ECX)) { x86Reg = x86_ECX; }
		if (x86Reg == x86_Any8Bit) {
			x86Reg = Free8BitX86Reg(Section);
			if (x86Reg < 0) {
				return -1;
			}
		}
	} else {
		int NewReg;
		if (x86Mapped(x86Reg) == GPR_Mapped) {
			if (x86Protected(x86Reg) == TRUE) {
				return -1;
			}
			x86Protected(x86Reg) = TRUE;
			NewReg = FreeX86Reg(Section);
			for (count = 1; count < 32; count ++) {
				if (IsMapped(count)) {
					if (MipsRegLo(count) == (DWORD)x86Reg) {
						if (NewReg < 0) {
							UnMap_GPR(Section,count,TRUE);
							count = 32;
							continue;
						}
						x86Mapped(NewReg) = GPR_Mapped;
						x86MapOrder(NewReg) = x86MapOrder(x86Reg);
						MipsRegLo(count) = NewReg;
						MoveX86RegToX86Reg(x86Reg,NewReg);
						if (MipsReg == count && LoadHiWord == FALSE) { MipsReg = -1; }
						count = 32;
					}
					if (Is64Bit(count) && MipsRegHi(count) == (DWORD)x86Reg) {
						if (NewReg < 0) {
							UnMap_GPR(Section,count,TRUE);
							count = 32;
							continue;
						}
						x86Mapped(NewReg) = GPR_Mapped;
						x86MapOrder(NewReg) = x86MapOrder(x86Reg);
						MipsRegHi(count) = NewReg;
						MoveX86RegToX86Reg(x86Reg,NewReg);
						if (MipsReg == count && LoadHiWord == TRUE) { MipsReg = -1; }
						count = 32;
					}
				}
			}
		}
		if (x86Mapped(x86Reg) == Stack_Mapped) {
			UnMap_X86reg(Section,x86Reg);
		}
	}
	if (MipsReg >= 0) {
		if (LoadHiWord) {
			if (IsUnknown(MipsReg)) {
				MoveVariableToX86reg(&GPR[MipsReg].UW[1],x86Reg);
			} else if (IsMapped(MipsReg)) {
				if (Is64Bit(MipsReg)) {
					MoveX86RegToX86Reg(MipsRegHi(MipsReg),x86Reg);
				} else if (IsSigned(MipsReg)) {
					MoveX86RegToX86Reg(MipsRegLo(MipsReg),x86Reg);
					ShiftRightSignImmed(x86Reg,31);
				} else {
					MoveConstToX86reg(0,x86Reg);
				}
			} else {
				if (Is64Bit(MipsReg)) {
					if (MipsRegHi(MipsReg) != 0) {
						MoveConstToX86reg(MipsRegHi(MipsReg),x86Reg);
					} else {
						XorX86RegToX86Reg(x86Reg,x86Reg);
					}
				} else {
					if ((int)MipsRegLo(MipsReg) >> 31 != 0) {
						MoveConstToX86reg((int)MipsRegLo(MipsReg) >> 31,x86Reg);
					} else {
						XorX86RegToX86Reg(x86Reg,x86Reg);
					}
				}
			}
		} else {
			if (IsUnknown(MipsReg)) {
				MoveVariableToX86reg(&GPR[MipsReg].UW[0],x86Reg);
			} else if (IsMapped(MipsReg)) {
				MoveX86RegToX86Reg(MipsRegLo(MipsReg),x86Reg);
			} else {
				if (MipsRegLo(MipsReg) != 0) {
					MoveConstToX86reg(MipsRegLo(MipsReg),x86Reg);
				} else {
					XorX86RegToX86Reg(x86Reg,x86Reg);
				}
			}
		}
	}
	x86Mapped(x86Reg) = Temp_Mapped;
	x86Protected(x86Reg) = TRUE;
	for (count = 0; count < 10; count ++) {
		if (x86MapOrder(count) > 0) {
			x86MapOrder(count) += 1;
		}
	}
	x86MapOrder(x86Reg) = 1;
	return x86Reg;
}
void ProtectGPR(BLOCK_SECTION * Section,DWORD Reg) {
	if (IsUnknown(Reg) || IsConst(Reg)) return;
	if (Is64Bit(Reg)) {
		x86Protected(MipsRegHi(Reg)) = TRUE;
	}
	x86Protected(MipsRegLo(Reg)) = TRUE;
}
BOOL RegInStack(BLOCK_SECTION * Section,int Reg,int Format) {
	int i;
	for (i = 0; i < 8; i++) {
		if (FpuMappedTo(i) == (DWORD)Reg) {
			if (FpuState(i) == (DWORD)Format) return TRUE;
			else if (Format == -1) return TRUE;
			return FALSE;
		}
	}
	return FALSE;
}
void SetFpuLocations (void) {
	int count;
	if ((STATUS_REGISTER & STATUS_FR) == 0) {
		for (count = 0; count < 32; count ++) {
			FPRFloatLocation[count] = (void *)(&FPR[count & ~1].W[count & 1]);
			FPRDoubleLocation[count] = (void *)(&FPR[count & ~1].DW);
		}
	} else {
		for (count = 0; count < 32; count ++) {
			FPRFloatLocation[count] = (void *)(&FPR[count].W[0]);
			FPRDoubleLocation[count] = (void *)(&FPR[count].DW);
		}
	}
}
void SetupRegisters(N64_REGISTERS * n64_Registers) {
	PROGRAM_COUNTER = n64_Registers->PROGRAM_COUNTER;
	HI.DW    = n64_Registers->HI.DW;
	LO.DW    = n64_Registers->LO.DW;
	CP0      = n64_Registers->CP0;
	GPR      = n64_Registers->GPR;
	FPR      = n64_Registers->FPR;
	FPCR     = n64_Registers->FPCR;
	RegRDRAM = n64_Registers->RDRAM;
	RegSP    = n64_Registers->SP;
	RegDPC   = n64_Registers->DPC;
	RegMI    = n64_Registers->MI;
	RegVI    = n64_Registers->VI;
	RegAI    = n64_Registers->AI;
	RegPI    = n64_Registers->PI;
	RegRI    = n64_Registers->RI;
	RegSI    = n64_Registers->SI;
	PIF_Ram  = n64_Registers->PIF_Ram;
	DMAUsed  = n64_Registers->DMAUsed;
}
int StackPosition (BLOCK_SECTION * Section,int Reg) {
	int i;
	for (i = 0; i < 8; i++) {
		if (FpuMappedTo(i) == (DWORD)Reg) {
			return ((i - StackTopPos) & 7);
		}
	}
	return -1;
}
int UnMap_8BitTempReg (BLOCK_SECTION * Section) {
	int count;
	for (count = 0; count < 10; count ++) {
		if (!Is8BitReg(count)) continue;
		if (MipsRegState(count) == Temp_Mapped) {
			if (x86Protected(count) == FALSE) {
				x86Mapped(count) = NotMapped;
				return count;
			}
		}
	}
	return -1;
}
void UnMap_AllFPRs (BLOCK_SECTION * Section) {
	DWORD StackPos;
	for (;;) {
		int i,StartPos;
		StackPos = StackTopPos;
		if (FpuMappedTo(StackTopPos) != -1) {
			UnMap_FPR(Section,FpuMappedTo(StackTopPos),TRUE);
			continue;
		}
		//see if any more registers mapped
		StartPos = StackTopPos;
		for (i = 0; i < 8; i++) {
			if (FpuMappedTo((StartPos + i) & 7) != -1) { fpuIncStack(&StackTopPos); }
		}
		if (StackPos != StackTopPos) continue;
		return;
	}
}
void UnMap_FPR (BLOCK_SECTION * Section,int Reg,int WriteBackValue) {
	int TempReg;
	int i;
	if (Reg < 0) return;
	for (i = 0; i < 8; i++) {
		if (FpuMappedTo(i) != (DWORD)Reg) continue;
		if (WriteBackValue) {
			int RegPos;
			if (((i - StackTopPos + 8) & 7) != 0) {
				DWORD RoundingModel,MappedTo,RegState;
				RoundingModel = FpuRoundingModel(StackTopPos);
				MappedTo      = FpuMappedTo(StackTopPos);
				RegState      = FpuState(StackTopPos);
				FpuRoundingModel(StackTopPos) = FpuRoundingModel(i);
				FpuMappedTo(StackTopPos)      = FpuMappedTo(i);
				FpuState(StackTopPos)         = FpuState(i);
				FpuRoundingModel(i) = RoundingModel;
				FpuMappedTo(i)      = MappedTo;
				FpuState(i)         = RegState;
				fpuExchange((i - StackTopPos) & 7);
			}
			if (CurrentRoundingModel != FpuRoundingModel(i)) {
				int x86reg;
				fpuControl = 0;
				fpuStoreControl(&fpuControl);
				x86reg = Map_TempReg(Section,x86_Any,-1,FALSE);
				MoveVariableToX86reg(&fpuControl,x86reg);
				AndConstToX86Reg(x86reg,0xF3FF);
				switch (FpuRoundingModel(i)) {
				case RoundDefault: OrVariableToX86Reg(&FPU_RoundingMode,x86reg); break;
				case RoundTruncate: OrConstToX86Reg(0x0C00,x86reg); break;
				case RoundDown: OrConstToX86Reg(0x0400,x86reg); break;
				case RoundUp: OrConstToX86Reg(0x0800,x86reg);
				}
				MoveX86regToVariable(x86reg,&fpuControl);
				fpuLoadControl(&fpuControl);
				CurrentRoundingModel = FpuRoundingModel(i);
			}
			RegPos = StackTopPos;
			TempReg = Map_TempReg(Section,x86_Any,-1,FALSE);
			switch (FpuState(StackTopPos)) {
			case FPU_Dword:
				MoveVariableToX86reg(&FPRFloatLocation[FpuMappedTo(StackTopPos)],TempReg);
				fpuStoreIntegerDwordFromX86Reg(&StackTopPos,TempReg,TRUE);
				break;
			case FPU_Qword:
				MoveVariableToX86reg(&FPRDoubleLocation[FpuMappedTo(StackTopPos)],TempReg);
				fpuStoreIntegerQwordFromX86Reg(&StackTopPos,TempReg,TRUE);
				break;
			case FPU_Float:
				MoveVariableToX86reg(&FPRFloatLocation[FpuMappedTo(StackTopPos)],TempReg);
				fpuStoreDwordFromX86Reg(&StackTopPos,TempReg,TRUE);
				break;
			case FPU_Double:
				MoveVariableToX86reg(&FPRDoubleLocation[FpuMappedTo(StackTopPos)],TempReg);
				fpuStoreQwordFromX86Reg(&StackTopPos,TempReg,TRUE);
			}
			x86Protected(TempReg) = FALSE;
			FpuRoundingModel(RegPos) = RoundDefault;
			FpuMappedTo(RegPos)      = -1;
			FpuState(RegPos)         = FPU_Unkown;
		} else {
			fpuFree((i - StackTopPos) & 7);
			FpuRoundingModel(i) = RoundDefault;
			FpuMappedTo(i)      = -1;
			FpuState(i)         = FPU_Unkown;
		}
		return;
	}
}
void UnMap_GPR (BLOCK_SECTION * Section,DWORD Reg,int WriteBackValue) {
	if (Reg == 0 || IsUnknown(Reg)) return;
	if (IsConst(Reg)) {
		if (!WriteBackValue) {
			MipsRegState(Reg) = STATE_UNKNOWN;
			return;
		}
		if (Is64Bit(Reg)) {
			MoveConstToVariable(MipsRegHi(Reg),&GPR[Reg].UW[1]);
			MoveConstToVariable(MipsRegLo(Reg),&GPR[Reg].UW[0]);
			MipsRegState(Reg) = STATE_UNKNOWN;
			return;
		}
		if ((MipsRegLo(Reg) & 0x80000000) != 0) {
			MoveConstToVariable(0xFFFFFFFF,&GPR[Reg].UW[1]);
		} else {
			MoveConstToVariable(0,&GPR[Reg].UW[1]);
		}
		MoveConstToVariable(MipsRegLo(Reg),&GPR[Reg].UW[0]);
		MipsRegState(Reg) = STATE_UNKNOWN;
		return;
	}
	if (Is64Bit(Reg)) {
		x86Mapped(MipsRegHi(Reg)) = NotMapped;
		x86Protected(MipsRegHi(Reg)) = FALSE;
	}
	x86Mapped(MipsRegLo(Reg)) = NotMapped;
	x86Protected(MipsRegLo(Reg)) = FALSE;
	if (!WriteBackValue) {
		MipsRegState(Reg) = STATE_UNKNOWN;
		return;
	}
	MoveX86regToVariable(MipsRegLo(Reg),&GPR[Reg].UW[0]);
	if (Is64Bit(Reg)) {
		MoveX86regToVariable(MipsRegHi(Reg),&GPR[Reg].UW[1]);
	} else {
		if (IsSigned(Reg)) {
			ShiftRightSignImmed(MipsRegLo(Reg),31);
			MoveX86regToVariable(MipsRegLo(Reg),&GPR[Reg].UW[1]);
		} else {
			MoveConstToVariable(0,&GPR[Reg].UW[1]);
		}
	}
	MipsRegState(Reg) = STATE_UNKNOWN;
}
int UnMap_TempReg (BLOCK_SECTION * Section) {
	int count;
	for (count = 0; count < 10; count ++) {
		if (x86Mapped(count) == Temp_Mapped) {
			if (x86Protected(count) == FALSE) {
				x86Mapped(count) = NotMapped;
				return count;
			}
		}
	}
	return -1;
}
BOOL UnMap_X86reg (BLOCK_SECTION * Section,DWORD x86Reg) {
	int count;
	if (x86Mapped(x86Reg) == NotMapped && x86Protected(x86Reg) == FALSE) return TRUE;
	if (x86Mapped(x86Reg) == Temp_Mapped) {
		if (x86Protected(x86Reg) == FALSE) {
			x86Mapped(x86Reg) = NotMapped;
			return TRUE;
		}
		return FALSE;
	}
	for (count = 1; count < 32; count ++) {
		if (IsMapped(count)) {
			if (Is64Bit(count)) {
				if (MipsRegHi(count) == x86Reg) {
					if (x86Protected(x86Reg) == FALSE) {
						UnMap_GPR(Section,count,TRUE);
						return TRUE;
					}
					break;
				}
			}
			if (MipsRegLo(count) == x86Reg) {
				if (x86Protected(x86Reg) == FALSE) {
					UnMap_GPR(Section,count,TRUE);
					return TRUE;
				}
				break;
			}
		}
	}
	if (x86Mapped(x86Reg) == Stack_Mapped) {
		MoveX86regToVariable(x86Reg,&MemoryStack);
		x86Mapped(x86Reg) = NotMapped;
		return TRUE;
	}
	return FALSE;
}
void UnProtectGPR(BLOCK_SECTION * Section,DWORD Reg) {
	if (IsUnknown(Reg) || IsConst(Reg)) return;
	if (Is64Bit(Reg)) {
		x86Protected(MipsRegHi(Reg)) = FALSE;
	}
	x86Protected(MipsRegLo(Reg)) = FALSE;
}
void UpdateCurrentHalfLine (void) {
	if (Timers.Timer < 0) {
		HalfLine = 0;
		return;
	}
	HalfLine = (Timers.Timer / 1500);
	HalfLine &= ~1;
	HalfLine |= ViFieldSerration;
	VI_V_CURRENT_LINE_REG = HalfLine;
}
void UpdateFieldSerration(int interlaced)
{
	ViFieldSerration ^= 1;
	ViFieldSerration &= interlaced;
}
void WriteBackRegisters (BLOCK_SECTION * Section) {
	int count;
	BOOL bEdiZero = FALSE;
	BOOL bEsiSign = FALSE;
	/*** coming soon ***/
	BOOL bEaxGprLo = FALSE;
	BOOL bEbxGprHi = FALSE;
	for (count = 1; count < 10; count ++) { x86Protected(count) = FALSE; }
	for (count = 1; count < 10; count ++) { UnMap_X86reg (Section,count); }
	/*************************************/
	for (count = 1; count < 32; count ++) {
		switch (MipsRegState(count)) {
		case STATE_CONST_32:
			if (!bEdiZero && (!MipsRegLo(count) || !(MipsRegLo(count) & 0x80000000))) {
				XorX86RegToX86Reg(x86_EDI,x86_EDI);
				bEdiZero = TRUE;
			}
			if (!bEsiSign && (MipsRegLo(count) & 0x80000000)) {
				MoveConstToX86reg(0xFFFFFFFF,x86_ESI);
				bEsiSign = TRUE;
			}
			if ((MipsRegLo(count) & 0x80000000) != 0) {
				MoveX86regToVariable(x86_ESI,&GPR[count].UW[1]);
			} else {
				MoveX86regToVariable(x86_EDI,&GPR[count].UW[1]);
			}
			if (MipsRegLo(count) == 0) {
				MoveX86regToVariable(x86_EDI,&GPR[count].UW[0]);
			} else if (MipsRegLo(count) == 0xFFFFFFFF) {
				MoveX86regToVariable(x86_ESI,&GPR[count].UW[0]);
			} else
				MoveConstToVariable(MipsRegLo(count),&GPR[count].UW[0]);
			MipsRegState(count) = STATE_UNKNOWN;
		case STATE_UNKNOWN: break;
		case STATE_CONST_64:
			if (MipsRegLo(count) == 0 || MipsRegHi(count) == 0) {
				XorX86RegToX86Reg(x86_EDI,x86_EDI);
				bEdiZero = TRUE;
			}
			if (MipsRegLo(count) == 0xFFFFFFFF || MipsRegHi(count) == 0xFFFFFFFF) {
				MoveConstToX86reg(0xFFFFFFFF,x86_ESI);
				bEsiSign = TRUE;
			}
			if (MipsRegHi(count) == 0) {
				MoveX86regToVariable(x86_EDI,&GPR[count].UW[1]);
			} else if (MipsRegLo(count) == 0xFFFFFFFF) {
				MoveX86regToVariable(x86_ESI,&GPR[count].UW[1]);
			} else {
				MoveConstToVariable(MipsRegHi(count),&GPR[count].UW[1]);
			}
			if (MipsRegLo(count) == 0) {
				MoveX86regToVariable(x86_EDI,&GPR[count].UW[0]);
			} else if (MipsRegLo(count) == 0xFFFFFFFF) {
				MoveX86regToVariable(x86_ESI,&GPR[count].UW[0]);
			} else {
				MoveConstToVariable(MipsRegLo(count),&GPR[count].UW[0]);
			}
			MipsRegState(count) = STATE_UNKNOWN;
			break;
		}
	}
	UnMap_AllFPRs(Section);
}