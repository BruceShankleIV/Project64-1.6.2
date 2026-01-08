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
#include <math.h>
#include <float.h>
#include "Main.h"
#include "CPU.h"
#include "Plugin.h"
void * R4300i_Opcode[64];
void * R4300i_Special[64];
void * R4300i_Regimm[32];
void * R4300i_CoP0[32];
void * R4300i_CoP0_Function[64];
void * R4300i_CoP1[32];
void * R4300i_CoP1_BC[32];
void * R4300i_CoP1_S[64];
void * R4300i_CoP1_D[64];
void * R4300i_CoP1_W[64];
void * R4300i_CoP1_L[64];
void * R4300i_CoP2[32];
void _fastcall r4300i_SPECIAL (void) {
	((void (_fastcall *)()) R4300i_Special[ Opcode.funct ])();
}
void _fastcall r4300i_REGIMM (void) {
	((void (_fastcall *)()) R4300i_Regimm[ Opcode.rt ])();
}
void _fastcall r4300i_COP0 (void) {
	((void (_fastcall *)()) R4300i_CoP0[ Opcode.rs ])();
}
void _fastcall r4300i_COP0_CO (void) {
	((void (_fastcall *)()) R4300i_CoP0_Function[ Opcode.funct ])();
}
void _fastcall r4300i_COP1 (void) {
	((void (_fastcall *)()) R4300i_CoP1[ Opcode.fmt ])();
}
void _fastcall r4300i_COP1_BC (void) {
	((void (_fastcall *)()) R4300i_CoP1_BC[ Opcode.ft ])();
}
void _fastcall r4300i_COP1_S (void) {
	_controlfp(RoundingModel,_MCW_RC);
	((void (_fastcall *)()) R4300i_CoP1_S[ Opcode.funct ])();
}
void _fastcall r4300i_COP1_D (void) {
	_controlfp(RoundingModel,_MCW_RC);
	((void (_fastcall *)()) R4300i_CoP1_D[ Opcode.funct ])();
}
void _fastcall r4300i_COP1_W (void) {
	((void (_fastcall *)()) R4300i_CoP1_W[ Opcode.funct ])();
}
void _fastcall r4300i_COP1_L (void) {
	((void (_fastcall *)()) R4300i_CoP1_L[ Opcode.funct ])();
}
void _fastcall r4300i_COP2 (void) {
	((void (_fastcall *)()) R4300i_CoP2[ Opcode.rs ])();
}
void _fastcall DTE_RESERVED() {
	DisplayThreadExit("BuildInterpreter-i=0; i<64; i++\n\nThe emulator has crashed on a reserved Opcode at this location");
}
void _fastcall DTE_DUMMY() {
	DisplayThreadExit("BuildInterpreter-i=0; i<64; i++\n\nThe emulator has crashed on an unknown Opcode at this location");
}
void _fastcall DUMMY() {}
void BuildInterpreter (void) {
	int i;
	for (i=0; i<64; i++) {
		R4300i_Opcode[i]	=DTE_RESERVED;
		R4300i_Special[i]	=DTE_RESERVED;
		R4300i_Regimm[i]	=DTE_RESERVED;
		R4300i_CoP0[i]		=DTE_DUMMY;
		R4300i_CoP0_Function[i]	=DTE_DUMMY;
		R4300i_CoP1[i]		=DTE_DUMMY;
		R4300i_CoP1_BC[i]	=DTE_DUMMY;
		R4300i_CoP1_S[i]	=DTE_DUMMY;
		R4300i_CoP1_D[i]	=DTE_DUMMY;
		R4300i_CoP1_W[i]	=DTE_DUMMY;
		R4300i_CoP1_L[i]	=DTE_DUMMY;
		R4300i_CoP2[i]		=DTE_DUMMY;
	}
	R4300i_Opcode[ 0]=r4300i_SPECIAL;
	R4300i_Opcode[ 1]=r4300i_REGIMM;
	R4300i_Opcode[ 2]=r4300i_J;
	R4300i_Opcode[ 3]=r4300i_JAL;
	R4300i_Opcode[ 4]=r4300i_BEQ;
	R4300i_Opcode[ 5]=r4300i_BNE;
	R4300i_Opcode[ 6]=r4300i_BLEZ;
	R4300i_Opcode[ 7]=r4300i_BGTZ;
	R4300i_Opcode[ 8]=r4300i_ADDI;
	R4300i_Opcode[ 9]=r4300i_ADDIU;
	R4300i_Opcode[10]=r4300i_SLTI;
	R4300i_Opcode[11]=r4300i_SLTIU;
	R4300i_Opcode[12]=r4300i_ANDI;
	R4300i_Opcode[13]=r4300i_ORI;
	R4300i_Opcode[14]=r4300i_XORI;
	R4300i_Opcode[15]=r4300i_LUI;
	R4300i_Opcode[16]=r4300i_COP0;
	R4300i_Opcode[17]=r4300i_COP1;
	R4300i_Opcode[18]=DUMMY; // COP2
	R4300i_Opcode[19]=DUMMY; // COP3
	R4300i_Opcode[20]=r4300i_BEQL;
	R4300i_Opcode[21]=r4300i_BNEL;
	R4300i_Opcode[22]=r4300i_BLEZL;
	R4300i_Opcode[23]=r4300i_BGTZL;
	R4300i_Opcode[24]=DUMMY; // DADDI
	R4300i_Opcode[25]=r4300i_DADDIU;
	R4300i_Opcode[26]=r4300i_LDL;
	R4300i_Opcode[27]=r4300i_LDR;
	R4300i_Opcode[32]=r4300i_LB;
	R4300i_Opcode[33]=r4300i_LH;
	R4300i_Opcode[34]=r4300i_LWL;
	R4300i_Opcode[35]=r4300i_LW;
	R4300i_Opcode[36]=r4300i_LBU;
	R4300i_Opcode[37]=r4300i_LHU;
	R4300i_Opcode[38]=r4300i_LWR;
	R4300i_Opcode[39]=r4300i_LWU;
	R4300i_Opcode[40]=r4300i_SB;
	R4300i_Opcode[41]=r4300i_SH;
	R4300i_Opcode[42]=r4300i_SWL;
	R4300i_Opcode[43]=r4300i_SW;
	R4300i_Opcode[44]=r4300i_SDL;
	R4300i_Opcode[45]=r4300i_SDR;
	R4300i_Opcode[46]=r4300i_SWR;
	R4300i_Opcode[47]=DUMMY; // CACHE
	R4300i_Opcode[48]=r4300i_LL;
	R4300i_Opcode[49]=r4300i_LWC1;
	R4300i_Opcode[50]=DUMMY; // LWC2
	R4300i_Opcode[52]=DUMMY; // LLD
	R4300i_Opcode[53]=r4300i_LDC1;
	R4300i_Opcode[54]=DUMMY; // LDC2
	R4300i_Opcode[55]=r4300i_LD;
	R4300i_Opcode[56]=r4300i_SC;
	R4300i_Opcode[57]=r4300i_SWC1;
	R4300i_Opcode[58]=DUMMY; // SWC2
	R4300i_Opcode[60]=DUMMY; // SCD
	R4300i_Opcode[61]=r4300i_SDC1;
	R4300i_Opcode[62]=DUMMY; // SDC2
	R4300i_Opcode[63]=r4300i_SD;
	R4300i_Special[ 0]=r4300i_SPECIAL_SLL;
	R4300i_Special[ 2]=r4300i_SPECIAL_SRL;
	R4300i_Special[ 3]=r4300i_SPECIAL_SRA;
	R4300i_Special[ 4]=r4300i_SPECIAL_SLLV;
	R4300i_Special[ 6]=r4300i_SPECIAL_SRLV;
	R4300i_Special[ 7]=r4300i_SPECIAL_SRAV;
	R4300i_Special[ 8]=r4300i_SPECIAL_JR;
	R4300i_Special[ 9]=r4300i_SPECIAL_JALR;
	R4300i_Special[12]=r4300i_SPECIAL_SYSCALL;
	R4300i_Special[13]=r4300i_SPECIAL_BREAK;
	R4300i_Special[15]=DUMMY; // SPECIAL_SYNC
	R4300i_Special[16]=r4300i_SPECIAL_MFHI;
	R4300i_Special[17]=r4300i_SPECIAL_MTHI;
	R4300i_Special[18]=r4300i_SPECIAL_MFLO;
	R4300i_Special[19]=r4300i_SPECIAL_MTLO;
	R4300i_Special[20]=r4300i_SPECIAL_DSLLV;
	R4300i_Special[22]=r4300i_SPECIAL_DSRLV;
	R4300i_Special[23]=r4300i_SPECIAL_DSRAV;
	R4300i_Special[24]=r4300i_SPECIAL_MULT;
	R4300i_Special[25]=r4300i_SPECIAL_MULTU;
	R4300i_Special[26]=r4300i_SPECIAL_DIV;
	R4300i_Special[27]=r4300i_SPECIAL_DIVU;
	R4300i_Special[28]=r4300i_SPECIAL_DMULT;
	R4300i_Special[29]=r4300i_SPECIAL_DMULTU;
	R4300i_Special[30]=r4300i_SPECIAL_DDIV;
	R4300i_Special[31]=r4300i_SPECIAL_DDIVU;
	R4300i_Special[32]=r4300i_SPECIAL_ADD;
	R4300i_Special[33]=r4300i_SPECIAL_ADDU;
	R4300i_Special[34]=r4300i_SPECIAL_SUB;
	R4300i_Special[35]=r4300i_SPECIAL_SUBU;
	R4300i_Special[36]=r4300i_SPECIAL_AND;
	R4300i_Special[37]=r4300i_SPECIAL_OR;
	R4300i_Special[38]=r4300i_SPECIAL_XOR;
	R4300i_Special[39]=r4300i_SPECIAL_NOR;
	R4300i_Special[42]=r4300i_SPECIAL_SLT;
	R4300i_Special[43]=r4300i_SPECIAL_SLTU;
	R4300i_Special[44]=r4300i_SPECIAL_DADD;
	R4300i_Special[45]=r4300i_SPECIAL_DADDU;
	R4300i_Special[46]=r4300i_SPECIAL_DSUB;
	R4300i_Special[47]=r4300i_SPECIAL_DSUBU;
	R4300i_Special[48]=DUMMY; // SPECIAL_TGE
	R4300i_Special[49]=DUMMY; // SPECIAL_TGEU
	R4300i_Special[50]=DUMMY; // SPECIAL_TLT
	R4300i_Special[51]=DUMMY; // SPECIAL_TLTU
	R4300i_Special[52]=DUMMY; // SPECIAL_TEQ
	R4300i_Special[54]=DUMMY; // SPECIAL_TNE
	R4300i_Special[56]=r4300i_SPECIAL_DSLL;
	R4300i_Special[58]=r4300i_SPECIAL_DSRL;
	R4300i_Special[59]=r4300i_SPECIAL_DSRA;
	R4300i_Special[60]=r4300i_SPECIAL_DSLL32;
	R4300i_Special[62]=r4300i_SPECIAL_DSRL32;
	R4300i_Special[63]=r4300i_SPECIAL_DSRA32;
	R4300i_Regimm[ 0]=r4300i_REGIMM_BLTZ;
	R4300i_Regimm[ 1]=r4300i_REGIMM_BGEZ;
	R4300i_Regimm[ 2]=r4300i_REGIMM_BLTZL;
	R4300i_Regimm[ 3]=r4300i_REGIMM_BGEZL;
	R4300i_Regimm[ 8]=DUMMY; // REGIMM_TGEI
	R4300i_Regimm[ 9]=DUMMY; // REGIMM_TGEIU
	R4300i_Regimm[10]=DUMMY; // REGIMM_TLTI
	R4300i_Regimm[11]=DUMMY; // REGIMM_TLTIU
	R4300i_Regimm[12]=DUMMY; // REGIMM_TEQI
	R4300i_Regimm[14]=DUMMY; // REGIMM_TNEI
	R4300i_Regimm[16]=r4300i_REGIMM_BLTZAL;
	R4300i_Regimm[17]=r4300i_REGIMM_BGEZAL;
	R4300i_Regimm[18]=DUMMY; // REGIMM_BLTZALL
	R4300i_Regimm[19]=DUMMY; // REGIMM_BGEZALL
	R4300i_CoP0[ 0]=r4300i_COP0_MF;
	R4300i_CoP0[ 1]=DUMMY; // COP0_DMF
	R4300i_CoP0[ 4]=r4300i_COP0_MT;
	R4300i_CoP0[ 5]=DUMMY; // COP0_DMT
	R4300i_CoP0[16]=r4300i_COP0_CO;
	R4300i_CoP0[17]=r4300i_COP0_CO;
	R4300i_CoP0[18]=r4300i_COP0_CO;
	R4300i_CoP0[19]=r4300i_COP0_CO;
	R4300i_CoP0[20]=r4300i_COP0_CO;
	R4300i_CoP0[21]=r4300i_COP0_CO;
	R4300i_CoP0[22]=r4300i_COP0_CO;
	R4300i_CoP0[23]=r4300i_COP0_CO;
	R4300i_CoP0[24]=r4300i_COP0_CO;
	R4300i_CoP0[25]=r4300i_COP0_CO;
	R4300i_CoP0[26]=r4300i_COP0_CO;
	R4300i_CoP0[27]=r4300i_COP0_CO;
	R4300i_CoP0[28]=r4300i_COP0_CO;
	R4300i_CoP0[29]=r4300i_COP0_CO;
	R4300i_CoP0[30]=r4300i_COP0_CO;
	R4300i_CoP0[31]=r4300i_COP0_CO;
	R4300i_CoP0_Function[ 1]=r4300i_COP0_CO_TLBR;
	R4300i_CoP0_Function[ 2]=r4300i_COP0_CO_TLBWI;
	R4300i_CoP0_Function[ 6]=r4300i_COP0_CO_TLBWR;
	R4300i_CoP0_Function[ 8]=r4300i_COP0_CO_TLBP;
	R4300i_CoP0_Function[24]=r4300i_COP0_CO_ERET;
	R4300i_CoP1[ 0]=r4300i_COP1_MF;
	R4300i_CoP1[ 1]=r4300i_COP1_DMF;
	R4300i_CoP1[ 2]=r4300i_COP1_CF;
	R4300i_CoP1[ 3]=DUMMY; // COP1_DCF
	R4300i_CoP1[ 4]=r4300i_COP1_MT;
	R4300i_CoP1[ 5]=r4300i_COP1_DMT;
	R4300i_CoP1[ 6]=r4300i_COP1_CT;
	R4300i_CoP1[ 8]=r4300i_COP1_BC;
	R4300i_CoP1[16]=r4300i_COP1_S;
	R4300i_CoP1[17]=r4300i_COP1_D;
	R4300i_CoP1[20]=r4300i_COP1_W;
	R4300i_CoP1[21]=r4300i_COP1_L;
	R4300i_CoP1_BC[ 0]=r4300i_COP1_BC_BCF;
	R4300i_CoP1_BC[ 1]=r4300i_COP1_BC_BCT;
	R4300i_CoP1_BC[ 2]=r4300i_COP1_BC_BCFL;
	R4300i_CoP1_BC[ 3]=r4300i_COP1_BC_BCTL;
	R4300i_CoP1_S[ 0]=r4300i_COP1_S_ADD;
	R4300i_CoP1_S[ 1]=r4300i_COP1_S_SUB;
	R4300i_CoP1_S[ 2]=r4300i_COP1_S_MUL;
	R4300i_CoP1_S[ 3]=r4300i_COP1_S_DIV;
	R4300i_CoP1_S[ 4]=r4300i_COP1_S_SQRT;
	R4300i_CoP1_S[ 5]=r4300i_COP1_S_ABS;
	R4300i_CoP1_S[ 6]=r4300i_COP1_S_MOV;
	R4300i_CoP1_S[ 7]=r4300i_COP1_S_NEG;
	R4300i_CoP1_S[ 8]=DUMMY; // COP1_S_ROUND_L
	R4300i_CoP1_S[ 9]=r4300i_COP1_S_TRUNC_L;
	R4300i_CoP1_S[10]=r4300i_COP1_S_CEIL_L;		//added by Witten
	R4300i_CoP1_S[11]=r4300i_COP1_S_FLOOR_L;		//added by Witten
	R4300i_CoP1_S[12]=r4300i_COP1_S_ROUND_W;
	R4300i_CoP1_S[13]=r4300i_COP1_S_TRUNC_W;
	R4300i_CoP1_S[14]=r4300i_COP1_S_CEIL_W;		//added by Witten
	R4300i_CoP1_S[15]=r4300i_COP1_S_FLOOR_W;
	R4300i_CoP1_S[32]=DUMMY; // COP1_S_CVT_S
	R4300i_CoP1_S[33]=r4300i_COP1_S_CVT_D;
	R4300i_CoP1_S[36]=r4300i_COP1_S_CVT_W;
	R4300i_CoP1_S[37]=r4300i_COP1_S_CVT_L;
	R4300i_CoP1_S[48]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[49]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[50]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[51]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[52]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[53]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[54]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[55]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[56]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[57]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[58]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[59]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[60]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[61]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[62]=r4300i_COP1_S_CMP;
	R4300i_CoP1_S[63]=r4300i_COP1_S_CMP;
	R4300i_CoP1_D[ 0]=r4300i_COP1_D_ADD;
	R4300i_CoP1_D[ 1]=r4300i_COP1_D_SUB;
	R4300i_CoP1_D[ 2]=r4300i_COP1_D_MUL;
	R4300i_CoP1_D[ 3]=r4300i_COP1_D_DIV;
	R4300i_CoP1_D[ 4]=r4300i_COP1_D_SQRT;
	R4300i_CoP1_D[ 5]=r4300i_COP1_D_ABS;
	R4300i_CoP1_D[ 6]=r4300i_COP1_D_MOV;
	R4300i_CoP1_D[ 7]=r4300i_COP1_D_NEG;
	R4300i_CoP1_D[ 8]=DUMMY; // COP1_D_ROUND_L
	R4300i_CoP1_D[ 9]=r4300i_COP1_D_TRUNC_L;		//added by Witten
	R4300i_CoP1_D[10]=r4300i_COP1_D_CEIL_L;		//added by Witten
	R4300i_CoP1_D[11]=r4300i_COP1_D_FLOOR_L;		//added by Witten
	R4300i_CoP1_D[12]=r4300i_COP1_D_ROUND_W;
	R4300i_CoP1_D[13]=r4300i_COP1_D_TRUNC_W;
	R4300i_CoP1_D[14]=r4300i_COP1_D_CEIL_W;		//added by Witten
	R4300i_CoP1_D[15]=r4300i_COP1_D_FLOOR_W;		//added by Witten
	R4300i_CoP1_D[32]=r4300i_COP1_D_CVT_S;
	R4300i_CoP1_D[33]=DUMMY; // COP1_D_CVT_D
	R4300i_CoP1_D[36]=r4300i_COP1_D_CVT_W;
	R4300i_CoP1_D[37]=r4300i_COP1_D_CVT_L;
	R4300i_CoP1_D[48]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[49]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[50]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[51]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[52]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[53]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[54]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[55]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[56]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[57]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[58]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[59]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[60]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[61]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[62]=r4300i_COP1_D_CMP;
	R4300i_CoP1_D[63]=r4300i_COP1_D_CMP;
	R4300i_CoP1_W[ 8]=DUMMY; // COP1_W_ROUND_L
	R4300i_CoP1_W[ 9]=DUMMY; // COP1_W_TRUNC_L
	R4300i_CoP1_W[10]=DUMMY; // COP1_W_CEIL_L
	R4300i_CoP1_W[11]=DUMMY; // COP1_W_FLOOR_L
	R4300i_CoP1_W[12]=DUMMY; // COP1_W_ROUND_W
	R4300i_CoP1_W[13]=DUMMY; // COP1_W_TRUNC_W
	R4300i_CoP1_W[14]=DUMMY; // COP1_W_CEIL_W
	R4300i_CoP1_W[15]=DUMMY; // COP1_W_FLOOR_W
	R4300i_CoP1_W[32]=r4300i_COP1_W_CVT_S;
	R4300i_CoP1_W[33]=r4300i_COP1_W_CVT_D;
	R4300i_CoP1_W[36]=DUMMY; // COP1_W_CVT_W
	R4300i_CoP1_W[37]=DUMMY; // COP1_W_CVT_L
	R4300i_CoP1_L[ 8]=DUMMY; // COP1_L_ROUND_L
	R4300i_CoP1_L[ 9]=DUMMY; // COP1_L_TRUNC_L
	R4300i_CoP1_L[10]=DUMMY; // COP1_L_CEIL_L
	R4300i_CoP1_L[11]=DUMMY; // COP1_L_FLOOR_L
	R4300i_CoP1_L[12]=DUMMY; // COP1_L_ROUND_W
	R4300i_CoP1_L[13]=DUMMY; // COP1_L_TRUNC_W
	R4300i_CoP1_L[14]=DUMMY; // COP1_L_CEIL_W
	R4300i_CoP1_L[15]=DUMMY; // COP1_L_FLOOR_W
	R4300i_CoP1_L[32]=r4300i_COP1_L_CVT_S;
	R4300i_CoP1_L[33]=r4300i_COP1_L_CVT_D;
	R4300i_CoP1_L[36]=DUMMY; // COP1_L_CVT_W
	R4300i_CoP1_L[37]=DUMMY; // COP1_L_CVT_L
	R4300i_CoP2[ 0]=DUMMY; // COP2_MF
	R4300i_CoP2[ 1]=DUMMY; // COP2_DMF
	R4300i_CoP2[ 2]=DUMMY; // COP2_CF
	R4300i_CoP2[ 3]=DUMMY; // COP2_DCF
	R4300i_CoP2[ 4]=DUMMY; // COP2_MT
	R4300i_CoP2[ 5]=DUMMY; // COP2_DMT
	R4300i_CoP2[ 6]=DUMMY; // COP2_CT
	R4300i_CoP2[ 7]=DUMMY; // COP2_DCT
}
void ExecuteInterpreterOpCode (void) {
	if (!r4300i_LW_VAddr(PROGRAM_COUNTER,&Opcode.Hex)) {
		DoTLBMiss(NextInstruction==JUMP,PROGRAM_COUNTER);
		SetNormal
		return;
	}
	COUNT_REGISTER +=CountPerOp;
	Timers.Timer -=CountPerOp;
	RANDOM_REGISTER -=1;
	if ((int)RANDOM_REGISTER<(int)WIRED_REGISTER) {
		RANDOM_REGISTER=31;
	}
	((void (_fastcall *)()) R4300i_Opcode[ Opcode.op ])();
	if (GPR[0].DW!=0) {
		GPR[0].DW=0;
	}
	switch (NextInstruction) {
	case NORMAL:
		PROGRAM_COUNTER +=4;
		break;
	case DELAY_SLOT:
		SetJump
		PROGRAM_COUNTER +=4;
		break;
	case JUMP:
		PROGRAM_COUNTER=JumpToLocation;
		SetNormal
		if ((int)Timers.Timer<0) { TimerDone(); }
		if (CPU_Action.DoSomething) { DoSomething(); }
		}
	}
void StartInterpreterCPU (void) {
	CoInitialize(NULL);
	SetNormal
	if (GfxRomOpen!=NULL&&(!inFullScreen||strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0)) { GfxRomOpen(); }
	if (ContRomOpen!=NULL&&!GLideN64NeedsToBeSetupFirst) { ContRomOpen(); }
	__try {
		for (;;) {
			ExecuteInterpreterOpCode();
		}
	} __except(r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())) {
		DisplayThreadExit("StartInterpreterCPU-r4300i_CPU_MemoryFilter(GetExceptionCode(),GetExceptionInformation())");
	}
}
void TestInterpreterJump (DWORD PC,DWORD TargetPC,int Reg1,int Reg2) {
	if (PC!=TargetPC) return;
	if (DelaySlotEffectsCompare(PC,Reg1,Reg2)) return;
	if (CPU_Type!=CPU_Interpreter) return;
}