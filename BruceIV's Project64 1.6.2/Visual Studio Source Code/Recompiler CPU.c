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
#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
#include "x86.h"
#include "Plugin.h"
#define SectionCPC4 Section->CompilePC -= 4;
void _fastcall CreateSectionLinkage (BLOCK_SECTION * Section);
void _fastcall DetermineLoop(BLOCK_SECTION * Section,DWORD Test,DWORD Test2,DWORD TestID);
BOOL DisplaySectionInformation (BLOCK_SECTION * Section,DWORD ID,DWORD Test);
BLOCK_SECTION * ExistingSection(BLOCK_SECTION * StartSection,DWORD Addr,DWORD Test);
void _fastcall FillSectionInfo(BLOCK_SECTION * Section);
void _fastcall FixConstants (BLOCK_SECTION * Section,DWORD Test,int * Changed);
BOOL GenerateX86Code (BLOCK_SECTION * Section,DWORD Test);
DWORD GetNewTestValue(void);
void _fastcall InheritConstants(BLOCK_SECTION * Section);
BOOL InheritParentInfo (BLOCK_SECTION * Section);
void _fastcall InitializeSection(BLOCK_SECTION * Section,BLOCK_SECTION * Parent,DWORD StartAddr,DWORD ID);
void InitializeRegSet(REG_INFO * RegSet);
BOOL IsAllParentLoops(BLOCK_SECTION * Section,BLOCK_SECTION * Parent,BOOL IgnoreIfCompiled,DWORD Test);
void MarkCodeBlock (DWORD PAddr);
void SyncRegState (BLOCK_SECTION * Section,REG_INFO * SyncTo);
DWORD TLBLoadAddress,TargetIndex;
TARGET_INFO * TargetInfo = NULL;
BLOCK_INFO BlockInfo;
ORIGINAL_MEMMARKER * OrigMem = NULL;
void InitializeInitialCompilerVariable (void)
{
	memset(&BlockInfo,0,sizeof(BlockInfo));
}
void _fastcall AddParent(BLOCK_SECTION * Section,BLOCK_SECTION * Parent) {
	int NoOfParents,count;
	if (Section == NULL) return;
	if (Parent == NULL) {
		InitializeRegSet(&Section->RegStart);
		memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
		return;
	}
	if (Section->ParentSection != NULL) {
		for (NoOfParents = 0;Section->ParentSection[NoOfParents] != NULL;NoOfParents++) {
			if (Section->ParentSection[NoOfParents] == Parent) return;
		}
		for (NoOfParents = 0;Section->ParentSection[NoOfParents] != NULL;NoOfParents++);
		NoOfParents += 1;
	} else {
		NoOfParents = 1;
	}
	if (NoOfParents == 1) {
		Section->ParentSection = malloc((NoOfParents + 1)*sizeof(void *));
	} else {
		Section->ParentSection = realloc(Section->ParentSection,(NoOfParents + 1)*sizeof(void *));
	}
	Section->ParentSection[NoOfParents - 1] = Parent;
	Section->ParentSection[NoOfParents] = NULL;
	if (NoOfParents == 1) {
		if (Parent->ContinueSection == Section) {
			memcpy(&Section->RegStart,&Parent->Cont.RegSet,sizeof(REG_INFO));
		} else if (Parent->JumpSection == Section) {
			memcpy(&Section->RegStart,&Parent->Jump.RegSet,sizeof(REG_INFO));
		} else {
		}
		memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
	} else {
		if (Parent->ContinueSection == Section) {
			for (count = 0; count < 32; count++) {
				if (Section->RegStart.MIPS_RegState[count] != Parent->Cont.RegSet.MIPS_RegState[count]) {
					Section->RegStart.MIPS_RegState[count] = STATE_UNKNOWN;
				}
			}
		}
		if (Parent->JumpSection == Section) {
			for (count = 0; count < 32; count++) {
				if (Section->RegStart.MIPS_RegState[count] != Parent->Jump.RegSet.MIPS_RegState[count]) {
					Section->RegStart.MIPS_RegState[count] = STATE_UNKNOWN;
				}
			}
		}
		memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
	}
}
void AnalyzeBlock (void) {
	BLOCK_SECTION * Section = &BlockInfo.BlockInfo;
	BlockInfo.NoOfSections = 1;
	InitializeSection (Section,NULL,BlockInfo.StartVAddr,BlockInfo.NoOfSections);
}
int ConstantsType (__int64 Value) {
	if (((Value >> 32) == -1) && ((Value & 0x80000000) != 0) || ((Value >> 32) == 0) && ((Value & 0x80000000) == 0)) { return STATE_CONST_32; }
	return STATE_CONST_64;
}
BYTE * Compiler4300iBlock(void) {
	DWORD StartAddress;
	int count;
	if (BlockInfo.ExitInfo)
	{
		for (count = 0; count < BlockInfo.ExitCount; count ++) {
			free(BlockInfo.ExitInfo[count]);
		}
		if (BlockInfo.ExitInfo) { free(BlockInfo.ExitInfo); }
		BlockInfo.ExitInfo = NULL;
	}
	memset(&BlockInfo,0,sizeof(BlockInfo));
	BlockInfo.CompiledLocation = RecompPos;
	BlockInfo.StartVAddr = PROGRAM_COUNTER;
	AnalyzeBlock();
	StartAddress = BlockInfo.StartVAddr;
	TranslateVaddr(&StartAddress);
	MarkCodeBlock(StartAddress);
	GenerateX86Code(&BlockInfo.BlockInfo,GetNewTestValue());
	for (count = 0; count < BlockInfo.ExitCount; count ++) {
		SetJump32(BlockInfo.ExitInfo[count]->JumpLoc,RecompPos);
		NextInstruction = BlockInfo.ExitInfo[count]->NextInstruction;
		CompileExit(BlockInfo.ExitInfo[count]->TargetPC,BlockInfo.ExitInfo[count]->ExitRegSet,BlockInfo.ExitInfo[count]->reason,TRUE,NULL);
	}
	FreeSection (BlockInfo.BlockInfo.ContinueSection,&BlockInfo.BlockInfo);
	FreeSection (BlockInfo.BlockInfo.JumpSection,&BlockInfo.BlockInfo);
	for (count = 0; count < BlockInfo.ExitCount; count ++) {
		free(BlockInfo.ExitInfo[count]);
	}
	if (BlockInfo.ExitInfo) { free(BlockInfo.ExitInfo); }
	BlockInfo.ExitInfo = NULL;
	BlockInfo.ExitCount = 0;
	return BlockInfo.CompiledLocation;
}
BYTE * CompileDelaySlot (void) {
	DWORD StartAddress = PROGRAM_COUNTER;
	BLOCK_SECTION * Section,DelaySection;
	BYTE * Block = RecompPos;
	int count,x86Reg;
	Section = &DelaySection;
	if (!r4300i_LW_VAddr(StartAddress,&Opcode.Hex)) DisplayThreadExit("CompileDelaySlot - !r4300i_LW_VAddr(StartAddress,&Opcode.Hex)");
	TranslateVaddr(&StartAddress);
	MarkCodeBlock(StartAddress);
	InitializeSection (Section,NULL,PROGRAM_COUNTER,0);
	InitializeRegSet(&Section->RegStart);
	memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
	BlockCycleCount += CountPerOp;
	BlockRandomModifier += 1;
	switch (Opcode.op) {
	case R4300i_SPECIAL:
		switch (Opcode.funct) {
		case R4300i_SPECIAL_SYSCALL: Compile_R4300i_SPECIAL_SYSCALL(Section); break;
		case R4300i_SPECIAL_BREAK:
		{
			static BOOL Shown = FALSE;
			if (Shown) break;
			HandleModal1(hMainWindow);
			if (MessageBox(NULL,GS(SPECIAL_BREAK),GS(MSG_ERROR_TITLE),MB_YESNO | MB_ICONERROR | MB_SETFOREGROUND) == IDNO) Shown = TRUE;
			HandleModal2(hMainWindow);
			if (Shown) break;
		}
		case R4300i_SPECIAL_SYNC:
		case R4300i_SPECIAL_TGE:
		case R4300i_SPECIAL_TGEU:
		case R4300i_SPECIAL_TLT:
		case R4300i_SPECIAL_TLTU:
		case R4300i_SPECIAL_TEQ:
		case R4300i_SPECIAL_TNE:
		break;
		case R4300i_SPECIAL_SLL: Compile_R4300i_SPECIAL_SLL(Section); break;
		case R4300i_SPECIAL_SRL: Compile_R4300i_SPECIAL_SRL(Section); break;
		case R4300i_SPECIAL_SRA: Compile_R4300i_SPECIAL_SRA(Section); break;
		case R4300i_SPECIAL_SLLV: Compile_R4300i_SPECIAL_SLLV(Section); break;
		case R4300i_SPECIAL_SRLV: Compile_R4300i_SPECIAL_SRLV(Section); break;
		case R4300i_SPECIAL_SRAV: Compile_R4300i_SPECIAL_SRAV(Section); break;
		case R4300i_SPECIAL_JR: Compile_R4300i_SPECIAL_JR(Section); break;
		case R4300i_SPECIAL_JALR: Compile_R4300i_SPECIAL_JALR(Section); break;
		case R4300i_SPECIAL_MFHI: Compile_R4300i_SPECIAL_MFHI(Section); break;
		case R4300i_SPECIAL_MTHI: Compile_R4300i_SPECIAL_MTHI(Section); break;
		case R4300i_SPECIAL_MFLO: Compile_R4300i_SPECIAL_MFLO(Section); break;
		case R4300i_SPECIAL_MTLO: Compile_R4300i_SPECIAL_MTLO(Section); break;
		case R4300i_SPECIAL_DSLLV: Compile_R4300i_SPECIAL_DSLLV(Section); break;
		case R4300i_SPECIAL_DSRLV: Compile_R4300i_SPECIAL_DSRLV(Section); break;
		case R4300i_SPECIAL_DSRAV: Compile_R4300i_SPECIAL_DSRAV(Section); break;
		case R4300i_SPECIAL_MULT: Compile_R4300i_SPECIAL_MULT(Section); break;
		case R4300i_SPECIAL_MULTU: Compile_R4300i_SPECIAL_MULTU(Section); break;
		case R4300i_SPECIAL_DIV: Compile_R4300i_SPECIAL_DIV(Section); break;
		case R4300i_SPECIAL_DIVU: Compile_R4300i_SPECIAL_DIVU(Section); break;
		case R4300i_SPECIAL_DMULTU: Compile_R4300i_SPECIAL_DMULTU(Section); break;
		case R4300i_SPECIAL_DDIV: Compile_R4300i_SPECIAL_DDIV(Section); break;
		case R4300i_SPECIAL_DDIVU: Compile_R4300i_SPECIAL_DDIVU(Section); break;
		case R4300i_SPECIAL_ADD: Compile_R4300i_SPECIAL_ADD(Section); break;
		case R4300i_SPECIAL_ADDU: Compile_R4300i_SPECIAL_ADDU(Section); break;
		case R4300i_SPECIAL_SUB: Compile_R4300i_SPECIAL_SUB(Section); break;
		case R4300i_SPECIAL_SUBU: Compile_R4300i_SPECIAL_SUBU(Section); break;
		case R4300i_SPECIAL_AND: Compile_R4300i_SPECIAL_AND(Section); break;
		case R4300i_SPECIAL_OR: Compile_R4300i_SPECIAL_OR(Section); break;
		case R4300i_SPECIAL_XOR: Compile_R4300i_SPECIAL_XOR(Section); break;
		case R4300i_SPECIAL_NOR: Compile_R4300i_SPECIAL_NOR(Section); break;
		case R4300i_SPECIAL_SLT: Compile_R4300i_SPECIAL_SLT(Section); break;
		case R4300i_SPECIAL_SLTU: Compile_R4300i_SPECIAL_SLTU(Section); break;
		case R4300i_SPECIAL_DADD: Compile_R4300i_SPECIAL_DADD(Section); break;
		case R4300i_SPECIAL_DADDU: Compile_R4300i_SPECIAL_DADDU(Section); break;
		case R4300i_SPECIAL_DSUB: Compile_R4300i_SPECIAL_DSUB(Section); break;
		case R4300i_SPECIAL_DSUBU: Compile_R4300i_SPECIAL_DSUBU(Section); break;
		case R4300i_SPECIAL_DSLL: Compile_R4300i_SPECIAL_DSLL(Section); break;
		case R4300i_SPECIAL_DSRL: Compile_R4300i_SPECIAL_DSRL(Section); break;
		case R4300i_SPECIAL_DSRA: Compile_R4300i_SPECIAL_DSRA(Section); break;
		case R4300i_SPECIAL_DSLL32: Compile_R4300i_SPECIAL_DSLL32(Section); break;
		case R4300i_SPECIAL_DSRL32: Compile_R4300i_SPECIAL_DSRL32(Section); break;
		case R4300i_SPECIAL_DSRA32: Compile_R4300i_SPECIAL_DSRA32(Section); break;
		default:
			DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_SPECIAL: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on a reserved Opcode at this location");
		}
	break;
	case R4300i_REGIMM:
		switch (Opcode.rt) {
		case R4300i_REGIMM_BLTZ: Compile_R4300i_Branch(Section,BLTZ_Compare,BranchTypeRs,FALSE); break;
		case R4300i_REGIMM_BGEZ: Compile_R4300i_Branch(Section,BGEZ_Compare,BranchTypeRs,FALSE); break;
		case R4300i_REGIMM_BLTZL: Compile_R4300i_BranchLikely(Section,BLTZ_Compare,FALSE); break;
		case R4300i_REGIMM_BGEZL: Compile_R4300i_BranchLikely(Section,BGEZ_Compare,FALSE);
		case R4300i_REGIMM_TGEI:
		case R4300i_REGIMM_TGEIU:
		case R4300i_REGIMM_TLTI:
		case R4300i_REGIMM_TLTIU:
		case R4300i_REGIMM_TEQI:
		case R4300i_REGIMM_TNEI:
		case R4300i_REGIMM_BLTZALL:
		case R4300i_REGIMM_BGEZALL: break;
		case R4300i_REGIMM_BLTZAL: Compile_R4300i_Branch(Section,BLTZ_Compare,BranchTypeRs,TRUE); break;
		case R4300i_REGIMM_BGEZAL: Compile_R4300i_Branch(Section,BGEZ_Compare,BranchTypeRs,TRUE); break;
		default:
			DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_REGIMM: - switch (Opcode.rt) - default:\n\nThe emulator has crashed on a reserved Opcode at this location");
		}
	break;
	case R4300i_J: Compile_R4300i_J(Section); break;
	case R4300i_JAL: Compile_R4300i_JAL(Section); break;
	case R4300i_BEQ: Compile_R4300i_Branch(Section,BEQ_Compare,BranchTypeRsRt,FALSE); break;
	case R4300i_BNE: Compile_R4300i_Branch(Section,BNE_Compare,BranchTypeRsRt,FALSE); break;
	case R4300i_BLEZ: Compile_R4300i_Branch(Section,BLEZ_Compare,BranchTypeRs,FALSE); break;
	case R4300i_BGTZ: Compile_R4300i_Branch(Section,BGTZ_Compare,BranchTypeRs,FALSE); break;
	case R4300i_ADDI: Compile_R4300i_ADDI(Section); break;
	case R4300i_ADDIU: Compile_R4300i_ADDIU(Section); break;
	case R4300i_SLTI: Compile_R4300i_SLTI(Section); break;
	case R4300i_SLTIU: Compile_R4300i_SLTIU(Section); break;
	case R4300i_ANDI: Compile_R4300i_ANDI(Section); break;
	case R4300i_ORI: Compile_R4300i_ORI(Section); break;
	case R4300i_XORI: Compile_R4300i_XORI(Section); break;
	case R4300i_LUI: Compile_R4300i_LUI(Section); break;
	case R4300i_CP0:
		switch (Opcode.rs) {
		case R4300i_COP0_MF: Compile_R4300i_COP0_MF(Section); break;
		case R4300i_COP0_MT: Compile_R4300i_COP0_MT(Section);
		case R4300i_COP0_DMF:
		case R4300i_COP0_DMT:
		break;
		default:
			if ((Opcode.rs & 0x10) != 0) {
				switch (Opcode.funct) {
				case R4300i_COP0_CO_TLBR: Compile_R4300i_COP0_CO_TLBR(Section); break;
				case R4300i_COP0_CO_TLBWI: Compile_R4300i_COP0_CO_TLBWI(Section); break;
				case R4300i_COP0_CO_TLBWR: Compile_R4300i_COP0_CO_TLBWR(Section); break;
				case R4300i_COP0_CO_TLBP: Compile_R4300i_COP0_CO_TLBP(Section); break;
				case R4300i_COP0_CO_ERET: Compile_R4300i_COP0_CO_ERET(Section); break;
				default:
					DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP0: - switch (Opcode.rs) - default: - (Opcode.rs & 0x10) != 0) - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				}
			} else DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP0: - switch (Opcode.rs) - default: - (Opcode.rs & 0x10) != 0) - else\n\nThe emulator has crashed on an unknown Opcode at this location");
		}
	break;
	case R4300i_CP1:
		switch (Opcode.rs) {
		case R4300i_COP1_MF: Compile_R4300i_COP1_MF(Section); break;
		case R4300i_COP1_DMF: Compile_R4300i_COP1_DMF(Section); break;
		case R4300i_COP1_CF: Compile_R4300i_COP1_CF(Section); break;
		case R4300i_COP1_MT: Compile_R4300i_COP1_MT(Section); break;
		case R4300i_COP1_DMT: Compile_R4300i_COP1_DMT(Section); break;
		case R4300i_COP1_CT: Compile_R4300i_COP1_CT(Section);
		case R4300i_COP1_DCF:
		case R4300i_COP1_DCT:
		break;
		case R4300i_COP1_BC:
			switch (Opcode.ft) {
			case R4300i_COP1_BC_BCF: Compile_R4300i_Branch(Section,COP1_BCF_Compare,BranchTypeCop1,FALSE); break;
			case R4300i_COP1_BC_BCT: Compile_R4300i_Branch(Section,COP1_BCT_Compare,BranchTypeCop1,FALSE); break;
			case R4300i_COP1_BC_BCFL: Compile_R4300i_BranchLikely(Section,COP1_BCF_Compare,FALSE); break;
			case R4300i_COP1_BC_BCTL: Compile_R4300i_BranchLikely(Section,COP1_BCT_Compare,FALSE); break;
			default:
				DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_BC: - switch (Opcode.ft) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		case R4300i_COP1_S:
			switch (Opcode.funct) {
			case R4300i_COP1_FUNCT_ADD: Compile_R4300i_COP1_S_ADD(Section); break;
			case R4300i_COP1_FUNCT_SUB: Compile_R4300i_COP1_S_SUB(Section); break;
			case R4300i_COP1_FUNCT_MUL: Compile_R4300i_COP1_S_MUL(Section); break;
			case R4300i_COP1_FUNCT_DIV: Compile_R4300i_COP1_S_DIV(Section); break;
			case R4300i_COP1_FUNCT_SQRT: Compile_R4300i_COP1_S_SQRT(Section); break;
			case R4300i_COP1_FUNCT_ABS: Compile_R4300i_COP1_S_ABS(Section); break;
			case R4300i_COP1_FUNCT_MOV: Compile_R4300i_COP1_S_MOV(Section); break;
			case R4300i_COP1_FUNCT_NEG: Compile_R4300i_COP1_S_NEG(Section); break;
			case R4300i_COP1_FUNCT_TRUNC_L: Compile_R4300i_COP1_S_TRUNC_L(Section); break;
			case R4300i_COP1_FUNCT_CEIL_L: Compile_R4300i_COP1_S_CEIL_L(Section); break;
			case R4300i_COP1_FUNCT_FLOOR_L: Compile_R4300i_COP1_S_FLOOR_L(Section); break;
			case R4300i_COP1_FUNCT_ROUND_W: Compile_R4300i_COP1_S_ROUND_W(Section); break;
			case R4300i_COP1_FUNCT_TRUNC_W: Compile_R4300i_COP1_S_TRUNC_W(Section); break;
			case R4300i_COP1_FUNCT_CEIL_W: Compile_R4300i_COP1_S_CEIL_W(Section); break;
			case R4300i_COP1_FUNCT_FLOOR_W: Compile_R4300i_COP1_S_FLOOR_W(Section); break;
			case R4300i_COP1_FUNCT_CVT_D: Compile_R4300i_COP1_S_CVT_D(Section); break;
			case R4300i_COP1_FUNCT_CVT_W: Compile_R4300i_COP1_S_CVT_W(Section); break;
			case R4300i_COP1_FUNCT_CVT_L: Compile_R4300i_COP1_S_CVT_L(Section); break;
			case R4300i_COP1_FUNCT_C_F:   case R4300i_COP1_FUNCT_C_UN:
			case R4300i_COP1_FUNCT_C_EQ:  case R4300i_COP1_FUNCT_C_UEQ:
			case R4300i_COP1_FUNCT_C_OLT: case R4300i_COP1_FUNCT_C_ULT:
			case R4300i_COP1_FUNCT_C_OLE: case R4300i_COP1_FUNCT_C_ULE:
			case R4300i_COP1_FUNCT_C_SF:  case R4300i_COP1_FUNCT_C_NGLE:
			case R4300i_COP1_FUNCT_C_SEQ: case R4300i_COP1_FUNCT_C_NGL:
			case R4300i_COP1_FUNCT_C_LT:  case R4300i_COP1_FUNCT_C_NGE:
			case R4300i_COP1_FUNCT_C_LE:  case R4300i_COP1_FUNCT_C_NGT:
				Compile_R4300i_COP1_S_CMP(Section);
			case R4300i_COP1_FUNCT_ROUND_L:
			case R4300i_COP1_FUNCT_CVT_S:
			break;
			default:
				DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_S: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		case R4300i_COP1_D:
			switch (Opcode.funct) {
			case R4300i_COP1_FUNCT_ADD: Compile_R4300i_COP1_D_ADD(Section); break;
			case R4300i_COP1_FUNCT_SUB: Compile_R4300i_COP1_D_SUB(Section); break;
			case R4300i_COP1_FUNCT_MUL: Compile_R4300i_COP1_D_MUL(Section); break;
			case R4300i_COP1_FUNCT_DIV: Compile_R4300i_COP1_D_DIV(Section); break;
			case R4300i_COP1_FUNCT_SQRT: Compile_R4300i_COP1_D_SQRT(Section); break;
			case R4300i_COP1_FUNCT_ABS: Compile_R4300i_COP1_D_ABS(Section); break;
			case R4300i_COP1_FUNCT_MOV: Compile_R4300i_COP1_D_MOV(Section); break;
			case R4300i_COP1_FUNCT_NEG: Compile_R4300i_COP1_D_NEG(Section); break;
			case R4300i_COP1_FUNCT_TRUNC_L: Compile_R4300i_COP1_D_TRUNC_L(Section); break;
			case R4300i_COP1_FUNCT_CEIL_L: Compile_R4300i_COP1_D_CEIL_L(Section); break;
			case R4300i_COP1_FUNCT_FLOOR_L: Compile_R4300i_COP1_D_FLOOR_L(Section); break;
			case R4300i_COP1_FUNCT_ROUND_W: Compile_R4300i_COP1_D_ROUND_W(Section); break;
			case R4300i_COP1_FUNCT_TRUNC_W: Compile_R4300i_COP1_D_TRUNC_W(Section); break;
			case R4300i_COP1_FUNCT_CEIL_W: Compile_R4300i_COP1_D_CEIL_W(Section); break;
			case R4300i_COP1_FUNCT_FLOOR_W: Compile_R4300i_COP1_D_FLOOR_W(Section); break;
			case R4300i_COP1_FUNCT_CVT_S: Compile_R4300i_COP1_D_CVT_S(Section); break;
			case R4300i_COP1_FUNCT_CVT_W: Compile_R4300i_COP1_D_CVT_W(Section); break;
			case R4300i_COP1_FUNCT_CVT_L: Compile_R4300i_COP1_D_CVT_L(Section); break;
			case R4300i_COP1_FUNCT_C_F:   case R4300i_COP1_FUNCT_C_UN:
			case R4300i_COP1_FUNCT_C_EQ:  case R4300i_COP1_FUNCT_C_UEQ:
			case R4300i_COP1_FUNCT_C_OLT: case R4300i_COP1_FUNCT_C_ULT:
			case R4300i_COP1_FUNCT_C_OLE: case R4300i_COP1_FUNCT_C_ULE:
			case R4300i_COP1_FUNCT_C_SF:  case R4300i_COP1_FUNCT_C_NGLE:
			case R4300i_COP1_FUNCT_C_SEQ: case R4300i_COP1_FUNCT_C_NGL:
			case R4300i_COP1_FUNCT_C_LT:  case R4300i_COP1_FUNCT_C_NGE:
			case R4300i_COP1_FUNCT_C_LE:  case R4300i_COP1_FUNCT_C_NGT:
				Compile_R4300i_COP1_D_CMP(Section);
			case R4300i_COP1_FUNCT_ROUND_L:
			case R4300i_COP1_FUNCT_CVT_D:
			break;
			default:
				DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_D: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		case R4300i_COP1_W:
			switch (Opcode.funct) {
			case R4300i_COP1_FUNCT_CVT_S: Compile_R4300i_COP1_W_CVT_S(Section); break;
			case R4300i_COP1_FUNCT_CVT_D: Compile_R4300i_COP1_W_CVT_D(Section); break;
			default:
				DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_W: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		case R4300i_COP1_L:
			switch (Opcode.funct) {
			case R4300i_COP1_FUNCT_CVT_S: Compile_R4300i_COP1_L_CVT_S(Section); break;
			case R4300i_COP1_FUNCT_CVT_D: Compile_R4300i_COP1_L_CVT_D(Section); break;
			default:
				DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_L: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		default:
			DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP1: - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
		}
	break;
	case R4300i_CP2:
		switch (Opcode.rs) {
		case R4300i_COP2_MF:
		case R4300i_COP2_DMF:
		case R4300i_COP2_CF:
		case R4300i_COP2_MT:
		case R4300i_COP2_DMT:
		case R4300i_COP2_CT:
		case R4300i_COP2_DCF:
		case R4300i_COP2_DCT:
		break;
		default:
			DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - case R4300i_CP2: - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
		}
	break;
	case R4300i_BEQL: Compile_R4300i_BranchLikely(Section,BEQ_Compare,FALSE); break;
	case R4300i_BNEL: Compile_R4300i_BranchLikely(Section,BNE_Compare,FALSE); break;
	case R4300i_BLEZL: Compile_R4300i_BranchLikely(Section,BLEZ_Compare,FALSE); break;
	case R4300i_BGTZL: Compile_R4300i_BranchLikely(Section,BGTZ_Compare,FALSE); break;
	case R4300i_DADDI:
	case R4300i_DADDIU: Compile_R4300i_DADDIU(Section); break;
	case R4300i_LDL: Compile_R4300i_LDL(Section); break;
	case R4300i_LDR: Compile_R4300i_LDR(Section); break;
	case R4300i_LB: Compile_R4300i_LB(Section); break;
	case R4300i_LH: Compile_R4300i_LH(Section); break;
	case R4300i_LWL: Compile_R4300i_LWL(Section); break;
	case R4300i_LW: Compile_R4300i_LW(Section); break;
	case R4300i_LBU: Compile_R4300i_LBU(Section); break;
	case R4300i_LHU: Compile_R4300i_LHU(Section); break;
	case R4300i_LWR: Compile_R4300i_LWR(Section); break;
	case R4300i_LWU: Compile_R4300i_LWU(Section); break;
	case R4300i_SB: Compile_R4300i_SB(Section); break;
	case R4300i_SH: Compile_R4300i_SH(Section); break;
	case R4300i_SWL: Compile_R4300i_SWL(Section); break;
	case R4300i_SW: Compile_R4300i_SW(Section); break;
	case R4300i_SDL: Compile_R4300i_SDL(Section); break;
	case R4300i_SDR: Compile_R4300i_SDR(Section); break;
	case R4300i_SWR: Compile_R4300i_SWR(Section); break;
	case R4300i_CACHE: Compile_R4300i_CACHE(Section); break;
	case R4300i_LL: Compile_R4300i_LL(Section); break;
	case R4300i_LWC1: Compile_R4300i_LWC1(Section);
	case R4300i_CP3:
	case R4300i_LWC2:
	case R4300i_LLD:
	case R4300i_LDC2:
	case R4300i_SWC2:
	case R4300i_SCD:
	case R4300i_SDC2:
	break;
	case R4300i_LDC1: Compile_R4300i_LDC1(Section); break;
	case R4300i_LD: Compile_R4300i_LD(Section); break;
	case R4300i_SC: Compile_R4300i_SC(Section); break;
	case R4300i_SWC1: Compile_R4300i_SWC1(Section); break;
	case R4300i_SDC1: Compile_R4300i_SDC1(Section); break;
	case R4300i_SD: Compile_R4300i_SD(Section); break;
	default:
		DisplayThreadExit("CompileDelaySlot - switch (Opcode.op) - default:\n\nThe emulator has crashed on a reserved Opcode at this location");
		break;
	}
	for (count = 1; count < 10; count ++) { x86Protected(count) = FALSE; }
	WriteBackRegisters(Section);
	if (BlockCycleCount != 0) {
		AddConstToVariable(BlockCycleCount,&CP0[9]);
		SubConstFromVariable(BlockCycleCount,&Timers.Timer);
	}
	if (BlockRandomModifier != 0) { SubConstFromVariable(BlockRandomModifier,&CP0[1]); }
	x86Reg = Map_TempReg(Section,x86_Any,-1,FALSE);
	MoveVariableToX86reg(&JumpToLocation,x86Reg);
	MoveX86regToVariable(x86Reg,&PROGRAM_COUNTER);
	MoveConstToVariable(NORMAL,&NextInstruction);
	Ret();
	return Block;
}
void CompileExit (DWORD TargetPC,REG_INFO ExitRegSet,int reason,int CompileNow,void (*x86Jmp)(DWORD Value)) {
	BLOCK_SECTION Section;
	if (!CompileNow) {
		if (BlockInfo.ExitCount == 0) {
			BlockInfo.ExitInfo = malloc(sizeof(void *));
		} else {
			BlockInfo.ExitInfo = realloc(BlockInfo.ExitInfo,(BlockInfo.ExitCount + 1) * sizeof(void *));
		}
		if (x86Jmp == NULL) DisplayThreadExit("CompileExit - x86Jmp == NULL");
		x86Jmp(0);
		BlockInfo.ExitInfo[BlockInfo.ExitCount] = malloc(sizeof(EXIT_INFO));
		BlockInfo.ExitInfo[BlockInfo.ExitCount]->TargetPC = TargetPC;
		BlockInfo.ExitInfo[BlockInfo.ExitCount]->ExitRegSet = ExitRegSet;
		BlockInfo.ExitInfo[BlockInfo.ExitCount]->reason = reason;
		BlockInfo.ExitInfo[BlockInfo.ExitCount]->NextInstruction = NextInstruction;
		BlockInfo.ExitInfo[BlockInfo.ExitCount]->JumpLoc = RecompPos - 4;
		BlockInfo.ExitCount += 1;
		return;
	}
	InitializeSection (&Section,NULL,(DWORD)-1,0);
	memcpy(&Section.RegWorking,&ExitRegSet,sizeof(REG_INFO));
	if (TargetPC != (DWORD)-1) { MoveConstToVariable(TargetPC,&PROGRAM_COUNTER); }
	if (ExitRegSet.CycleCount != 0) {
		AddConstToVariable(ExitRegSet.CycleCount,&CP0[9]);
		SubConstFromVariable(ExitRegSet.CycleCount,&Timers.Timer);
	}
	if (ExitRegSet.RandomModifier != 0) { SubConstFromVariable(ExitRegSet.RandomModifier,&CP0[1]); }
	WriteBackRegisters(&Section);
	switch (reason) {
	case Normal: case Normal_NoSysCheck:
		Section.RegWorking.RandomModifier = 0;
		Section.RegWorking.CycleCount = 0;
		if (reason == Normal) { CompileSystemCheck(0,(DWORD)-1,Section.RegWorking);	}
		if (SelfModCheck != ModCode_CheckSetMemory && SelfModCheck != ModCode_CheckMemory) {
			BYTE * Jump,* Jump2;
			if (TargetPC >= 0x80000000 && TargetPC < 0x90000000) {
				DWORD pAddr = TargetPC & 0x1FFFFFFF;
				MoveVariableToX86reg((BYTE *)JumpTable + pAddr,x86_ECX);
				Jump2 = NULL;
			} else if (TargetPC >= 0x90000000 && TargetPC < 0xC0000000) {
			} else {
				MoveConstToX86reg((TargetPC >> 12),x86_ECX);
				MoveConstToX86reg(TargetPC,x86_EBX);
				MoveVariableDispToX86Reg(TLB_ReadMap,x86_ECX,x86_ECX,4);
				TestX86RegToX86Reg(x86_ECX,x86_ECX);
				JeLabel8(0);
				Jump2 = RecompPos - 1;
				AddConstToX86Reg(x86_ECX,(DWORD)JumpTable - (DWORD)N64MEM);
				MoveX86regPointerToX86reg(x86_ECX,x86_EBX,x86_ECX);
			}
			if (TargetPC < 0x90000000 || TargetPC >= 0xC0000000)
			{
				JecxzLabel8(0);
				Jump = RecompPos - 1;
				JmpDirectReg(x86_ECX);
				*((BYTE *)(Jump))=(BYTE)(RecompPos - Jump - 1);
				if (Jump2 != NULL) *((BYTE *)(Jump2))=(BYTE)(RecompPos - Jump2 - 1);
			}
		}
		Ret();
		break;
	case DoCPU_Action:
		Call_Direct(DoSomething);
		Ret();
		break;
	case DoSysCall:
		MoveConstToX86reg(NextInstruction == JUMP || NextInstruction == DELAY_SLOT,x86_ECX);
		Call_Direct(DoSysCallException);
		Ret();
		break;
	case COP1_Unuseable:
		MoveConstToX86reg(NextInstruction == JUMP || NextInstruction == DELAY_SLOT,x86_ECX);
		MoveConstToX86reg(1,x86_EDX);
		Call_Direct(DoCopUnusableException);
		Ret();
		break;
	case ExitResetRecompCode:
		Call_Direct(ResetRecompCode);
		Ret();
		break;
	case TLBReadMiss:
		MoveConstToX86reg(NextInstruction == JUMP || NextInstruction == DELAY_SLOT,x86_ECX);
		MoveVariableToX86reg(&TLBLoadAddress,x86_EDX);
		Call_Direct(DoTLBMiss);
		Ret();
		break;
	case TLBWriteMiss:
		MoveConstToX86reg(NextInstruction == JUMP || NextInstruction == DELAY_SLOT,x86_ECX);
		MoveVariableToX86reg(&TLBLoadAddress,x86_EDX);
		Call_Direct(DoTLBMiss);
		Ret();
	}
}
void CompileSystemCheck (DWORD TimerModifier,DWORD TargetPC,REG_INFO RegSet) {
	BLOCK_SECTION Section;
	BYTE * Jump,* Jump2;
	// Timer
	if (TimerModifier != 0) {
		SubConstFromVariable(TimerModifier,&Timers.Timer);
	} else {
		CompConstToVariable(0,&Timers.Timer);
	}
	JnsLabel32(0);
	Jump = RecompPos - 4;
	Pushad();
	if (TargetPC != (DWORD)-1) { MoveConstToVariable(TargetPC,&PROGRAM_COUNTER); }
	InitializeSection (&Section,NULL,(DWORD)-1,0);
	memcpy(&Section.RegWorking,&RegSet,sizeof(REG_INFO));
	WriteBackRegisters(&Section);
	Call_Direct(TimerDone);
	Popad();
	// Interrupt
	CompConstToVariable(0,&CPU_Action.DoSomething);
	JeLabel32(0);
	Jump2 = RecompPos - 4;
	CompileExit(-1,Section.RegWorking,DoCPU_Action,TRUE,NULL);
	SetJump32(Jump2,RecompPos);
	Ret();
	SetJump32(Jump,RecompPos);
	// Interrupt 2
	CompConstToVariable(0,&CPU_Action.DoSomething);
	JeLabel32(0);
	Jump = RecompPos - 4;
	if (TargetPC != (DWORD)-1) { MoveConstToVariable(TargetPC,&PROGRAM_COUNTER); }
	InitializeSection (&Section,NULL,(DWORD)-1,0);
	memcpy(&Section.RegWorking,&RegSet,sizeof(REG_INFO));
	WriteBackRegisters(&Section);
	CompileExit(-1,Section.RegWorking,DoCPU_Action,TRUE,NULL);
	SetJump32(Jump,RecompPos);
}
void _fastcall CreateSectionLinkage (BLOCK_SECTION * Section) {
	BLOCK_SECTION ** TargetSection[2];
	DWORD * TargetPC[2],count;
	InheritConstants(Section);
	__try {
		FillSectionInfo(Section);
	} __except(r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())) {
		DisplayThreadExit("CreateSectionLinkage - r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation()");
	}
	if (Section->Jump.TargetPC < Section->Cont.TargetPC) {
		TargetSection[0] = (BLOCK_SECTION **)&Section->JumpSection;
		TargetSection[1] = (BLOCK_SECTION **)&Section->ContinueSection;
		TargetPC[0] = &Section->Jump.TargetPC;
		TargetPC[1] = &Section->Cont.TargetPC;
	} else {
		TargetSection[0] = (BLOCK_SECTION **)&Section->ContinueSection;
		TargetSection[1] = (BLOCK_SECTION **)&Section->JumpSection;
		TargetPC[0] = &Section->Cont.TargetPC;
		TargetPC[1] = &Section->Jump.TargetPC;
	}
	for (count = 0; count < 2; count ++) {
		if (*TargetPC[count] != (DWORD)-1 && *TargetSection[count] == NULL) {
			*TargetSection[count] = ExistingSection(BlockInfo.BlockInfo.ContinueSection,*TargetPC[count],GetNewTestValue());
			if (*TargetSection[count] == NULL) {
				*TargetSection[count] = ExistingSection(BlockInfo.BlockInfo.JumpSection,*TargetPC[count],GetNewTestValue());
			}
			if (*TargetSection[count] == NULL) {
				BlockInfo.NoOfSections += 1;
				*TargetSection[count] = malloc(sizeof(BLOCK_SECTION));
				InitializeSection (*TargetSection[count],Section,*TargetPC[count],BlockInfo.NoOfSections);
				CreateSectionLinkage(*TargetSection[count]);
			} else {
				AddParent(*TargetSection[count],Section);
			}
		}
	}
}
void _fastcall DetermineLoop(BLOCK_SECTION * Section,DWORD Test,DWORD Test2,DWORD TestID) {
	if (Section == NULL) return;
	if (Section->SectionID != TestID) {
		if (Section->Test2 == Test2) return;
		Section->Test2 = Test2;
		DetermineLoop(Section->ContinueSection,Test,Test2,TestID);
		DetermineLoop(Section->JumpSection,Test,Test2,TestID);
		return;
	}
	if (Section->Test2 == Test2) {
		Section->InLoop = TRUE;
		return;
	}
	Section->Test2 = Test2;
	DetermineLoop(Section->ContinueSection,Test,Test2,TestID);
	DetermineLoop(Section->JumpSection,Test,Test2,TestID);
	if (Section->Test == Test) return;
	Section->Test = Test;
	if (Section->ContinueSection != NULL) {
		DetermineLoop(Section->ContinueSection,Test,GetNewTestValue(),((BLOCK_SECTION *)Section->ContinueSection)->SectionID);
	}
	if (Section->JumpSection != NULL) {
		DetermineLoop(Section->JumpSection,Test,GetNewTestValue(),((BLOCK_SECTION *)Section->JumpSection)->SectionID);
	}
}
BOOL DisplaySectionInformation (BLOCK_SECTION * Section,DWORD ID,DWORD Test) {
	if (Section == NULL || Section->Test == Test) return FALSE;
	Section->Test = Test;
	if (Section->SectionID != ID) {
		if (DisplaySectionInformation(Section->ContinueSection,ID,Test)) return TRUE;
		if (DisplaySectionInformation(Section->JumpSection,ID,Test)) return TRUE;
		return FALSE;
	}
	return TRUE;
}
BLOCK_SECTION * ExistingSection(BLOCK_SECTION * StartSection,DWORD Addr,DWORD Test) {
	BLOCK_SECTION * Section;
	if (StartSection == NULL) { return NULL; }
	if (StartSection->StartPC == Addr) { return StartSection; }
	if (StartSection->Test == Test) { return NULL; }
	StartSection->Test = Test;
	Section = ExistingSection(StartSection->JumpSection,Addr,Test);
	if (Section != NULL) { return Section; }
	Section = ExistingSection(StartSection->ContinueSection,Addr,Test);
	if (Section != NULL) { return Section; }
	return NULL;
}
void _fastcall FillSectionInfo(BLOCK_SECTION * Section) {
	OPCODE Command;
	if (Section->CompiledLocation != NULL) return;
	Section->CompilePC      = Section->StartPC;
	memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
	SetNormal
	do {
		if (!r4300i_LW_VAddr(Section->CompilePC,&Command.Hex)) DisplayThreadExit("FillSectionInfo - !r4300i_LW_VAddr(Section->CompilePC,&Command.Hex)");
		switch (Command.op) {
		case R4300i_SPECIAL:
			switch (Command.funct) {
			case R4300i_SPECIAL_SLL:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = MipsRegLo(Command.rt) << Command.sa;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
			case R4300i_SPECIAL_SYNC:
			case R4300i_SPECIAL_MTHI:
			case R4300i_SPECIAL_MTLO:
			case R4300i_SPECIAL_MULT:
			case R4300i_SPECIAL_MULTU:
			case R4300i_SPECIAL_DIV:
			case R4300i_SPECIAL_DIVU:
			case R4300i_SPECIAL_DMULT:
			case R4300i_SPECIAL_DMULTU:
			case R4300i_SPECIAL_DDIV:
			case R4300i_SPECIAL_DDIVU: break;
			case R4300i_SPECIAL_SRL:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = MipsRegLo(Command.rt) >> Command.sa;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SRA:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = MipsRegLo_S(Command.rt) >> Command.sa;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SLLV:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = MipsRegLo(Command.rt) << (MipsRegLo(Command.rs) & 0x1F);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SRLV:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = MipsRegLo(Command.rt) >> (MipsRegLo(Command.rs) & 0x1F);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SRAV:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = MipsRegLo_S(Command.rt) >> (MipsRegLo(Command.rs) & 0x1F);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_JR:
				if (IsConst(Command.rs)) {
					Section->Jump.TargetPC = MipsRegLo(Command.rs);
				} else {
					Section->Jump.TargetPC = (DWORD)-1;
				}
				SetDelay
				break;
			case R4300i_SPECIAL_JALR:
				MipsRegLo(Opcode.rd) = Section->CompilePC + 8;
				MipsRegState(Opcode.rd) = STATE_CONST_32;
				if (IsConst(Command.rs)) {
					Section->Jump.TargetPC = MipsRegLo(Command.rs);
				} else {
					Section->Jump.TargetPC = (DWORD)-1;
				}
				SetDelay
				break;
			case R4300i_SPECIAL_MFHI:
			case R4300i_SPECIAL_MFLO: MipsRegState(Command.rd) = STATE_UNKNOWN; break;
			case R4300i_SPECIAL_DSLLV:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg(Command.rd) = Is64Bit(Command.rt)?MipsReg(Command.rt):(QWORD)MipsRegLo_S(Command.rt) << (MipsRegLo(Command.rs) & 0x3F);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSRLV:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg(Command.rd) = Is64Bit(Command.rt)?MipsReg(Command.rt):(QWORD)MipsRegLo_S(Command.rt) >> (MipsRegLo(Command.rs) & 0x3F);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSRAV:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg(Command.rd) = Is64Bit(Command.rt)?MipsReg_S(Command.rt):(_int64)MipsRegLo_S(Command.rt) >> (MipsRegLo(Command.rs) & 0x3F);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_ADD:
			case R4300i_SPECIAL_ADDU:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegLo(Command.rd) = MipsRegLo(Command.rs) + MipsRegLo(Command.rt);
					MipsRegState(Command.rd) = STATE_CONST_32;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SUB:
			case R4300i_SPECIAL_SUBU:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsRegLo(Command.rd) = MipsRegLo(Command.rs) - MipsRegLo(Command.rt);
					MipsRegState(Command.rd) = STATE_CONST_32;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_AND:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					if (Is64Bit(Command.rt) && Is64Bit(Command.rs)) {
						MipsReg(Command.rd) = MipsReg(Command.rt) & MipsReg(Command.rs);
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else if (Is64Bit(Command.rt) || Is64Bit(Command.rs)) {
						if (Is64Bit(Command.rt)) {
							MipsReg(Command.rd) = MipsReg(Command.rt) & MipsRegLo(Command.rs);
						} else {
							MipsReg(Command.rd) = MipsRegLo(Command.rt) & MipsReg(Command.rs);
						}
						MipsRegState(Command.rd) = ConstantsType(MipsReg(Command.rd));
					} else {
						MipsRegLo(Command.rd) = MipsRegLo(Command.rt) & MipsRegLo(Command.rs);
						MipsRegState(Command.rd) = STATE_CONST_32;
					}
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_OR:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					if (Is64Bit(Command.rt) && Is64Bit(Command.rs)) {
						MipsReg(Command.rd) = MipsReg(Command.rt) | MipsReg(Command.rs);
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else if (Is64Bit(Command.rt) || Is64Bit(Command.rs)) {
						if (Is64Bit(Command.rt)) {
							MipsReg(Command.rd) = MipsReg(Command.rt) | MipsRegLo(Command.rs);
						} else {
							MipsReg(Command.rd) = MipsRegLo(Command.rt) | MipsReg(Command.rs);
						}
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else {
						MipsRegLo(Command.rd) = MipsRegLo(Command.rt) | MipsRegLo(Command.rs);
						MipsRegState(Command.rd) = STATE_CONST_32;
					}
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_XOR:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					if (Is64Bit(Command.rt) && Is64Bit(Command.rs)) {
						MipsReg(Command.rd) = MipsReg(Command.rt) ^ MipsReg(Command.rs);
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else if (Is64Bit(Command.rt) || Is64Bit(Command.rs)) {
						if (Is64Bit(Command.rt)) {
							MipsReg(Command.rd) = MipsReg(Command.rt) ^ MipsRegLo(Command.rs);
						} else {
							MipsReg(Command.rd) = MipsRegLo(Command.rt) ^ MipsReg(Command.rs);
						}
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else {
						MipsRegLo(Command.rd) = MipsRegLo(Command.rt) ^ MipsRegLo(Command.rs);
						MipsRegState(Command.rd) = STATE_CONST_32;
					}
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_NOR:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					if (Is64Bit(Command.rt) && Is64Bit(Command.rs)) {
						MipsReg(Command.rd) = ~(MipsReg(Command.rt) | MipsReg(Command.rs));
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else if (Is64Bit(Command.rt) || Is64Bit(Command.rs)) {
						if (Is64Bit(Command.rt)) {
							MipsReg(Command.rd) = ~(MipsReg(Command.rt) | MipsRegLo(Command.rs));
						} else {
							MipsReg(Command.rd) = ~(MipsRegLo(Command.rt) | MipsReg(Command.rs));
						}
						MipsRegState(Command.rd) = STATE_CONST_64;
					} else {
						MipsRegLo(Command.rd) = ~(MipsRegLo(Command.rt) | MipsRegLo(Command.rs));
						MipsRegState(Command.rd) = STATE_CONST_32;
					}
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SLT:
				if (Command.rd == 0) { break; }
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					if (Is64Bit(Command.rt) || Is64Bit(Command.rs)) {
						if (Is64Bit(Command.rt)) {
							MipsRegLo(Command.rd) = (MipsRegLo_S(Command.rs) < MipsReg_S(Command.rt))?1:0;
						} else {
							MipsRegLo(Command.rd) = (MipsReg_S(Command.rs) < MipsRegLo_S(Command.rt))?1:0;
						}
					} else {
						MipsRegLo(Command.rd) = (MipsRegLo_S(Command.rs) < MipsRegLo_S(Command.rt))?1:0;
					}
					MipsRegState(Command.rd) = STATE_CONST_32;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_SLTU:
				if (Command.rd == 0) { break; }
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					if (Is64Bit(Command.rt) || Is64Bit(Command.rs)) {
						if (Is64Bit(Command.rt)) {
							MipsRegLo(Command.rd) = (MipsRegLo(Command.rs) < MipsReg(Command.rt))?1:0;
						} else {
							MipsRegLo(Command.rd) = (MipsReg(Command.rs) < MipsRegLo(Command.rt))?1:0;
						}
					} else {
						MipsRegLo(Command.rd) = (MipsRegLo(Command.rs) < MipsRegLo(Command.rt))?1:0;
					}
					MipsRegState(Command.rd) = STATE_CONST_32;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DADD:
			case R4300i_SPECIAL_DADDU:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsReg(Command.rd) =
						Is64Bit(Command.rs)?MipsReg(Command.rs):(_int64)MipsRegLo_S(Command.rs) +
						Is64Bit(Command.rt)?MipsReg(Command.rt):(_int64)MipsRegLo_S(Command.rt);
					MipsRegState(Command.rd) = STATE_CONST_64;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSUB:
			case R4300i_SPECIAL_DSUBU:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && (Command.rt == Command.rd || Command.rs == Command.rd)) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt) && IsConst(Command.rs)) {
					MipsReg(Command.rd) =
						Is64Bit(Command.rs)?MipsReg(Command.rs):(_int64)MipsRegLo_S(Command.rs) -
						Is64Bit(Command.rt)?MipsReg(Command.rt):(_int64)MipsRegLo_S(Command.rt);
					MipsRegState(Command.rd) = STATE_CONST_64;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSLL:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg(Command.rd) = Is64Bit(Command.rt)?MipsReg(Command.rt):(_int64)MipsRegLo_S(Command.rt) << Command.sa;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSRL:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg(Command.rd) = Is64Bit(Command.rt)?MipsReg(Command.rt):(QWORD)MipsRegLo_S(Command.rt) >> Command.sa;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSRA:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg_S(Command.rd) = Is64Bit(Command.rt)?MipsReg_S(Command.rt):(_int64)MipsRegLo_S(Command.rt) >> Command.sa;
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSLL32:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_64;
					MipsReg(Command.rd) = MipsRegLo(Command.rt) << (Command.sa + 32);
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSRL32:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = (DWORD)(MipsReg(Command.rt) >> (Command.sa + 32));
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			case R4300i_SPECIAL_DSRA32:
				if (Command.rd == 0) { break; }
				if (Section->InLoop && Command.rt == Command.rd) {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				if (IsConst(Command.rt)) {
					MipsRegState(Command.rd) = STATE_CONST_32;
					MipsRegLo(Command.rd) = (DWORD)(MipsReg_S(Command.rt) >> (Command.sa + 32));
				} else {
					MipsRegState(Command.rd) = STATE_UNKNOWN;
				}
				break;
			default:
				SetEnd
				SectionCPC4
			}
			break;
		case R4300i_REGIMM:
			switch (Command.rt) {
			case R4300i_REGIMM_BLTZ:
			case R4300i_REGIMM_BGEZ:
				SetDelay
				Section->Cont.TargetPC = Section->CompilePC + 8;
				Section->Jump.TargetPC = Section->CompilePC + ((short)Command.offset << 2) + 4;
				if (Section->CompilePC == Section->Jump.TargetPC) {
					if (!DelaySlotEffectsCompare(Section->CompilePC,Command.rs,0)) {
						Section->Jump.PermLoop = TRUE;
					}
				}
				break;
			case R4300i_REGIMM_BLTZL:
			case R4300i_REGIMM_BGEZL:
				NextInstruction = LIKELY_DELAY_SLOT;
				Section->Cont.TargetPC = Section->CompilePC + 8;
				Section->Jump.TargetPC = Section->CompilePC + ((short)Command.offset << 2) + 4;
				if (Section->CompilePC == Section->Jump.TargetPC) {
					if (!DelaySlotEffectsCompare(Section->CompilePC,Command.rs,0)) {
						Section->Jump.PermLoop = TRUE;
					}
				}
				break;
			case R4300i_REGIMM_BLTZAL:
			case R4300i_REGIMM_BGEZAL:
				SetDelay
				MipsRegLo(31) = Section->CompilePC + 8;
				MipsRegState(31) = STATE_CONST_32;
				Section->Cont.TargetPC = Section->CompilePC + 8;
				Section->Jump.TargetPC = Section->CompilePC + ((short)Command.offset << 2) + 4;
				if (Section->CompilePC == Section->Jump.TargetPC) {
					if (!DelaySlotEffectsCompare(Section->CompilePC,Command.rs,0)) {
						Section->Jump.PermLoop = TRUE;
					}
				}
				break;
			default:
				SetEnd
				SectionCPC4
			}
			break;
		case R4300i_JAL:
			SetDelay
			MipsRegLo(31) = Section->CompilePC + 8;
			MipsRegState(31) = STATE_CONST_32;
			Section->Jump.TargetPC = (Section->CompilePC & 0xF0000000) + (Command.target << 2);
			if (Section->CompilePC == Section->Jump.TargetPC) {
				if (!DelaySlotEffectsCompare(Section->CompilePC,31,0)) {
					Section->Jump.PermLoop = TRUE;
				}
			}
			break;
		case R4300i_J:
			SetDelay
			Section->Jump.TargetPC = (Section->CompilePC & 0xF0000000) + (Command.target << 2);
			if (Section->CompilePC == Section->Jump.TargetPC) { Section->Jump.PermLoop = TRUE; }
			break;
		case R4300i_BEQ:
		case R4300i_BNE:
		case R4300i_BLEZ:
		case R4300i_BGTZ:
			SetDelay
			Section->Cont.TargetPC = Section->CompilePC + 8;
			Section->Jump.TargetPC = Section->CompilePC + ((short)Command.offset << 2) + 4;
			if (Section->CompilePC == Section->Jump.TargetPC) {
				if (!DelaySlotEffectsCompare(Section->CompilePC,Command.rs,Command.rt)) {
					Section->Jump.PermLoop = TRUE;
				}
			}
			break;
		case R4300i_ADDI:
		case R4300i_ADDIU:
			if (Command.rt == 0) { break; }
			if (Section->InLoop && Command.rs == Command.rt) {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			if (IsConst(Command.rs)) {
				MipsRegLo(Command.rt) = MipsRegLo(Command.rs) + (short)Command.immediate;
				MipsRegState(Command.rt) = STATE_CONST_32;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_SLTI:
			if (Command.rt == 0) { break; }
			if (IsConst(Command.rs)) {
				if (Is64Bit(Command.rs)) {
					MipsRegLo(Command.rt) = (MipsReg_S(Command.rs) < (_int64)((short)Command.immediate))?1:0;
				} else {
					MipsRegLo(Command.rt) = (MipsRegLo_S(Command.rs) < (int)((short)Command.immediate))?1:0;
				}
				MipsRegState(Command.rt) = STATE_CONST_32;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_SLTIU:
			if (Command.rt == 0) { break; }
			if (IsConst(Command.rs)) {
				if (Is64Bit(Command.rs)) {
					MipsRegLo(Command.rt) = (MipsReg(Command.rs) < (unsigned _int64)((short)Command.immediate))?1:0;
				} else {
					MipsRegLo(Command.rt) = (MipsRegLo(Command.rs) < (DWORD)((short)Command.immediate))?1:0;
				}
				MipsRegState(Command.rt) = STATE_CONST_32;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_LUI:
			if (Command.rt == 0) { break; }
			MipsRegLo(Command.rt) = ((short)Command.offset << 16);
			MipsRegState(Command.rt) = STATE_CONST_32;
			break;
		case R4300i_ANDI:
			if (Command.rt == 0) { break; }
			if (Section->InLoop && Command.rs == Command.rt) {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			if (IsConst(Command.rs)) {
				MipsRegState(Command.rt) = STATE_CONST_32;
				MipsRegLo(Command.rt) = MipsRegLo(Command.rs) & Command.immediate;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_ORI:
			if (Command.rt == 0) { break; }
			if (Section->InLoop && Command.rs == Command.rt) {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			if (IsConst(Command.rs)) {
				MipsRegState(Command.rt) = STATE_CONST_32;
				MipsRegLo(Command.rt) = MipsRegLo(Command.rs) | Command.immediate;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_XORI:
			if (Command.rt == 0) { break; }
			if (Section->InLoop && Command.rs == Command.rt) {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			if (IsConst(Command.rs)) {
				MipsRegState(Command.rt) = STATE_CONST_32;
				MipsRegLo(Command.rt) = MipsRegLo(Command.rs) ^ Command.immediate;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_CP0:
			switch (Command.rs) {
			case R4300i_COP0_MF:
				if (Command.rt == 0) { break; }
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			case R4300i_COP0_MT: break;
			default:
				if ((Command.rs & 0x10) != 0) {
					if (Command.funct == R4300i_COP0_CO_ERET) {
						SetEnd
						break;
					}
					if (Command.funct == R4300i_COP0_CO_TLBR || Command.funct == R4300i_COP0_CO_TLBWI || Command.funct == R4300i_COP0_CO_TLBWR || Command.funct == R4300i_COP0_CO_TLBP) break;
				}
				SetEnd
				SectionCPC4
			}
			break;
		case R4300i_CP1:
			switch (Command.fmt) {
			case R4300i_COP1_CF:
			case R4300i_COP1_MF:
			case R4300i_COP1_DMF:
				if (Command.rt == 0) { break; }
				MipsRegState(Command.rt) = STATE_UNKNOWN;
				break;
			case R4300i_COP1_BC:
				switch (Command.ft) {
				case R4300i_COP1_BC_BCF:
				case R4300i_COP1_BC_BCT:
				case R4300i_COP1_BC_BCFL:
				case R4300i_COP1_BC_BCTL:
					SetDelay
					Section->Cont.TargetPC = Section->CompilePC + 8;
					Section->Jump.TargetPC = Section->CompilePC + ((short)Command.offset << 2) + 4;
					if (Section->CompilePC == Section->Jump.TargetPC) {
						int EffectDelaySlot;
						OPCODE NewCommand;
						if (!r4300i_LW_VAddr(Section->CompilePC + 4,&NewCommand.Hex)) DisplayThreadExit("FillSectionInfo - !r4300i_LW_VAddr(Section->CompilePC + 4,&NewCommand.Hex)");
						EffectDelaySlot = FALSE;
						if (NewCommand.op == R4300i_CP1) {
							if (NewCommand.fmt == R4300i_COP1_S && (NewCommand.funct & 0x30) == 0x30) {
								EffectDelaySlot = TRUE;
							}
							if (NewCommand.fmt == R4300i_COP1_D && (NewCommand.funct & 0x30) == 0x30) {
								EffectDelaySlot = TRUE;
							}
						}
						if (!EffectDelaySlot) {
							Section->Jump.PermLoop = TRUE;
						}
					}
				}
			case R4300i_COP1_MT:
			case R4300i_COP1_DMT:
			case R4300i_COP1_CT:
			case R4300i_COP1_S:
			case R4300i_COP1_D:
			case R4300i_COP1_W:
			case R4300i_COP1_L: break;
			default:
				SetEnd
				SectionCPC4
			}
			break;
		case R4300i_BEQL:
		case R4300i_BNEL:
		case R4300i_BLEZL:
		case R4300i_BGTZL:
			NextInstruction = LIKELY_DELAY_SLOT;
			Section->Cont.TargetPC = Section->CompilePC + 8;
			Section->Jump.TargetPC = Section->CompilePC + ((short)Command.offset << 2) + 4;
			if (Section->CompilePC == Section->Jump.TargetPC) {
				if (!DelaySlotEffectsCompare(Section->CompilePC,Command.rs,Command.rt)) {
					Section->Jump.PermLoop = TRUE;
				}
			}
			break;
		case R4300i_DADDI:
		case R4300i_DADDIU:
			if (Command.rt == 0) { break; }
			if (Section->InLoop && Command.rs == Command.rt) {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			if (IsConst(Command.rs)) {
				if (Is64Bit(Command.rs)) {
					int imm32 = (short)Opcode.immediate;
					__int64 imm64 = imm32;
					MipsReg_S(Command.rt) = MipsRegLo_S(Command.rs) + imm64;
				} else {
					MipsReg_S(Command.rt) = MipsRegLo_S(Command.rs) + (short)Command.immediate;
				}
				MipsRegState(Command.rt) = STATE_CONST_64;
			} else {
				MipsRegState(Command.rt) = STATE_UNKNOWN;
			}
			break;
		case R4300i_LDR:
		case R4300i_LDL:
		case R4300i_LB:
		case R4300i_LH:
		case R4300i_LWL:
		case R4300i_LW:
		case R4300i_LWU:
		case R4300i_LL:
		case R4300i_LBU:
		case R4300i_LHU:
		case R4300i_LWR:
		case R4300i_SC:
		case R4300i_LD:
			if (Command.rt == 0) { break; }
			MipsRegState(Command.rt) = STATE_UNKNOWN;
		case R4300i_SB:
		case R4300i_SH:
		case R4300i_SWL:
		case R4300i_SW:
		case R4300i_SDL:
		case R4300i_SDR:
		case R4300i_SWR:
		case R4300i_CACHE:
		case R4300i_LWC1:
		case R4300i_LDC1:
		case R4300i_SWC1:
		case R4300i_SDC1:
		case R4300i_SD: break;
		default:
			SetEnd
			SectionCPC4
			if (Command.Hex == 0x7C1C97C0 || Command.Hex == 0x7FFFFFFF || Command.Hex == 0xF1F3F5F7 || Command.Hex == 0xC1200000 || Command.Hex == 0x4C5A5353) { break; }
		}
		switch (NextInstruction) {
		case NORMAL:
			Section->CompilePC += 4;
			break;
		case DELAY_SLOT:
			NextInstruction = DELAY_SLOT_DONE;
			Section->CompilePC += 4;
			break;
		case LIKELY_DELAY_SLOT:
			memcpy(&Section->Cont.RegSet,&Section->RegWorking,sizeof(REG_INFO));
			NextInstruction = LIKELY_DELAY_SLOT_DONE;
			Section->CompilePC += 4;
			break;
		case DELAY_SLOT_DONE:
			memcpy(&Section->Cont.RegSet,&Section->RegWorking,sizeof(REG_INFO));
			memcpy(&Section->Jump.RegSet,&Section->RegWorking,sizeof(REG_INFO));
			SetEnd
			break;
		case LIKELY_DELAY_SLOT_DONE:
			memcpy(&Section->Jump.RegSet,&Section->RegWorking,sizeof(REG_INFO));
			SetEnd
			break;
		}
		if ((Section->CompilePC & 0xFFFFF000) != (Section->StartPC & 0xFFFFF000)) {
			if (NextInstruction != END_BLOCK && NextInstruction != NORMAL) {
				Section->Cont.TargetPC = (DWORD)-1;
				Section->Jump.TargetPC = (DWORD)-1;
			}
			SetEnd
			SectionCPC4
		}
	} while (NextInstruction != END_BLOCK);
	if (Section->Cont.TargetPC != (DWORD)-1) {
		if ((Section->Cont.TargetPC & 0xFFFFF000) != (Section->StartPC & 0xFFFFF000)) {
			Section->Cont.TargetPC = (DWORD)-1;
		}
	}
	if (Section->Jump.TargetPC != (DWORD)-1) {
		if ((Section->Jump.TargetPC & 0xFFFFF000) != (Section->StartPC & 0xFFFFF000)) {
			Section->Jump.TargetPC = (DWORD)-1;
		}
	}
}
void _fastcall FixConstants (BLOCK_SECTION * Section,DWORD Test,int * Changed) {
	BLOCK_SECTION * Parent;
	int count,NoOfParents;
	REG_INFO Original[2];
	if (Section == NULL || Section->Test == Test) return;
	Section->Test = Test;
	InheritConstants(Section);
	memcpy(&Original[0],&Section->Cont.RegSet,sizeof(REG_INFO));
	memcpy(&Original[1],&Section->Jump.RegSet,sizeof(REG_INFO));
	if (Section->ParentSection) {
		for (NoOfParents = 0;Section->ParentSection[NoOfParents] != NULL;NoOfParents++) {
			Parent = Section->ParentSection[NoOfParents];
			if (Parent->ContinueSection == Section) {
				for (count = 0; count < 32; count++) {
					if (Section->RegStart.MIPS_RegState[count] != Parent->Cont.RegSet.MIPS_RegState[count]) {
						Section->RegStart.MIPS_RegState[count] = STATE_UNKNOWN;
					}
					Section->RegStart.MIPS_RegState[count] = STATE_UNKNOWN;
				}
			}
			if (Parent->JumpSection == Section) {
				for (count = 0; count < 32; count++) {
					if (Section->RegStart.MIPS_RegState[count] != Parent->Jump.RegSet.MIPS_RegState[count]) {
						Section->RegStart.MIPS_RegState[count] = STATE_UNKNOWN;
					}
				}
			}
			memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
		}
	}
	FillSectionInfo(Section);
	if (memcmp(&Original[0],&Section->Cont.RegSet,sizeof(REG_INFO)) != 0) { *Changed = TRUE; }
	if (memcmp(&Original[1],&Section->Jump.RegSet,sizeof(REG_INFO)) != 0) { *Changed = TRUE; }
	if (Section->JumpSection) { FixConstants(Section->JumpSection,Test,Changed); }
	if (Section->ContinueSection) { FixConstants(Section->ContinueSection,Test,Changed); }
}
void FixRandomReg (void) {
	while ((int)Registers.CP0[1] < (int)Registers.CP0[6]) {
		Registers.CP0[1] += 32 - Registers.CP0[6];
	}
}
void FreeSection (BLOCK_SECTION * Section,BLOCK_SECTION * Parent) {
	if (Section == NULL) return;
	if (Section->ParentSection) {
		int NoOfParents,count;
		for (NoOfParents = 0;Section->ParentSection[NoOfParents] != NULL;NoOfParents++);
		for (count = 0; count < NoOfParents; count++) {
			if (Section->ParentSection[count] == Parent) {
				if (NoOfParents == 1) {
					free(Section->ParentSection);
					Section->ParentSection = NULL;
				} else {
					memmove(&Section->ParentSection[count],&Section->ParentSection[count + 1],
						sizeof(void*) * (NoOfParents - count));
					Section->ParentSection = realloc(Section->ParentSection,NoOfParents*sizeof(void *));
				}
				NoOfParents -= 1;
			}
		}
		if (Parent->JumpSection == Section) { Parent->JumpSection = NULL; }
		if (Parent->ContinueSection == Section) { Parent->ContinueSection = NULL; }
		if (Section->ParentSection) {
			for (count = 0; count < NoOfParents; count++) {
				if (!IsAllParentLoops(Section,Section->ParentSection[count],FALSE,GetNewTestValue())) return;
			}
			for (count = 0; count < NoOfParents; count++) {
				Parent = Section->ParentSection[count];
				if (Parent->JumpSection == Section) { Parent->JumpSection = NULL; }
				if (Parent->ContinueSection == Section) { Parent->ContinueSection = NULL; }
			}
			free(Section->ParentSection);
			Section->ParentSection = NULL;
		}
	}
	if (Section->ParentSection == NULL) {
		FreeSection(Section->JumpSection,Section);
		FreeSection(Section->ContinueSection,Section);
		free(Section);
	}
}
void GenerateSectionLinkage (BLOCK_SECTION * Section) {
	BLOCK_SECTION * TargetSection[2],*Parent;
	JUMP_INFO * JumpInfo[2];
	BYTE * Jump;
	int count;
	TargetSection[0] = Section->ContinueSection;
	TargetSection[1] = Section->JumpSection;
	JumpInfo[0] = &Section->Cont;
	JumpInfo[1] = &Section->Jump;
	for (count = 0; count < 2; count ++) {
		if (JumpInfo[count]->LinkLocation == NULL && JumpInfo[count]->FallThrough == FALSE) {
			JumpInfo[count]->TargetPC = -1;
		}
	}
	if ((Section->CompilePC & 0xFFC) == 0xFFC) {
		//Handle Fall through
		Jump = NULL;
		for (count = 0; count < 2; count ++) {
			if (!JumpInfo[count]->FallThrough) continue;
			JumpInfo[count]->FallThrough = FALSE;
			if (JumpInfo[count]->LinkLocation != NULL) {
				SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
				JumpInfo[count]->LinkLocation = NULL;
				if (JumpInfo[count]->LinkLocation2 != NULL) {
					SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
					JumpInfo[count]->LinkLocation2 = NULL;
				}
			}
			MoveConstToVariable(JumpInfo[count]->TargetPC,&JumpToLocation);
			if (JumpInfo[(count + 1) & 1]->LinkLocation == NULL) { break; }
			JmpLabel8(0);
			Jump = RecompPos - 1;
		}
		for (count = 0; count < 2; count ++) {
			if (JumpInfo[count]->LinkLocation == NULL) continue;
			JumpInfo[count]->FallThrough = FALSE;
			if (JumpInfo[count]->LinkLocation != NULL) {
				SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
				JumpInfo[count]->LinkLocation = NULL;
				if (JumpInfo[count]->LinkLocation2 != NULL) {
					SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
					JumpInfo[count]->LinkLocation2 = NULL;
				}
			}
			MoveConstToVariable(JumpInfo[count]->TargetPC,&JumpToLocation);
			if (JumpInfo[(count + 1) & 1]->LinkLocation == NULL) { break; }
			JmpLabel8(0);
			Jump = RecompPos - 1;
		}
		if (Jump != NULL) SetJump8(Jump,RecompPos);
		MoveConstToVariable(Section->CompilePC + 4,&PROGRAM_COUNTER);
		if (BlockCycleCount != 0) {
			AddConstToVariable(BlockCycleCount,&CP0[9]);
			SubConstFromVariable(BlockCycleCount,&Timers.Timer);
		}
		if (BlockRandomModifier != 0) { SubConstFromVariable(BlockRandomModifier,&CP0[1]); }
		WriteBackRegisters(Section);
		MoveConstToVariable(DELAY_SLOT,&NextInstruction);
		Ret();
		return;
	}
	if (Section->CompilePC == Section->Jump.TargetPC && (Section->Cont.TargetPC == -1)) {
		if (!DelaySlotEffectsJump(Section->CompilePC)) {
			WriteBackRegisters(Section);
			memcpy(&Section->Jump.RegSet,&Section->RegWorking,sizeof(REG_INFO));
			Call_Direct(InPermLoop);
		}
	}
	if (TargetSection[0] != TargetSection[1] || TargetSection[0] == NULL) {
		for (count = 0; count < 2; count ++) {
			if (JumpInfo[count]->LinkLocation == NULL && JumpInfo[count]->FallThrough == FALSE) {
				FreeSection(TargetSection[count],Section);
			} else if (TargetSection[count] == NULL && JumpInfo[count]->FallThrough) {
				if (JumpInfo[count]->LinkLocation != NULL) {
					SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
					JumpInfo[count]->LinkLocation = NULL;
					if (JumpInfo[count]->LinkLocation2 != NULL) {
						SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
						JumpInfo[count]->LinkLocation2 = NULL;
					}
				}
				if (JumpInfo[count]->TargetPC > (Section->CompilePC + 4)) {
					CompileExit (JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet,Normal,TRUE,NULL);
				} else {
					CompileExit (JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet,Normal,TRUE,NULL);
				}
				JumpInfo[count]->FallThrough = FALSE;
			} else if (TargetSection[count] != NULL && JumpInfo[count] != NULL) {
				if (!JumpInfo[count]->FallThrough) continue;
				if (JumpInfo[count]->TargetPC == TargetSection[count]->StartPC) continue;
				if (JumpInfo[count]->LinkLocation != NULL) {
					SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
					JumpInfo[count]->LinkLocation = NULL;
					if (JumpInfo[count]->LinkLocation2 != NULL) {
						SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
						JumpInfo[count]->LinkLocation2 = NULL;
					}
				}
				CompileExit (JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet,Normal,TRUE,NULL);
				FreeSection(TargetSection[count],Section);
			}
		}
	} else {
		if (Section->Cont.LinkLocation == NULL && Section->Cont.FallThrough == FALSE) { Section->ContinueSection = NULL; }
		if (Section->Jump.LinkLocation == NULL && Section->Jump.FallThrough == FALSE) { Section->JumpSection = NULL; }
		if (Section->JumpSection == NULL &&  Section->ContinueSection == NULL) {
			FreeSection(TargetSection[0],Section);
		}
	}
	TargetSection[0] = Section->ContinueSection;
	TargetSection[1] = Section->JumpSection;
	for (count = 0; count < 2; count ++) {
		if (TargetSection[count] == NULL) continue;
		if (!JumpInfo[count]->FallThrough) continue;
		if (TargetSection[count]->CompiledLocation != NULL) {
			JumpInfo[count]->FallThrough = FALSE;
			if (JumpInfo[count]->LinkLocation != NULL) {
				SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
				JumpInfo[count]->LinkLocation = NULL;
				if (JumpInfo[count]->LinkLocation2 != NULL) {
					SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
					JumpInfo[count]->LinkLocation2 = NULL;
				}
			}
			if (JumpInfo[count]->RegSet.RandomModifier != 0) {
				SubConstFromVariable(JumpInfo[count]->RegSet.RandomModifier,&CP0[1]);
				JumpInfo[count]->RegSet.RandomModifier = 0;
			}
			if (JumpInfo[count]->RegSet.CycleCount != 0) {
				AddConstToVariable(JumpInfo[count]->RegSet.CycleCount,&CP0[9]);
			}
			if (JumpInfo[count]->TargetPC <= Section->CompilePC) {
				DWORD CycleCount = JumpInfo[count]->RegSet.CycleCount;
				JumpInfo[count]->RegSet.CycleCount = 0;
				if (JumpInfo[count]->PermLoop) {
					MoveConstToVariable(JumpInfo[count]->TargetPC,&PROGRAM_COUNTER);
					Call_Direct(InPermLoop);
					CompileSystemCheck(0,-1,JumpInfo[count]->RegSet);
				} else {
					CompileSystemCheck(CycleCount,JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet);
				}
			} else {
				if (JumpInfo[count]->RegSet.CycleCount != 0) {
					SubConstFromVariable(JumpInfo[count]->RegSet.CycleCount,&Timers.Timer);
					JumpInfo[count]->RegSet.CycleCount = 0;
				}
			}
			memcpy(&Section->RegWorking,&JumpInfo[count]->RegSet,sizeof(REG_INFO));
			SyncRegState(Section,&TargetSection[count]->RegStart);
			JmpLabel32(0);
			SetJump32((DWORD *)RecompPos - 1,TargetSection[count]->CompiledLocation);
		}
	}
	for (count = 0; count < 2; count ++) {
		int count2;
		if (TargetSection[count] == NULL) continue;
		if (TargetSection[count]->ParentSection == NULL) continue;
		for (count2 = 0;TargetSection[count]->ParentSection[count2] != NULL;count2++) {
			Parent = TargetSection[count]->ParentSection[count2];
			if (Parent->CompiledLocation != NULL) continue;
			if (JumpInfo[count]->FallThrough) {
				JumpInfo[count]->FallThrough = FALSE;
				JmpLabel32(0);
				JumpInfo[count]->LinkLocation = RecompPos - 4;
			}
		}
	}
	for (count = 0; count < 2; count ++) {
		if (JumpInfo[count]->FallThrough) {
			if (JumpInfo[count]->TargetPC < Section->CompilePC) {
				DWORD CycleCount = JumpInfo[count]->RegSet.CycleCount;;
				if (JumpInfo[count]->RegSet.RandomModifier != 0) {
					SubConstFromVariable(JumpInfo[count]->RegSet.RandomModifier,&CP0[1]);
					JumpInfo[count]->RegSet.RandomModifier = 0;
				}
				if (JumpInfo[count]->RegSet.CycleCount != 0) {
					AddConstToVariable(JumpInfo[count]->RegSet.CycleCount,&CP0[9]);
				}
				JumpInfo[count]->RegSet.CycleCount = 0;
				CompileSystemCheck(CycleCount,JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet);
			}
		}
	}
	for (count = 0; count < 2; count ++) {
		if (JumpInfo[count]->FallThrough) {
			GenerateX86Code(TargetSection[count],GetNewTestValue());
		}
	}
	for (count = 0; count < 2; count ++) {
		if (JumpInfo[count]->LinkLocation == NULL) continue;
		if (TargetSection[count] == NULL) {
			SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
			JumpInfo[count]->LinkLocation = NULL;
			if (JumpInfo[count]->LinkLocation2 != NULL) {
				SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
				JumpInfo[count]->LinkLocation2 = NULL;
			}
			CompileExit (JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet,Normal,TRUE,NULL);
			continue;
		}
		if (TargetSection[count]->CompiledLocation == NULL) {
			GenerateX86Code(TargetSection[count],GetNewTestValue());
		} else {
			SetJump32(JumpInfo[count]->LinkLocation,RecompPos);
			JumpInfo[count]->LinkLocation = NULL;
			if (JumpInfo[count]->LinkLocation2 != NULL) {
				SetJump32(JumpInfo[count]->LinkLocation2,RecompPos);
				JumpInfo[count]->LinkLocation2 = NULL;
			}
			memcpy(&Section->RegWorking,&JumpInfo[count]->RegSet,sizeof(REG_INFO));
			if (JumpInfo[count]->RegSet.RandomModifier != 0) {
				SubConstFromVariable(JumpInfo[count]->RegSet.RandomModifier,&CP0[1]);
				JumpInfo[count]->RegSet.RandomModifier = 0;
			}
			if (JumpInfo[count]->RegSet.CycleCount != 0) {
				AddConstToVariable(JumpInfo[count]->RegSet.CycleCount,&CP0[9]);
			}
			if (JumpInfo[count]->TargetPC <= Section->CompilePC) {
				DWORD CycleCount = JumpInfo[count]->RegSet.CycleCount;
				JumpInfo[count]->RegSet.CycleCount = 0;
				if (JumpInfo[count]->PermLoop) {
					MoveConstToVariable(JumpInfo[count]->TargetPC,&PROGRAM_COUNTER);
					Call_Direct(InPermLoop);
					CompileSystemCheck(0,-1,JumpInfo[count]->RegSet);
				} else {
					CompileSystemCheck(CycleCount,JumpInfo[count]->TargetPC,JumpInfo[count]->RegSet);
				}
			} else {
				if (JumpInfo[count]->RegSet.CycleCount != 0) {
					SubConstFromVariable(JumpInfo[count]->RegSet.CycleCount,&Timers.Timer);
					JumpInfo[count]->RegSet.CycleCount = 0;
				}
			}
			memcpy(&Section->RegWorking,&JumpInfo[count]->RegSet,sizeof(REG_INFO));
			SyncRegState(Section,&TargetSection[count]->RegStart);
			JmpLabel32(0);
			SetJump32((DWORD *)RecompPos - 1,TargetSection[count]->CompiledLocation);
		}
	}
}
BOOL GenerateX86Code (BLOCK_SECTION * Section,DWORD Test) {
	int count;
	if (Section == NULL) return FALSE;
	if (Section->CompiledLocation != NULL) {
		if (Section->Test == Test) return FALSE;
		Section->Test = Test;
		if (GenerateX86Code(Section->ContinueSection,Test) || GenerateX86Code(Section->JumpSection,Test)) return TRUE;
		return FALSE;
	}
	if (Section->ParentSection) {
		for (count = 0;Section->ParentSection[count] != NULL;count++) {
			BLOCK_SECTION * Parent;
			Parent = Section->ParentSection[count];
			if (Parent->CompiledLocation != NULL) continue;
			if (IsAllParentLoops(Section,Parent,TRUE,GetNewTestValue())) continue;
			return FALSE;
		}
	}
	if (!InheritParentInfo(Section)) return FALSE;
	Section->CompiledLocation = RecompPos;
	Section->CompilePC = Section->StartPC;
	SetNormal
	do {
		__try {
			if (BlockCycleCount > 1 && CF1CF0) BlockCycleCount = 1;
			if (!r4300i_LW_VAddr(Section->CompilePC,&Opcode.Hex)) DisplayThreadExit("GenerateX86Code - !r4300i_LW_VAddr(Section->CompilePC,&Opcode.Hex)\n\nSelf-modifying Code Method-related?");
		} __except(r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())) {
			DisplayThreadExit("GenerateX86Code - r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation()");
		}
		BlockCycleCount += CountPerOp;
		BlockRandomModifier += 1;
		for (count = 1; count < 10; count ++) { x86Protected(count) = FALSE; }
		switch (Opcode.op) {
		case R4300i_SPECIAL:
			switch (Opcode.funct) {
			case R4300i_SPECIAL_BREAK:
			{
				static BOOL Shown = FALSE;
				if (Shown) break;
				HandleModal1(hMainWindow);
				if (MessageBox(NULL,GS(SPECIAL_BREAK),GS(MSG_ERROR_TITLE),MB_YESNO | MB_ICONERROR | MB_SETFOREGROUND) == IDNO) Shown = TRUE;
				HandleModal2(hMainWindow);
				if (Shown) break;
			}
			case R4300i_SPECIAL_SYSCALL: Compile_R4300i_SPECIAL_SYSCALL(Section);
			case R4300i_SPECIAL_SYNC: // Used by older versions of OoT GZ (ROMhack)
			case R4300i_SPECIAL_TGE:
			case R4300i_SPECIAL_TGEU:
			case R4300i_SPECIAL_TLT:
			case R4300i_SPECIAL_TLTU:
			case R4300i_SPECIAL_TEQ: // Used by Xeno Crisis (Commercial Homebrew) and potentially some ROMhacks
			case R4300i_SPECIAL_TNE:
			break;
			case R4300i_SPECIAL_SLL: Compile_R4300i_SPECIAL_SLL(Section); break;
			case R4300i_SPECIAL_SRL: Compile_R4300i_SPECIAL_SRL(Section); break;
			case R4300i_SPECIAL_SRA: Compile_R4300i_SPECIAL_SRA(Section); break;
			case R4300i_SPECIAL_SLLV: Compile_R4300i_SPECIAL_SLLV(Section); break;
			case R4300i_SPECIAL_SRLV: Compile_R4300i_SPECIAL_SRLV(Section); break;
			case R4300i_SPECIAL_SRAV: Compile_R4300i_SPECIAL_SRAV(Section); break;
			case R4300i_SPECIAL_JR: Compile_R4300i_SPECIAL_JR(Section); break;
			case R4300i_SPECIAL_JALR: Compile_R4300i_SPECIAL_JALR(Section); break;
			case R4300i_SPECIAL_MFHI: Compile_R4300i_SPECIAL_MFHI(Section); break;
			case R4300i_SPECIAL_MTHI: Compile_R4300i_SPECIAL_MTHI(Section); break;
			case R4300i_SPECIAL_MFLO: Compile_R4300i_SPECIAL_MFLO(Section); break;
			case R4300i_SPECIAL_MTLO: Compile_R4300i_SPECIAL_MTLO(Section); break;
			case R4300i_SPECIAL_DSLLV: Compile_R4300i_SPECIAL_DSLLV(Section); break;
			case R4300i_SPECIAL_DSRLV: Compile_R4300i_SPECIAL_DSRLV(Section); break;
			case R4300i_SPECIAL_DSRAV: Compile_R4300i_SPECIAL_DSRAV(Section); break;
			case R4300i_SPECIAL_MULT: Compile_R4300i_SPECIAL_MULT(Section); break;
			case R4300i_SPECIAL_MULTU: Compile_R4300i_SPECIAL_MULTU(Section); break;
			case R4300i_SPECIAL_DIV: Compile_R4300i_SPECIAL_DIV(Section); break;
			case R4300i_SPECIAL_DIVU: Compile_R4300i_SPECIAL_DIVU(Section); break;
			case R4300i_SPECIAL_DMULT: Compile_R4300i_SPECIAL_DMULT(Section); break;
			case R4300i_SPECIAL_DMULTU: Compile_R4300i_SPECIAL_DMULTU(Section); break;
			case R4300i_SPECIAL_DDIV: Compile_R4300i_SPECIAL_DDIV(Section); break;
			case R4300i_SPECIAL_DDIVU: Compile_R4300i_SPECIAL_DDIVU(Section); break;
			case R4300i_SPECIAL_ADD: Compile_R4300i_SPECIAL_ADD(Section); break;
			case R4300i_SPECIAL_ADDU: Compile_R4300i_SPECIAL_ADDU(Section); break;
			case R4300i_SPECIAL_SUB: Compile_R4300i_SPECIAL_SUB(Section); break;
			case R4300i_SPECIAL_SUBU: Compile_R4300i_SPECIAL_SUBU(Section); break;
			case R4300i_SPECIAL_AND: Compile_R4300i_SPECIAL_AND(Section); break;
			case R4300i_SPECIAL_OR: Compile_R4300i_SPECIAL_OR(Section); break;
			case R4300i_SPECIAL_XOR: Compile_R4300i_SPECIAL_XOR(Section); break;
			case R4300i_SPECIAL_NOR: Compile_R4300i_SPECIAL_NOR(Section); break;
			case R4300i_SPECIAL_SLT: Compile_R4300i_SPECIAL_SLT(Section); break;
			case R4300i_SPECIAL_SLTU: Compile_R4300i_SPECIAL_SLTU(Section); break;
			case R4300i_SPECIAL_DADD: Compile_R4300i_SPECIAL_DADD(Section); break;
			case R4300i_SPECIAL_DADDU: Compile_R4300i_SPECIAL_DADDU(Section); break;
			case R4300i_SPECIAL_DSUB: Compile_R4300i_SPECIAL_DSUB(Section); break;
			case R4300i_SPECIAL_DSUBU: Compile_R4300i_SPECIAL_DSUBU(Section); break;
			case R4300i_SPECIAL_DSLL: Compile_R4300i_SPECIAL_DSLL(Section); break;
			case R4300i_SPECIAL_DSRL: Compile_R4300i_SPECIAL_DSRL(Section); break;
			case R4300i_SPECIAL_DSRA: Compile_R4300i_SPECIAL_DSRA(Section); break;
			case R4300i_SPECIAL_DSLL32: Compile_R4300i_SPECIAL_DSLL32(Section); break;
			case R4300i_SPECIAL_DSRL32: Compile_R4300i_SPECIAL_DSRL32(Section); break;
			case R4300i_SPECIAL_DSRA32: Compile_R4300i_SPECIAL_DSRA32(Section); break;
			default:
				DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_SPECIAL: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on a reserved Opcode at this location");
			}
		break;
		case R4300i_REGIMM:
			switch (Opcode.rt) {
			case R4300i_REGIMM_BLTZ: Compile_R4300i_Branch(Section,BLTZ_Compare,BranchTypeRs,FALSE); break;
			case R4300i_REGIMM_BGEZ: Compile_R4300i_Branch(Section,BGEZ_Compare,BranchTypeRs,FALSE); break;
			case R4300i_REGIMM_BLTZL: Compile_R4300i_BranchLikely(Section,BLTZ_Compare,FALSE); break;
			case R4300i_REGIMM_BGEZL: Compile_R4300i_BranchLikely(Section,BGEZ_Compare,FALSE);
			case R4300i_REGIMM_TGEI:
			case R4300i_REGIMM_TGEIU:
			case R4300i_REGIMM_TLTI:
			case R4300i_REGIMM_TLTIU:
			case R4300i_REGIMM_TEQI:
			case R4300i_REGIMM_TNEI:
			case R4300i_REGIMM_BLTZALL:
			case R4300i_REGIMM_BGEZALL:
			break;
			case R4300i_REGIMM_BLTZAL: Compile_R4300i_Branch(Section,BLTZ_Compare,BranchTypeRs,TRUE); break;
			case R4300i_REGIMM_BGEZAL: Compile_R4300i_Branch(Section,BGEZ_Compare,BranchTypeRs,TRUE); break;
			default:
				DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_REGIMM: - switch (Opcode.rt) - default:\n\nThe emulator has crashed on a reserved Opcode at this location");
			}
		break;
		case R4300i_J: Compile_R4300i_J(Section); break;
		case R4300i_JAL: Compile_R4300i_JAL(Section); break;
		case R4300i_BEQ: Compile_R4300i_Branch(Section,BEQ_Compare,BranchTypeRsRt,FALSE); break;
		case R4300i_BNE: Compile_R4300i_Branch(Section,BNE_Compare,BranchTypeRsRt,FALSE); break;
		case R4300i_BLEZ: Compile_R4300i_Branch(Section,BLEZ_Compare,BranchTypeRs,FALSE); break;
		case R4300i_BGTZ: Compile_R4300i_Branch(Section,BGTZ_Compare,BranchTypeRs,FALSE); break;
		case R4300i_ADDI: Compile_R4300i_ADDI(Section); break;
		case R4300i_ADDIU: Compile_R4300i_ADDIU(Section); break;
		case R4300i_SLTI: Compile_R4300i_SLTI(Section); break;
		case R4300i_SLTIU: Compile_R4300i_SLTIU(Section); break;
		case R4300i_ANDI: Compile_R4300i_ANDI(Section); break;
		case R4300i_ORI: Compile_R4300i_ORI(Section); break;
		case R4300i_XORI: Compile_R4300i_XORI(Section); break;
		case R4300i_LUI: Compile_R4300i_LUI(Section); break;
		case R4300i_CP0:
			switch (Opcode.rs) {
			case R4300i_COP0_MF: Compile_R4300i_COP0_MF(Section); break;
			case R4300i_COP0_MT: Compile_R4300i_COP0_MT(Section);
			case R4300i_COP0_DMF:
			case R4300i_COP0_DMT:
			break;
			default:
				if ((Opcode.rs & 0x10) != 0) {
					switch (Opcode.funct) {
					case R4300i_COP0_CO_TLBR: Compile_R4300i_COP0_CO_TLBR(Section); break;
					case R4300i_COP0_CO_TLBWI: Compile_R4300i_COP0_CO_TLBWI(Section); break;
					case R4300i_COP0_CO_TLBWR: Compile_R4300i_COP0_CO_TLBWR(Section); break;
					case R4300i_COP0_CO_TLBP: Compile_R4300i_COP0_CO_TLBP(Section); break;
					case R4300i_COP0_CO_ERET: Compile_R4300i_COP0_CO_ERET(Section); break;
					default:
						DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP0: - switch (Opcode.rs) - default: - (Opcode.rs & 0x10) != 0) - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
					}
				} else DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP0: - switch (Opcode.rs) - default: - (Opcode.rs & 0x10) != 0) - else\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		case R4300i_CP1:
			switch (Opcode.rs) {
			case R4300i_COP1_MF: Compile_R4300i_COP1_MF(Section); break;
			case R4300i_COP1_DMF: Compile_R4300i_COP1_DMF(Section); break;
			case R4300i_COP1_CF: Compile_R4300i_COP1_CF(Section); break;
			case R4300i_COP1_MT: Compile_R4300i_COP1_MT(Section); break;
			case R4300i_COP1_DMT: Compile_R4300i_COP1_DMT(Section); break;
			case R4300i_COP1_CT: Compile_R4300i_COP1_CT(Section);
			case R4300i_COP1_DCF:
			case R4300i_COP1_DCT:
			break;
			case R4300i_COP1_BC:
				switch (Opcode.ft) {
				case R4300i_COP1_BC_BCF: Compile_R4300i_Branch(Section,COP1_BCF_Compare,BranchTypeCop1,FALSE); break;
				case R4300i_COP1_BC_BCT: Compile_R4300i_Branch(Section,COP1_BCT_Compare,BranchTypeCop1,FALSE); break;
				case R4300i_COP1_BC_BCFL: Compile_R4300i_BranchLikely(Section,COP1_BCF_Compare,FALSE); break;
				case R4300i_COP1_BC_BCTL: Compile_R4300i_BranchLikely(Section,COP1_BCT_Compare,FALSE); break;
				default:
					DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_BC: - switch (Opcode.ft) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				}
			break;
			case R4300i_COP1_S:
				switch (Opcode.funct) {
				case R4300i_COP1_FUNCT_ADD: Compile_R4300i_COP1_S_ADD(Section); break;
				case R4300i_COP1_FUNCT_SUB: Compile_R4300i_COP1_S_SUB(Section); break;
				case R4300i_COP1_FUNCT_MUL: Compile_R4300i_COP1_S_MUL(Section); break;
				case R4300i_COP1_FUNCT_DIV: Compile_R4300i_COP1_S_DIV(Section); break;
				case R4300i_COP1_FUNCT_SQRT: Compile_R4300i_COP1_S_SQRT(Section); break;
				case R4300i_COP1_FUNCT_ABS: Compile_R4300i_COP1_S_ABS(Section); break;
				case R4300i_COP1_FUNCT_MOV: Compile_R4300i_COP1_S_MOV(Section); break;
				case R4300i_COP1_FUNCT_NEG: Compile_R4300i_COP1_S_NEG(Section); break;
				case R4300i_COP1_FUNCT_TRUNC_L: Compile_R4300i_COP1_S_TRUNC_L(Section); break;
				case R4300i_COP1_FUNCT_CEIL_L: Compile_R4300i_COP1_S_CEIL_L(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_FLOOR_L: Compile_R4300i_COP1_S_FLOOR_L(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_ROUND_W: Compile_R4300i_COP1_S_ROUND_W(Section); break;
				case R4300i_COP1_FUNCT_TRUNC_W: Compile_R4300i_COP1_S_TRUNC_W(Section); break;
				case R4300i_COP1_FUNCT_CEIL_W: Compile_R4300i_COP1_S_CEIL_W(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_FLOOR_W: Compile_R4300i_COP1_S_FLOOR_W(Section); break;
				case R4300i_COP1_FUNCT_CVT_D: Compile_R4300i_COP1_S_CVT_D(Section); break;
				case R4300i_COP1_FUNCT_CVT_W: Compile_R4300i_COP1_S_CVT_W(Section); break;
				case R4300i_COP1_FUNCT_CVT_L: Compile_R4300i_COP1_S_CVT_L(Section); break;
				case R4300i_COP1_FUNCT_C_F:   case R4300i_COP1_FUNCT_C_UN:
				case R4300i_COP1_FUNCT_C_EQ:  case R4300i_COP1_FUNCT_C_UEQ:
				case R4300i_COP1_FUNCT_C_OLT: case R4300i_COP1_FUNCT_C_ULT:
				case R4300i_COP1_FUNCT_C_OLE: case R4300i_COP1_FUNCT_C_ULE:
				case R4300i_COP1_FUNCT_C_SF:  case R4300i_COP1_FUNCT_C_NGLE:
				case R4300i_COP1_FUNCT_C_SEQ: case R4300i_COP1_FUNCT_C_NGL:
				case R4300i_COP1_FUNCT_C_LT:  case R4300i_COP1_FUNCT_C_NGE:
				case R4300i_COP1_FUNCT_C_LE:  case R4300i_COP1_FUNCT_C_NGT:
					Compile_R4300i_COP1_S_CMP(Section);
				case R4300i_COP1_FUNCT_ROUND_L:
				case R4300i_COP1_FUNCT_CVT_S:
					break;
				default:
					DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_S: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				}
			break;
			case R4300i_COP1_D:
				switch (Opcode.funct) {
				case R4300i_COP1_FUNCT_ADD: Compile_R4300i_COP1_D_ADD(Section); break;
				case R4300i_COP1_FUNCT_SUB: Compile_R4300i_COP1_D_SUB(Section); break;
				case R4300i_COP1_FUNCT_MUL: Compile_R4300i_COP1_D_MUL(Section); break;
				case R4300i_COP1_FUNCT_DIV: Compile_R4300i_COP1_D_DIV(Section); break;
				case R4300i_COP1_FUNCT_SQRT: Compile_R4300i_COP1_D_SQRT(Section); break;
				case R4300i_COP1_FUNCT_ABS: Compile_R4300i_COP1_D_ABS(Section); break;
				case R4300i_COP1_FUNCT_MOV: Compile_R4300i_COP1_D_MOV(Section); break;
				case R4300i_COP1_FUNCT_NEG: Compile_R4300i_COP1_D_NEG(Section); break;
				case R4300i_COP1_FUNCT_TRUNC_L: Compile_R4300i_COP1_D_TRUNC_L(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_CEIL_L: Compile_R4300i_COP1_D_CEIL_L(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_FLOOR_L: Compile_R4300i_COP1_D_FLOOR_L(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_ROUND_W: Compile_R4300i_COP1_D_ROUND_W(Section); break;
				case R4300i_COP1_FUNCT_TRUNC_W: Compile_R4300i_COP1_D_TRUNC_W(Section); break;
				case R4300i_COP1_FUNCT_CEIL_W: Compile_R4300i_COP1_D_CEIL_W(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_FLOOR_W: Compile_R4300i_COP1_D_FLOOR_W(Section); break;	//added by Witten
				case R4300i_COP1_FUNCT_CVT_S: Compile_R4300i_COP1_D_CVT_S(Section); break;
				case R4300i_COP1_FUNCT_CVT_W: Compile_R4300i_COP1_D_CVT_W(Section); break;
				case R4300i_COP1_FUNCT_CVT_L: Compile_R4300i_COP1_D_CVT_L(Section); break;
				case R4300i_COP1_FUNCT_C_F:   case R4300i_COP1_FUNCT_C_UN:
				case R4300i_COP1_FUNCT_C_EQ:  case R4300i_COP1_FUNCT_C_UEQ:
				case R4300i_COP1_FUNCT_C_OLT: case R4300i_COP1_FUNCT_C_ULT:
				case R4300i_COP1_FUNCT_C_OLE: case R4300i_COP1_FUNCT_C_ULE:
				case R4300i_COP1_FUNCT_C_SF:  case R4300i_COP1_FUNCT_C_NGLE:
				case R4300i_COP1_FUNCT_C_SEQ: case R4300i_COP1_FUNCT_C_NGL:
				case R4300i_COP1_FUNCT_C_LT:  case R4300i_COP1_FUNCT_C_NGE:
				case R4300i_COP1_FUNCT_C_LE:  case R4300i_COP1_FUNCT_C_NGT:
					Compile_R4300i_COP1_D_CMP(Section);
				case R4300i_COP1_FUNCT_ROUND_L:
				case R4300i_COP1_FUNCT_CVT_D:
					break;
				default:
					DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_D: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				}
			break;
			case R4300i_COP1_W:
				switch (Opcode.funct) {
				case R4300i_COP1_FUNCT_CVT_S: Compile_R4300i_COP1_W_CVT_S(Section); break;
				case R4300i_COP1_FUNCT_CVT_D: Compile_R4300i_COP1_W_CVT_D(Section); break;
				default:
					DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_W: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				}
				break;
			case R4300i_COP1_L:
				switch (Opcode.funct) {
				case R4300i_COP1_FUNCT_CVT_S: Compile_R4300i_COP1_L_CVT_S(Section); break;
				case R4300i_COP1_FUNCT_CVT_D: Compile_R4300i_COP1_L_CVT_D(Section); break;
				default:
					DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP1: - switch (Opcode.rs) - case R4300i_COP1_L: - switch (Opcode.funct) - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				}
				break;
			default:
				DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP1: - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
				break;
			}
		break;
		case R4300i_CP2:
			switch (Opcode.rs) {
			case R4300i_COP2_MF:
			case R4300i_COP2_DMF:
			case R4300i_COP2_CF:
			case R4300i_COP2_MT:
			case R4300i_COP2_DMT:
			case R4300i_COP2_CT:
			case R4300i_COP2_DCF:
			case R4300i_COP2_DCT:
			break;
			default:
				DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - case R4300i_CP2: - default:\n\nThe emulator has crashed on an unknown Opcode at this location");
			}
		break;
		case R4300i_BEQL: Compile_R4300i_BranchLikely(Section,BEQ_Compare,FALSE); break;
		case R4300i_BNEL: Compile_R4300i_BranchLikely(Section,BNE_Compare,FALSE); break;
		case R4300i_BLEZL: Compile_R4300i_BranchLikely(Section,BLEZ_Compare,FALSE); break;
		case R4300i_BGTZL: Compile_R4300i_BranchLikely(Section,BGTZ_Compare,FALSE); break;
		case R4300i_DADDI:
		case R4300i_DADDIU: Compile_R4300i_DADDIU(Section); break;
		case R4300i_LDL: Compile_R4300i_LDL(Section); break;
		case R4300i_LDR: Compile_R4300i_LDR(Section); break;
		case R4300i_LB: Compile_R4300i_LB(Section); break;
		case R4300i_LH: Compile_R4300i_LH(Section); break;
		case R4300i_LWL: Compile_R4300i_LWL(Section); break;
		case R4300i_LW: Compile_R4300i_LW(Section); break;
		case R4300i_LBU: Compile_R4300i_LBU(Section); break;
		case R4300i_LHU: Compile_R4300i_LHU(Section); break;
		case R4300i_LWR: Compile_R4300i_LWR(Section); break;
		case R4300i_LWU: Compile_R4300i_LWU(Section); break;	//added by Witten
		case R4300i_SB: Compile_R4300i_SB(Section); break;
		case R4300i_SH: Compile_R4300i_SH(Section); break;
		case R4300i_SWL: Compile_R4300i_SWL(Section); break;
		case R4300i_SW: Compile_R4300i_SW(Section); break;
		case R4300i_SDL: Compile_R4300i_SDL(Section); break;
		case R4300i_SWR: Compile_R4300i_SWR(Section); break;
		case R4300i_SDR: Compile_R4300i_SDR(Section); break;
		case R4300i_CACHE: Compile_R4300i_CACHE(Section); break;
		case R4300i_LL: Compile_R4300i_LL(Section); break;
		case R4300i_LWC1: Compile_R4300i_LWC1(Section);
		case R4300i_CP3:
		case R4300i_LWC2:
		case R4300i_LLD:
		case R4300i_LDC2:
		case R4300i_SWC2:
		case R4300i_SCD:
		case R4300i_SDC2:
		break;
		case R4300i_LDC1: Compile_R4300i_LDC1(Section); break;
		case R4300i_SC: Compile_R4300i_SC(Section); break;
		case R4300i_LD: Compile_R4300i_LD(Section); break;
		case R4300i_SWC1: Compile_R4300i_SWC1(Section); break;
		case R4300i_SDC1: Compile_R4300i_SDC1(Section); break;
		case R4300i_SD: Compile_R4300i_SD(Section); break;
		default:
			if (SelfModCheck == ModCode_ProtectMemory) DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - default:\n\nThe emulator has crashed on a reserved Opcode at this location");
			else {
				if (SelfModCheck == ModCode_CheckSetMemory) {
					DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - default:\n\nThe emulator has crashed on a reserved Opcode at this location\n\nNeeds 'Self-modifying Code Method=Protect Memory'?");
				} else DisplayThreadExit("GenerateX86Code - switch (Opcode.op) - default:\n\nThe emulator has crashed on a reserved Opcode at this location\n\nNeeds 'Self-modifying Code Method=Check & Set Memory'?");
			}
			break;
		}
		if (UseCache == REG_CACHE_OFF) { WriteBackRegisters(Section); }
		for (count = 1; count < 10; count ++) { x86Protected(count) = FALSE; }
		UnMap_AllFPRs(Section);
		if ((Section->CompilePC &0xFFC) == 0xFFC) {
			if (NextInstruction == NORMAL) {
				CompileExit (Section->CompilePC + 4,Section->RegWorking,Normal,TRUE,NULL);
				SetEnd
			}
		}
		switch (NextInstruction) {
		case NORMAL:
			Section->CompilePC += 4;
			break;
		case DO_DELAY_SLOT:
			SetDelay
			Section->CompilePC += 4;
			break;
		case DELAY_SLOT:
			NextInstruction = DELAY_SLOT_DONE;
			BlockCycleCount -= CountPerOp;
			BlockRandomModifier -= 1;
			SectionCPC4
			break;
		}
	} while (NextInstruction != END_BLOCK);
	return TRUE;
}
DWORD GetNewTestValue(void) {
	static DWORD LastTest = 0;
	if (LastTest == 0xFFFFFFFF) { LastTest = 0; }
	LastTest += 1;
	return LastTest;
}
void InitializeRegSet(REG_INFO * RegSet) {
	int count;
	RegSet->MIPS_RegState[0]  = STATE_CONST_32;
	RegSet->MIPS_RegVal[0].DW = 0;
	for (count = 1; count < 32; count ++) {
		RegSet->MIPS_RegState[count]   = STATE_UNKNOWN;
		RegSet->MIPS_RegVal[count].DW = 0;
	}
	for (count = 0; count < 10; count ++) {
		RegSet->x86reg_MappedTo[count]  = NotMapped;
		RegSet->x86reg_Protected[count] = FALSE;
		RegSet->x86reg_MapOrder[count]  = 0;
	}
	RegSet->CycleCount = 0;
	RegSet->RandomModifier = 0;
	RegSet->Stack_TopPos = 0;
	for (count = 0; count < 8; count ++) {
		RegSet->x86fpu_MappedTo[count] = -1;
		RegSet->x86fpu_State[count] = FPU_Unkown;
		RegSet->x86fpu_RoundingModel[count] = RoundDefault;
	}
	RegSet->Fpu_Used = FALSE;
	RegSet->RoundingModel = RoundUnknown;
}
void _fastcall InheritConstants(BLOCK_SECTION * Section) {
	int NoOfParents,count;
	BLOCK_SECTION * Parent;
	REG_INFO * RegSet;
	if (Section->ParentSection == NULL) {
		InitializeRegSet(&Section->RegStart);
		memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
		return;
	}
	Parent = Section->ParentSection[0];
	RegSet = Section == Parent->ContinueSection?&Parent->Cont.RegSet:&Parent->Jump.RegSet;
	memcpy(&Section->RegStart,RegSet,sizeof(REG_INFO));
	memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
	for (NoOfParents = 1;Section->ParentSection[NoOfParents] != NULL;NoOfParents++) {
		Parent = Section->ParentSection[NoOfParents];
		RegSet = Section == Parent->ContinueSection?&Parent->Cont.RegSet:&Parent->Jump.RegSet;
		for (count = 0; count < 32; count++) {
			if (IsConst(count)) {
				if (MipsRegState(count) != RegSet->MIPS_RegState[count]) {
					MipsRegState(count) = STATE_UNKNOWN;
				} else if (Is32Bit(count) && MipsRegLo(count) != RegSet->MIPS_RegVal[count].UW[0]) {
					MipsRegState(count) = STATE_UNKNOWN;
				} else if (Is64Bit(count) && MipsReg(count) != RegSet->MIPS_RegVal[count].UDW) {
					MipsRegState(count) = STATE_UNKNOWN;
				}
			}
		}
	}
	memcpy(&Section->RegStart,&Section->RegWorking,sizeof(REG_INFO));
}
BOOL InheritParentInfo (BLOCK_SECTION * Section) {
	int count,start,NoOfParents,NoOfCompiledParents,FirstParent,CurrentParent;
	BLOCK_PARENT * SectionParents;
	BLOCK_SECTION * Parent;
	JUMP_INFO * JumpInfo;
	BOOL NeedSync;
	DisplaySectionInformation(Section,Section->SectionID,GetNewTestValue());
	if (Section->ParentSection == NULL) {
		InitializeRegSet(&Section->RegStart);
		memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
		return TRUE;
	}
	NoOfParents = 0;
	for (count = 0;Section->ParentSection[count] != NULL;count++) {
		Parent = Section->ParentSection[count];
		NoOfParents += Parent->JumpSection != Parent->ContinueSection?1:2;
	}
	if (NoOfParents == 0) {
		return FALSE;
	} else if (NoOfParents == 1) {
		Parent = Section->ParentSection[0];
		if (Section == Parent->ContinueSection) { JumpInfo = &Parent->Cont; }
		else if (Section == Parent->JumpSection) { JumpInfo = &Parent->Jump; }
		memcpy(&Section->RegStart,&JumpInfo->RegSet,sizeof(REG_INFO));
		if (JumpInfo->LinkLocation != NULL) {
			SetJump32(JumpInfo->LinkLocation,RecompPos);
			if (JumpInfo->LinkLocation2 != NULL) {
				SetJump32(JumpInfo->LinkLocation2,RecompPos);
			}
		}
		memcpy(&Section->RegWorking,&Section->RegStart,sizeof(REG_INFO));
		return TRUE;
	}
	//Multiple Parents
	for (count = 0,NoOfCompiledParents = 0;Section->ParentSection[count] != NULL;count++) {
		Parent = Section->ParentSection[count];
		if (Parent->CompiledLocation != NULL) {
			NoOfCompiledParents += Parent->JumpSection != Parent->ContinueSection?1:2;
		}
	}
	if (NoOfCompiledParents == 0) return FALSE;
	SectionParents = (BLOCK_PARENT *)malloc(NoOfParents * sizeof(BLOCK_PARENT));
	for (count = 0,NoOfCompiledParents = 0;Section->ParentSection[count] != NULL;count++) {
		Parent = Section->ParentSection[count];
		if (Parent->CompiledLocation == NULL) continue;
		if (Parent->JumpSection != Parent->ContinueSection) {
			SectionParents[NoOfCompiledParents].Parent = Parent;
			SectionParents[NoOfCompiledParents].JumpInfo =
				Section == Parent->ContinueSection?&Parent->Cont:&Parent->Jump;
			NoOfCompiledParents += 1;
		} else {
			SectionParents[NoOfCompiledParents].Parent = Parent;
			SectionParents[NoOfCompiledParents].JumpInfo = &Parent->Cont;
			NoOfCompiledParents += 1;
			SectionParents[NoOfCompiledParents].Parent = Parent;
			SectionParents[NoOfCompiledParents].JumpInfo = &Parent->Jump;
			NoOfCompiledParents += 1;
		}
	}
	start = NoOfCompiledParents;
	for (count = 0;Section->ParentSection[count] != NULL;count++) {
		Parent = Section->ParentSection[count];
		if (Parent->CompiledLocation != NULL) continue;
		if (Parent->JumpSection != Parent->ContinueSection) {
			SectionParents[start].Parent = Parent;
			SectionParents[start].JumpInfo =
				Section == Parent->ContinueSection?&Parent->Cont:&Parent->Jump;
			start += 1;
		} else {
			SectionParents[start].Parent = Parent;
			SectionParents[start].JumpInfo = &Parent->Cont;
			start += 1;
			SectionParents[start].Parent = Parent;
			SectionParents[start].JumpInfo = &Parent->Jump;
			start += 1;
		}
	}
	FirstParent = 0;
	for (count = 1;count < NoOfCompiledParents;count++) {
		if (SectionParents[count].JumpInfo->FallThrough) {
			FirstParent = count; break;
		}
	}
	//Link First Parent to start
	Parent = SectionParents[FirstParent].Parent;
	JumpInfo = SectionParents[FirstParent].JumpInfo;
	memcpy(&Section->RegWorking,&JumpInfo->RegSet,sizeof(REG_INFO));
	if (JumpInfo->LinkLocation != NULL) {
		SetJump32(JumpInfo->LinkLocation,RecompPos);
		JumpInfo->LinkLocation  = NULL;
		if (JumpInfo->LinkLocation2 != NULL) {
			SetJump32(JumpInfo->LinkLocation2,RecompPos);
			JumpInfo->LinkLocation2  = NULL;
		}
	}
	if (BlockRandomModifier != 0) { SubConstFromVariable(BlockRandomModifier,&CP0[1]); }
	if (BlockCycleCount != 0) {
		AddConstToVariable(BlockCycleCount,&CP0[9]);
		SubConstFromVariable(BlockCycleCount,&Timers.Timer);
	}
	JumpInfo->FallThrough   = FALSE;
	//Fix up initial state
	UnMap_AllFPRs(Section);
	for (count = 0;count < NoOfParents;count++) {
		int count2,count3,MemoryStackPos;
		REG_INFO * RegSet;
		if (count == FirstParent) continue;
		Parent = SectionParents[count].Parent;
		RegSet = &SectionParents[count].JumpInfo->RegSet;
		if (CurrentRoundingModel != RegSet->RoundingModel) { CurrentRoundingModel = RoundUnknown; }
		if (NoOfParents != NoOfCompiledParents) { CurrentRoundingModel = RoundUnknown; }
		//Find Parent MapRegState
		MemoryStackPos = -1;
		for (count2 = 1; count2 < 10; count2++) {
			if (RegSet->x86reg_MappedTo[count2] == Stack_Mapped) {
				MemoryStackPos = count2;
				break;
			}
		}
		if (MemoryStackPos < 0) {
			if (Map_MemoryStack(Section,FALSE) > 0) {
				UnMap_X86reg(Section,Map_MemoryStack(Section,FALSE));
			}
		}
		for (count2 = 1; count2 < 32; count2++) {
			if (Is32BitMapped(count2)) {
				switch (RegSet->MIPS_RegState[count2]) {
				case STATE_MAPPED_64: Map_GPR_64bit(Section,count2,count2);
				case STATE_MAPPED_32_ZERO: break;
				case STATE_MAPPED_32_SIGN:
					if (IsUnsigned(count2)) {
						MipsRegState(count2) = STATE_MAPPED_32_SIGN;
					}
					break;
				case STATE_UNKNOWN:
				case STATE_CONST_64: Map_GPR_64bit(Section,count2,count2); break;
				case STATE_CONST_32:
					if ((RegSet->MIPS_RegVal[count2].W[0] < 0) && IsUnsigned(count2)) {
						MipsRegState(count2) = STATE_MAPPED_32_SIGN;
					}
					break;
				}
			}
			if (IsConst(count2)) {
				if (MipsRegState(count2) != RegSet->MIPS_RegState[count2]) {
					if (Is32Bit(count2)) {
						Map_GPR_32bit(Section,count2,TRUE,count2);
					} else {
						Map_GPR_32bit(Section,count2,TRUE,count2);
					}
				} else if (Is32Bit(count2) && MipsRegLo(count2) != RegSet->MIPS_RegVal[count2].UW[0]) {
					Map_GPR_32bit(Section,count2,TRUE,count2);
				} else if (Is64Bit(count2) && MipsReg(count2) != RegSet->MIPS_RegVal[count2].UDW) {
					Map_GPR_32bit(Section,count2,TRUE,count2);
				}
			}
			for (count3 = 1; count3 < 10; count3 ++) { x86Protected(count3) = FALSE; }
		}
	}
	memcpy(&Section->RegStart,&Section->RegWorking,sizeof(REG_INFO));
	//Sync registers for different blocks
	CurrentParent = FirstParent;
	NeedSync = FALSE;
	for (count = 0;count < NoOfCompiledParents;count++) {
		REG_INFO * RegSet;
		int count2;
		if (count == FirstParent) continue;
		Parent    = SectionParents[count].Parent;
		JumpInfo = SectionParents[count].JumpInfo;
		RegSet   = &SectionParents[count].JumpInfo->RegSet;
		if (JumpInfo->RegSet.CycleCount != 0) { NeedSync = TRUE; }
		if (JumpInfo->RegSet.RandomModifier  != 0) { NeedSync = TRUE; }
		for (count2 = 0; count2 < 8; count2++) {
			if (FpuMappedTo(count2) == (DWORD)-1) {
				NeedSync = TRUE;
			}
		}
		for (count2 = 1; count2 < 10; count2++) {
			if (x86Mapped(count2) == Stack_Mapped) {
				if (x86Mapped(count2) != RegSet->x86reg_MappedTo[count2]) {
					NeedSync = TRUE;
				}
				break;
			}
		}
		for (count2 = 0; count2 < 32; count2++) {
			if (NeedSync == TRUE)  { break; }
			if (MipsRegState(count2) != RegSet->MIPS_RegState[count2]) {
				NeedSync = TRUE;
				continue;
			}
			switch (MipsRegState(count2)) {
			case STATE_UNKNOWN: break;
			case STATE_MAPPED_64:
				if (MipsReg(count2) != RegSet->MIPS_RegVal[count2].UDW) {
					NeedSync = TRUE;
				}
				break;
			case STATE_MAPPED_32_ZERO:
			case STATE_MAPPED_32_SIGN:
			case STATE_CONST_32:
				if (MipsRegLo(count2) != RegSet->MIPS_RegVal[count2].UW[0]) {
					NeedSync = TRUE;
				}
				break;
			}
		}
		if (NeedSync == FALSE) continue;
		Parent   = SectionParents[CurrentParent].Parent;
		JumpInfo = SectionParents[CurrentParent].JumpInfo;
		JmpLabel32(0);
		JumpInfo->LinkLocation  = RecompPos - 4;
		JumpInfo->LinkLocation2 = NULL;
		CurrentParent = count;
		Parent   = SectionParents[CurrentParent].Parent;
		JumpInfo = SectionParents[CurrentParent].JumpInfo;
		if (JumpInfo->LinkLocation != NULL) {
			SetJump32(JumpInfo->LinkLocation,RecompPos);
			JumpInfo->LinkLocation = NULL;
			if (JumpInfo->LinkLocation2 != NULL) {
				SetJump32(JumpInfo->LinkLocation2,RecompPos);
				JumpInfo->LinkLocation2 = NULL;
			}
		}
		memcpy(&Section->RegWorking,&JumpInfo->RegSet,sizeof(REG_INFO));
		if (BlockRandomModifier != 0) { SubConstFromVariable(BlockRandomModifier,&CP0[1]); }
		if (BlockCycleCount != 0) {
			AddConstToVariable(BlockCycleCount,&CP0[9]);
			SubConstFromVariable(BlockCycleCount,&Timers.Timer);
		}
		SyncRegState(Section,&Section->RegStart); 		//Sync
		memcpy(&Section->RegStart,&Section->RegWorking,sizeof(REG_INFO));
	}
	for (count = 0;count < NoOfCompiledParents;count++) {
		Parent   = SectionParents[count].Parent;
		JumpInfo = SectionParents[count].JumpInfo;
		if (JumpInfo->LinkLocation != NULL) {
			SetJump32(JumpInfo->LinkLocation,RecompPos);
			JumpInfo->LinkLocation = NULL;
			if (JumpInfo->LinkLocation2 != NULL) {
				SetJump32(JumpInfo->LinkLocation2,RecompPos);
				JumpInfo->LinkLocation2 = NULL;
			}
		}
	}
	BlockCycleCount = 0;
	BlockRandomModifier = 0;
	free(SectionParents);
	return TRUE;
}
BOOL IsAllParentLoops(BLOCK_SECTION * Section,BLOCK_SECTION * Parent,BOOL IgnoreIfCompiled,DWORD Test) {
	int count;
	if (IgnoreIfCompiled && Parent->CompiledLocation != NULL) return TRUE;
	if (!Section->InLoop || !Parent->InLoop || Parent->ParentSection == NULL) return FALSE;
	if (Section == Parent || Parent->Test == Test) return TRUE;
	Parent->Test = Test;
	for (count = 0;Parent->ParentSection[count] != NULL;count++) {
		if (!IsAllParentLoops(Section,Parent->ParentSection[count],IgnoreIfCompiled,Test)) return FALSE;
	}
	return TRUE;
}
void _fastcall InitializeSection (BLOCK_SECTION * Section,BLOCK_SECTION * Parent,DWORD StartAddr,DWORD ID) {
	Section->ParentSection      = NULL;
	Section->JumpSection        = NULL;
	Section->ContinueSection    = NULL;
	Section->CompiledLocation   = NULL;
	Section->SectionID          = ID;
	Section->Test               = 0;
	Section->Test2              = 0;
	Section->InLoop             = FALSE;
	Section->StartPC            = StartAddr;
	Section->CompilePC          = Section->StartPC;
	Section->Jump.LinkLocation  = NULL;
	Section->Jump.LinkLocation2 = NULL;
	Section->Jump.FallThrough   = FALSE;
	Section->Jump.PermLoop      = FALSE;
	Section->Jump.TargetPC      = (DWORD)-1;
	Section->Cont.LinkLocation  = NULL;
	Section->Cont.LinkLocation2 = NULL;
	Section->Cont.FallThrough   = FALSE;
	Section->Cont.PermLoop      = FALSE;
	Section->Cont.TargetPC      = (DWORD)-1;
	AddParent(Section,Parent);
}
void MarkCodeBlock (DWORD PAddr) {
	if (PAddr < RDRAMsize) {
		N64_Blocks.NoOfRDRAMBlocks[PAddr >> 12] += 1;
	} else if (PAddr >= 0x04000000 && PAddr <= 0x04000FFC) {
		N64_Blocks.NoOfDMEMBlocks += 1;
	} else if (PAddr >= 0x04001000 && PAddr <= 0x04001FFC) {
		N64_Blocks.NoOfIMEMBlocks += 1;
	} else if (PAddr >= 0x1FC00000 && PAddr <= 0x1FC00800) {
		N64_Blocks.NoOfPifRomBlocks += 1;
	}
}
void StartRecompilerCPU (void) {
	DWORD Addr;
	BYTE * Block;
	CoInitialize(NULL);
	if (SelfModCheck == ModCode_CheckMemory || SelfModCheck == ModCode_CheckSetMemory) {
		if (TargetInfo == NULL) {
			TargetInfo = VirtualAlloc(NULL,MaxCodeBlocks * sizeof(TARGET_INFO),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
			if (TargetInfo == NULL) {
				DisplayError(GS(MSG_MEM_ALLOC_ERROR));
				DisplayThreadExit("StartRecompilerCPU - TargetInfo == NULL");
			}
		}
		TargetIndex = 0;
	}
	if (OrigMem != NULL) {
		VirtualFree(OrigMem,0,MEM_RELEASE);
		OrigMem = NULL;
	}
	if (GfxRomOpen != NULL && (!inFullScreen || strcmp(GfxDLL,"Icepir8sLegacyLLE.dll") == 0)) { GfxRomOpen(); }
	if (ContRomOpen != NULL && !GLideN64NeedsToBeSetupFirst) { ContRomOpen(); }
	ResetRecompCode();
	memset(&N64_Blocks,0,sizeof(N64_Blocks));
	SetNormal
	__try {
		for (;;) {
			Addr = PROGRAM_COUNTER;
			if (UseTLB) {
				if (!TranslateVaddr(&Addr)) {
					DoTLBMiss(NextInstruction == DELAY_SLOT,PROGRAM_COUNTER);
					SetNormal
					Addr = PROGRAM_COUNTER;
					if (!TranslateVaddr(&Addr)) DisplayThreadExit("StartRecompilerCPU - !TranslateVaddr(&Addr) x2");
				}
			} else {
				Addr &= 0x1FFFFFFF;
			}
			if (NextInstruction == DELAY_SLOT) {
				__try {
					Block = *(DelaySlotTable + (Addr >> 12));
				} __except(EXCEPTION_EXECUTE_HANDLER) { DisplayThreadExit("StartRecompilerCPU - EXCEPTION_EXECUTE_HANDLER"); }
				if (Block == NULL) {
					DWORD OldProtect;
					Block = CompileDelaySlot();
					*(DelaySlotTable + (Addr >> 12)) = Block;
					if (SelfModCheck == ModCode_ProtectMemory) {
						VirtualProtect(N64MEM + Addr,4,PAGE_READONLY,&OldProtect);
					}
					SetNormal
				}
				_asm {
					pushad
					call Block
					popad
				}
				continue;
			}
			__try {
				if (Addr > 0x10000000)
				{
					if (PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000)) {
						while (PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000)) {
							ExecuteInterpreterOpCode();
						}
						continue;
					} else DisplayThreadExit("StartRecompilerCPU - Addr > 0x10000000 - PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000) - else\n\nSelf-modifying Code Method-related?");
				}
				Block = *(JumpTable + (Addr >> 2));
			} __except(EXCEPTION_EXECUTE_HANDLER) {
				if (PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000)) {
					while (PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000)) {
						ExecuteInterpreterOpCode();
					}
					continue;
				}
				else {
					if (SelfModCheck == ModCode_CheckSetMemory) DisplayThreadExit("StartRecompilerCPU - EXCEPTION_EXECUTE_HANDLER - PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000) - else\n\nNeeds 'Self-modifying Code Method=Protect Memory'?");
					else if (RDRAMsize == 0x400000) DisplayThreadExit("StartRecompilerCPU - EXCEPTION_EXECUTE_HANDLER - PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000) - else\n\nNeeds 'Memory Size=8MB'?");
					else DisplayThreadExit("StartRecompilerCPU - EXCEPTION_EXECUTE_HANDLER - PROGRAM_COUNTER >= 0xB0000000 && PROGRAM_COUNTER < (RomFileSize | 0xB0000000) - else");
				}
			}
			if ((SelfModCheck == ModCode_CheckMemory || SelfModCheck == ModCode_CheckSetMemory) && Block != NULL) {
				TARGET_INFO * Target = (TARGET_INFO *)Block;
				if (*(QWORD *)(N64MEM+Addr) != Target->OriginalMemory) {
					DWORD Start = (Addr & ~0xFFF) - 0x10000,End = Start + 0x20000,count;
					if (End < RDRAMsize) { End = RDRAMsize; }
					for (count = (Start >> 12); count < (End >> 12); count ++) {
						if (N64_Blocks.NoOfRDRAMBlocks[count] > 0) {
							N64_Blocks.NoOfRDRAMBlocks[count] = 0;
							memset(JumpTable + (count << 10),0,0x1000);
							*(DelaySlotTable + count) = NULL;
						}
					}
					Block = NULL;
				} else {
					Block = Target->CodeBlock;
				}
			}
			if (Block == NULL) {
				DWORD OldProtect;
				__try {
					Block = Compiler4300iBlock();
				} __except(EXCEPTION_EXECUTE_HANDLER) {
					ResetRecompCode();
					Block = Compiler4300iBlock();
				}
				if (SelfModCheck == ModCode_CheckMemory || SelfModCheck == ModCode_CheckSetMemory) {
					TargetInfo[TargetIndex].CodeBlock = Block;
					TargetInfo[TargetIndex].OriginalMemory = *(QWORD *)(N64MEM+Addr);
					*(JumpTable + (Addr >> 2)) = &TargetInfo[TargetIndex];
					TargetIndex += 1;
					if (TargetIndex == MaxCodeBlocks) {
						ResetRecompCode();
						continue;
					}
				} else {
					*(JumpTable + (Addr >> 2)) = Block;
				}
				if (SelfModCheck == ModCode_ProtectMemory) {
					VirtualProtect(N64MEM + Addr,4,PAGE_READONLY,&OldProtect);
				}
				SetNormal
			}
			_asm {
				pushad
				call Block
				popad
			}
		}
	} __except(r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())) { DisplayThreadExit("StartRecompilerCPU - r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())"); }
}
void SyncRegState (BLOCK_SECTION * Section,REG_INFO * SyncTo) {
	int count,x86Reg,x86RegHi,changed;
	changed = FALSE;
	UnMap_AllFPRs(Section);
	if (CurrentRoundingModel != SyncTo->RoundingModel) { CurrentRoundingModel = RoundUnknown; }
	x86Reg = Map_MemoryStack(Section,FALSE);
	for (x86Reg = 1; x86Reg < 10; x86Reg ++) {
		if (x86Mapped(x86Reg) != Stack_Mapped) continue;
		if (SyncTo->x86reg_MappedTo[x86Reg] != Stack_Mapped) {
			UnMap_X86reg(Section,x86Reg);
			for (count = 1; count < 10; count ++) {
				if (SyncTo->x86reg_MappedTo[count] == Stack_Mapped) {
					MoveX86RegToX86Reg(count,x86Reg);
					changed = TRUE;
				}
			}
			if (!changed) {
				MoveVariableToX86reg(&MemoryStack,x86Reg);
			}
			changed = TRUE;
		}
	}
	for (x86Reg = 1; x86Reg < 10; x86Reg ++) {
		if (SyncTo->x86reg_MappedTo[x86Reg] != Stack_Mapped) continue;
		if (x86Mapped(x86Reg) == Stack_Mapped) { break; }
		UnMap_X86reg(Section,x86Reg);
	}
	for (count = 1; count < 32; count ++) {
		if (MipsRegState(count) == SyncTo->MIPS_RegState[count]) {
			switch (MipsRegState(count)) {
			case STATE_UNKNOWN: continue;
			case STATE_MAPPED_64:
				if (MipsReg(count) == SyncTo->MIPS_RegVal[count].UDW) {
					continue;
				}
				break;
			case STATE_MAPPED_32_ZERO:
			case STATE_MAPPED_32_SIGN:
				if (MipsRegLo(count) == SyncTo->MIPS_RegVal[count].UW[0]) {
					continue;
				}
				break;
			case STATE_CONST_64:
				continue;
			case STATE_CONST_32:
				continue;
			}
		}
		changed = TRUE;
		switch (SyncTo->MIPS_RegState[count]) {
		case STATE_UNKNOWN: UnMap_GPR(Section,count,TRUE); break;
		case STATE_MAPPED_64:
			x86Reg = SyncTo->MIPS_RegVal[count].UW[0];
			x86RegHi = SyncTo->MIPS_RegVal[count].UW[1];
			UnMap_X86reg(Section,x86Reg);
			UnMap_X86reg(Section,x86RegHi);
			switch (MipsRegState(count)) {
			case STATE_UNKNOWN:
				MoveVariableToX86reg(&GPR[count].UW[0],x86Reg);
				MoveVariableToX86reg(&GPR[count].UW[1],x86RegHi);
				break;
			case STATE_MAPPED_64:
				MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
				x86Mapped(MipsRegLo(count)) = NotMapped;
				MoveX86RegToX86Reg(MipsRegHi(count),x86RegHi);
				x86Mapped(MipsRegHi(count)) = NotMapped;
				break;
			case STATE_MAPPED_32_SIGN:
				MoveX86RegToX86Reg(MipsRegLo(count),x86RegHi);
				ShiftRightSignImmed(x86RegHi,31);
				MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
				x86Mapped(MipsRegLo(count)) = NotMapped;
				break;
			case STATE_MAPPED_32_ZERO:
				XorX86RegToX86Reg(x86RegHi,x86RegHi);
				MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
				x86Mapped(MipsRegLo(count)) = NotMapped;
				break;
			case STATE_CONST_64:
				MoveConstToX86reg(MipsRegHi(count),x86RegHi);
				MoveConstToX86reg(MipsRegLo(count),x86Reg);
				break;
			case STATE_CONST_32:
				MoveConstToX86reg(MipsRegLo_S(count) >> 31,x86RegHi);
				MoveConstToX86reg(MipsRegLo(count),x86Reg);
				break;
			default:
				continue;
			}
			MipsRegLo(count) = x86Reg;
			MipsRegHi(count) = x86RegHi;
			MipsRegState(count) = STATE_MAPPED_64;
			x86Mapped(x86Reg) = GPR_Mapped;
			x86Mapped(x86RegHi) = GPR_Mapped;
			x86MapOrder(x86Reg) = 1;
			x86MapOrder(x86RegHi) = 1;
			break;
		case STATE_MAPPED_32_SIGN:
			x86Reg = SyncTo->MIPS_RegVal[count].UW[0];
			UnMap_X86reg(Section,x86Reg);
			switch (MipsRegState(count)) {
			case STATE_UNKNOWN: MoveVariableToX86reg(&GPR[count].UW[0],x86Reg); break;
			case STATE_CONST_32: MoveConstToX86reg(MipsRegLo(count),x86Reg); break;
			case STATE_MAPPED_32_SIGN:
				MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
				x86Mapped(MipsRegLo(count)) = NotMapped;
				break;
			case STATE_MAPPED_32_ZERO:
				if (MipsRegLo(count) != (DWORD)x86Reg) {
					MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
					x86Mapped(MipsRegLo(count)) = NotMapped;
				}
				break;
			case STATE_MAPPED_64:
				MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
				x86Mapped(MipsRegLo(count)) = NotMapped;
				x86Mapped(MipsRegHi(count)) = NotMapped;
				break;
			}
			MipsRegLo(count) = x86Reg;
			MipsRegState(count) = STATE_MAPPED_32_SIGN;
			x86Mapped(x86Reg) = GPR_Mapped;
			x86MapOrder(x86Reg) = 1;
			break;
		case STATE_MAPPED_32_ZERO:
			x86Reg = SyncTo->MIPS_RegVal[count].UW[0];
			UnMap_X86reg(Section,x86Reg);
			switch (MipsRegState(count)) {
			case STATE_MAPPED_64:
			case STATE_UNKNOWN:
				MoveVariableToX86reg(&GPR[count].UW[0],x86Reg);
				break;
			case STATE_MAPPED_32_ZERO:
				MoveX86RegToX86Reg(MipsRegLo(count),x86Reg);
				x86Mapped(MipsRegLo(count)) = NotMapped;
				break;
			case STATE_CONST_32:
				MoveConstToX86reg(MipsRegLo(count),x86Reg);
				break;
			}
			MipsRegLo(count) = x86Reg;
			MipsRegState(count) = SyncTo->MIPS_RegState[count];
			x86Mapped(x86Reg) = GPR_Mapped;
			x86MapOrder(x86Reg) = 1;
			break;
		default:
			changed = FALSE;
		}
	}
}