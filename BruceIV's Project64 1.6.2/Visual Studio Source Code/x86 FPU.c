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
#define PUTDST8(dest,value)  (*((BYTE *)(dest))=(BYTE)(value)); dest +=1;
#define PUTDST16(dest,value) (*((WORD *)(dest))=(WORD)(value)); dest +=2;
#define PUTDST32(dest,value) (*((DWORD *)(dest))=(DWORD)(value)); dest +=4;
void fpuAbs(void) {
	PUTDST16(RecompPos,0xE1D9);
}
void fpuAddDword(void *Variable) {
	PUTDST16(RecompPos,0x05D8);
	PUTDST32(RecompPos,Variable);
}
void fpuAddDwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x00D8); break;
	case x86_EBX: PUTDST16(RecompPos,0x03D8); break;
	case x86_ECX: PUTDST16(RecompPos,0x01D8); break;
	case x86_EDX: PUTDST16(RecompPos,0x02D8); break;
	case x86_ESI: PUTDST16(RecompPos,0x06D8); break;
	case x86_EDI: PUTDST16(RecompPos,0x07D8);
	}
}
void fpuAddQword(void *Variable) {
	PUTDST16(RecompPos,0x05DC);
	PUTDST32(RecompPos,Variable);
}
void fpuAddQwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x00DC); break;
	case x86_EBX: PUTDST16(RecompPos,0x03DC); break;
	case x86_ECX: PUTDST16(RecompPos,0x01DC); break;
	case x86_EDX: PUTDST16(RecompPos,0x02DC); break;
	case x86_ESI: PUTDST16(RecompPos,0x06DC); break;
	case x86_EDI: PUTDST16(RecompPos,0x07DC);
	}
}
void fpuAddReg(int x86reg) {
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC0D8); break;
	case x86_ST1: PUTDST16(RecompPos,0xC1D8); break;
	case x86_ST2: PUTDST16(RecompPos,0xC2D8); break;
	case x86_ST3: PUTDST16(RecompPos,0xC3D8); break;
	case x86_ST4: PUTDST16(RecompPos,0xC4D8); break;
	case x86_ST5: PUTDST16(RecompPos,0xC5D8); break;
	case x86_ST6: PUTDST16(RecompPos,0xC6D8); break;
	case x86_ST7: PUTDST16(RecompPos,0xC7D8);
	}
}
void fpuAddRegPop(int * StackPos,int x86reg) {
	*StackPos=(*StackPos+1) & 7;
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC0DE); break;
	case x86_ST1: PUTDST16(RecompPos,0xC1DE); break;
	case x86_ST2: PUTDST16(RecompPos,0xC2DE); break;
	case x86_ST3: PUTDST16(RecompPos,0xC3DE); break;
	case x86_ST4: PUTDST16(RecompPos,0xC4DE); break;
	case x86_ST5: PUTDST16(RecompPos,0xC5DE); break;
	case x86_ST6: PUTDST16(RecompPos,0xC6DE); break;
	case x86_ST7: PUTDST16(RecompPos,0xC7DE);
	}
}
void fpuComDword(void *Variable,BOOL Pop) {
	PUTDST16(RecompPos,(Pop==TRUE)?0x1DD8:0x15D8);
	PUTDST32(RecompPos,Variable);
}
void fpuComDwordRegPointer(int x86Pointer,BOOL Pop) {
	WORD x86Command;
	switch (x86Pointer) {
	case x86_EAX: x86Command=0x10D8; break;
	case x86_EBX: x86Command=0x13D8; break;
	case x86_ECX: x86Command=0x11D8; break;
	case x86_EDX: x86Command=0x12D8; break;
	case x86_ESI: x86Command=0x16D8; break;
	case x86_EDI: x86Command=0x17D8;
	}
	if (Pop) { x86Command|=0x0800; }
	PUTDST16(RecompPos,x86Command);
}
void fpuComQword(void *Variable,BOOL Pop) {
	PUTDST16(RecompPos,(Pop==TRUE)?0x1DDC:0x15DC);
	PUTDST32(RecompPos,Variable);
}
void fpuComQwordRegPointer(int x86Pointer,BOOL Pop) {
	WORD x86Command;
	switch (x86Pointer) {
	case x86_EAX: x86Command=0x10DC; break;
	case x86_EBX: x86Command=0x13DC; break;
	case x86_ECX: x86Command=0x11DC; break;
	case x86_EDX: x86Command=0x12DC; break;
	case x86_ESI: x86Command=0x16DC; break;
	case x86_EDI: x86Command=0x17DC;
	}
	if (Pop) { x86Command|=0x0800; }
	PUTDST16(RecompPos,x86Command);
}
void fpuComReg(int x86reg,BOOL Pop) {
	int s=(Pop==TRUE)?0x0800:0x0000;
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xD0D8|s); break;
	case x86_ST1: PUTDST16(RecompPos,0xD1D8|s); break;
	case x86_ST2: PUTDST16(RecompPos,0xD2D8|s); break;
	case x86_ST3: PUTDST16(RecompPos,0xD3D8|s); break;
	case x86_ST4: PUTDST16(RecompPos,0xD4D8|s); break;
	case x86_ST5: PUTDST16(RecompPos,0xD5D8|s); break;
	case x86_ST6: PUTDST16(RecompPos,0xD6D8|s); break;
	case x86_ST7: PUTDST16(RecompPos,0xD7D8|s);
	}
}
void fpuDivDword(void *Variable) {
	PUTDST16(RecompPos,0x35D8);
	PUTDST32(RecompPos,Variable);
}
void fpuDivDwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x30D8); break;
	case x86_EBX: PUTDST16(RecompPos,0x33D8); break;
	case x86_ECX: PUTDST16(RecompPos,0x31D8); break;
	case x86_EDX: PUTDST16(RecompPos,0x32D8); break;
	case x86_ESI: PUTDST16(RecompPos,0x36D8); break;
	case x86_EDI: PUTDST16(RecompPos,0x37D8);
	}
}
void fpuDivQword(void *Variable) {
	PUTDST16(RecompPos,0x35DC);
	PUTDST32(RecompPos,Variable);
}
void fpuDivQwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x30DC); break;
	case x86_EBX: PUTDST16(RecompPos,0x33DC); break;
	case x86_ECX: PUTDST16(RecompPos,0x31DC); break;
	case x86_EDX: PUTDST16(RecompPos,0x32DC); break;
	case x86_ESI: PUTDST16(RecompPos,0x36DC); break;
	case x86_EDI: PUTDST16(RecompPos,0x37DC);
	}
}
void fpuDivReg(int Reg) {
	switch (Reg) {
	case x86_ST0: PUTDST16(RecompPos,0xF0D8); break;
	case x86_ST1: PUTDST16(RecompPos,0xF1D8); break;
	case x86_ST2: PUTDST16(RecompPos,0xF2D8); break;
	case x86_ST3: PUTDST16(RecompPos,0xF3D8); break;
	case x86_ST4: PUTDST16(RecompPos,0xF4D8); break;
	case x86_ST5: PUTDST16(RecompPos,0xF5D8); break;
	case x86_ST6: PUTDST16(RecompPos,0xF6D8); break;
	case x86_ST7: PUTDST16(RecompPos,0xF7D8);
	}
}
void fpuDivRegPop(int x86reg) {
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xF8DE); break;
	case x86_ST1: PUTDST16(RecompPos,0xF9DE); break;
	case x86_ST2: PUTDST16(RecompPos,0xFADE); break;
	case x86_ST3: PUTDST16(RecompPos,0xFBDE); break;
	case x86_ST4: PUTDST16(RecompPos,0xFCDE); break;
	case x86_ST5: PUTDST16(RecompPos,0xFDDE); break;
	case x86_ST6: PUTDST16(RecompPos,0xFEDE); break;
	case x86_ST7: PUTDST16(RecompPos,0xFFDE);
	}
}
void fpuExchange(int Reg) {
	switch (Reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC8D9); break;
	case x86_ST1: PUTDST16(RecompPos,0xC9D9); break;
	case x86_ST2: PUTDST16(RecompPos,0xCAD9); break;
	case x86_ST3: PUTDST16(RecompPos,0xCBD9); break;
	case x86_ST4: PUTDST16(RecompPos,0xCCD9); break;
	case x86_ST5: PUTDST16(RecompPos,0xCDD9); break;
	case x86_ST6: PUTDST16(RecompPos,0xCED9); break;
	case x86_ST7: PUTDST16(RecompPos,0xCFD9);
	}
}
void fpuFree(int Reg) {
	switch (Reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC0DD); break;
	case x86_ST1: PUTDST16(RecompPos,0xC1DD); break;
	case x86_ST2: PUTDST16(RecompPos,0xC2DD); break;
	case x86_ST3: PUTDST16(RecompPos,0xC3DD); break;
	case x86_ST4: PUTDST16(RecompPos,0xC4DD); break;
	case x86_ST5: PUTDST16(RecompPos,0xC5DD); break;
	case x86_ST6: PUTDST16(RecompPos,0xC6DD); break;
	case x86_ST7: PUTDST16(RecompPos,0xC7DD);
	}
}
void fpuDecStack(int * StackPos) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST16(RecompPos,0xF6D9);
}
void fpuIncStack(int * StackPos) {
	*StackPos=(*StackPos+1) & 7;
	PUTDST16(RecompPos,0xF7D9);
}
void fpuLoadControl(void *Variable) {
	PUTDST16(RecompPos,0x2DD9);
	PUTDST32(RecompPos,Variable);
}
void fpuLoadDword(int * StackPos,void *Variable) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST16(RecompPos,0x05D9);
	PUTDST32(RecompPos,Variable);
}
void fpuLoadDwordFromX86Reg(int * StackPos,int x86Reg) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST8(RecompPos,0xD9);
	switch (x86Reg) {
	case x86_EAX: PUTDST8(RecompPos,0x00); break;
	case x86_EBX: PUTDST8(RecompPos,0x03); break;
	case x86_ECX: PUTDST8(RecompPos,0x01); break;
	case x86_EDX: PUTDST8(RecompPos,0x02); break;
	case x86_ESI: PUTDST8(RecompPos,0x06); break;
	case x86_EDI: PUTDST8(RecompPos,0x07);
	}
}
void fpuLoadDwordFromN64Mem(int * StackPos,int x86reg) {
	*StackPos=(*StackPos-1) & 7;
	switch (x86reg) {
	case x86_EAX: PUTDST16(RecompPos,0x80D9); break;
	case x86_EBX: PUTDST16(RecompPos,0x83D9); break;
	case x86_ECX: PUTDST16(RecompPos,0x81D9); break;
	case x86_EDX: PUTDST16(RecompPos,0x82D9); break;
	case x86_ESI: PUTDST16(RecompPos,0x86D9); break;
	case x86_EDI: PUTDST16(RecompPos,0x87D9); break;
	case x86_EBP: PUTDST16(RecompPos,0x85D9);
	}
	PUTDST32(RecompPos,N64MEM);
}
void fpuLoadInt32bFromN64Mem(int * StackPos,int x86reg) {
	*StackPos=(*StackPos-1) & 7;
	switch (x86reg) {
	case x86_EAX: PUTDST16(RecompPos,0x80DB); break;
	case x86_EBX: PUTDST16(RecompPos,0x83DB); break;
	case x86_ECX: PUTDST16(RecompPos,0x81DB); break;
	case x86_EDX: PUTDST16(RecompPos,0x82DB); break;
	case x86_ESI: PUTDST16(RecompPos,0x86DB); break;
	case x86_EDI: PUTDST16(RecompPos,0x87DB); break;
	case x86_EBP: PUTDST16(RecompPos,0x85DB);
	}
	PUTDST32(RecompPos,N64MEM);
}
void fpuLoadIntegerDword(int * StackPos,void *Variable) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST16(RecompPos,0x05DB);
	PUTDST32(RecompPos,Variable);
}
void fpuLoadIntegerDwordFromX86Reg(int * StackPos,int x86Reg) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST8(RecompPos,0xDB);
	switch (x86Reg) {
	case x86_EAX: PUTDST8(RecompPos,0x00); break;
	case x86_EBX: PUTDST8(RecompPos,0x03); break;
	case x86_ECX: PUTDST8(RecompPos,0x01); break;
	case x86_EDX: PUTDST8(RecompPos,0x02); break;
	case x86_ESI: PUTDST8(RecompPos,0x06); break;
	case x86_EDI: PUTDST8(RecompPos,0x07);
	}
}
void fpuLoadIntegerQword(int * StackPos,void *Variable) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST16(RecompPos,0x2DDF);
	PUTDST32(RecompPos,Variable);
}
void fpuLoadIntegerQwordFromX86Reg(int * StackPos,int x86Reg) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST8(RecompPos,0xDF);
	switch (x86Reg) {
	case x86_EAX: PUTDST8(RecompPos,0x28); break;
	case x86_EBX: PUTDST8(RecompPos,0x2B); break;
	case x86_ECX: PUTDST8(RecompPos,0x29); break;
	case x86_EDX: PUTDST8(RecompPos,0x2A); break;
	case x86_ESI: PUTDST8(RecompPos,0x2E); break;
	case x86_EDI: PUTDST8(RecompPos,0x2F);
	}
}
void fpuLoadQword(int * StackPos,void *Variable) {
	*StackPos=(*StackPos-1) & 7;
	PUTDST16(RecompPos,0x05DD);
	PUTDST32(RecompPos,Variable);
}
void fpuLoadQwordFromX86Reg(int * StackPos,int x86Reg) {;
	*StackPos=(*StackPos-1) & 7;
	PUTDST8(RecompPos,0xDD);
	switch (x86Reg) {
	case x86_EAX: PUTDST8(RecompPos,0x00); break;
	case x86_EBX: PUTDST8(RecompPos,0x03); break;
	case x86_ECX: PUTDST8(RecompPos,0x01); break;
	case x86_EDX: PUTDST8(RecompPos,0x02); break;
	case x86_ESI: PUTDST8(RecompPos,0x06); break;
	case x86_EDI: PUTDST8(RecompPos,0x07);
	}
}
void fpuLoadQwordFromN64Mem(int * StackPos,int x86reg) {
	*StackPos=(*StackPos-1) & 7;
	switch (x86reg) {
	case x86_EAX: PUTDST16(RecompPos,0x80DD); break;
	case x86_EBX: PUTDST16(RecompPos,0x83DD); break;
	case x86_ECX: PUTDST16(RecompPos,0x81DD); break;
	case x86_EDX: PUTDST16(RecompPos,0x82DD); break;
	case x86_ESI: PUTDST16(RecompPos,0x86DD); break;
	case x86_EDI: PUTDST16(RecompPos,0x87DD); break;
	case x86_EBP: PUTDST16(RecompPos,0x85DD);
	}
	PUTDST32(RecompPos,N64MEM);
}
void fpuLoadReg(int * StackPos,int Reg) {
	*StackPos=(*StackPos-1) & 7;
	switch (Reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC0D9); break;
	case x86_ST1: PUTDST16(RecompPos,0xC1D9); break;
	case x86_ST2: PUTDST16(RecompPos,0xC2D9); break;
	case x86_ST3: PUTDST16(RecompPos,0xC3D9); break;
	case x86_ST4: PUTDST16(RecompPos,0xC4D9); break;
	case x86_ST5: PUTDST16(RecompPos,0xC5D9); break;
	case x86_ST6: PUTDST16(RecompPos,0xC6D9); break;
	case x86_ST7: PUTDST16(RecompPos,0xC7D9);
	}
}
void fpuMulDword(void *Variable) {
	PUTDST16(RecompPos,0x0DD8);
	PUTDST32(RecompPos,Variable);
}
void fpuMulDwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x08D8); break;
	case x86_EBX: PUTDST16(RecompPos,0x0BD8); break;
	case x86_ECX: PUTDST16(RecompPos,0x09D8); break;
	case x86_EDX: PUTDST16(RecompPos,0x0AD8); break;
	case x86_ESI: PUTDST16(RecompPos,0x0ED8); break;
	case x86_EDI: PUTDST16(RecompPos,0x0FD8);
	}
}
void fpuMulQword(void *Variable) {
	PUTDST16(RecompPos,0x0DDC);
	PUTDST32(RecompPos,Variable);
}
void fpuMulQwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x08DC); break;
	case x86_EBX: PUTDST16(RecompPos,0x0BDC); break;
	case x86_ECX: PUTDST16(RecompPos,0x09DC); break;
	case x86_EDX: PUTDST16(RecompPos,0x0ADC); break;
	case x86_ESI: PUTDST16(RecompPos,0x0EDC); break;
	case x86_EDI: PUTDST16(RecompPos,0x0FDC);
	}
}
void fpuMulReg(int x86reg) {
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC8D8); break;
	case x86_ST1: PUTDST16(RecompPos,0xC9D8); break;
	case x86_ST2: PUTDST16(RecompPos,0xCAD8); break;
	case x86_ST3: PUTDST16(RecompPos,0xCBD8); break;
	case x86_ST4: PUTDST16(RecompPos,0xCCD8); break;
	case x86_ST5: PUTDST16(RecompPos,0xCDD8); break;
	case x86_ST6: PUTDST16(RecompPos,0xCED8); break;
	case x86_ST7: PUTDST16(RecompPos,0xCFD8);
	}
}
void fpuMulRegPop(int x86reg) {
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xC8DE); break;
	case x86_ST1: PUTDST16(RecompPos,0xC9DE); break;
	case x86_ST2: PUTDST16(RecompPos,0xCADE); break;
	case x86_ST3: PUTDST16(RecompPos,0xCBDE); break;
	case x86_ST4: PUTDST16(RecompPos,0xCCDE); break;
	case x86_ST5: PUTDST16(RecompPos,0xCDDE); break;
	case x86_ST6: PUTDST16(RecompPos,0xCEDE); break;
	case x86_ST7: PUTDST16(RecompPos,0xCFDE);
	}
}
void fpuNeg(void) {
	PUTDST16(RecompPos,0xE0D9);
}
void fpuRound(void) {
	PUTDST16(RecompPos,0xFCD9);
}
void fpuSqrt(void) {
	PUTDST16(RecompPos,0xFAD9);
}
void fpuStoreControl(void *Variable) {
	PUTDST16(RecompPos,0x3DD9);
	PUTDST32(RecompPos,Variable);
}
void fpuStoreDword(int * StackPos,void *Variable,BOOL pop) {
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST16(RecompPos,(pop==FALSE)?0x15D9:0x1DD9);
	PUTDST32(RecompPos,Variable);
}
void fpuStoreDwordFromX86Reg(int * StackPos,int x86Reg,BOOL pop) {
	BYTE Command;
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST8(RecompPos,0xD9);
	switch (x86Reg) {
	case x86_EAX: Command=0x10; break;
	case x86_EBX: Command=0x13; break;
	case x86_ECX: Command=0x11; break;
	case x86_EDX: Command=0x12; break;
	case x86_ESI: Command=0x16; break;
	case x86_EDI: Command=0x17;
	}
	PUTDST8(RecompPos,(pop==FALSE)?Command:(Command+0x8));
}
void fpuStoreDwordToN64Mem(int * StackPos,int x86reg,BOOL Pop) {
	int s=(Pop==TRUE)?0x0800:0;
	if (Pop) { *StackPos=(*StackPos+1) & 7; }
	switch (x86reg) {
	case x86_EAX: PUTDST16(RecompPos,0x90D9|s); break;
	case x86_EBX: PUTDST16(RecompPos,0x93D9|s); break;
	case x86_ECX: PUTDST16(RecompPos,0x91D9|s); break;
	case x86_EDX: PUTDST16(RecompPos,0x92D9|s); break;
	case x86_ESI: PUTDST16(RecompPos,0x96D9|s); break;
	case x86_EDI: PUTDST16(RecompPos,0x97D9|s); break;
	case x86_EBP: PUTDST16(RecompPos,0x95D9|s);
	}
	PUTDST32(RecompPos,N64MEM);
}
void fpuStoreIntegerDword(int * StackPos,void *Variable,BOOL pop) {
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST16(RecompPos,(pop==FALSE)?0x15DB:0x1DDB);
	PUTDST32(RecompPos,Variable);
}
void fpuStoreIntegerDwordFromX86Reg(int * StackPos,int x86Reg,BOOL pop) {
	BYTE Command;
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST8(RecompPos,0xDB);
	switch (x86Reg) {
	case x86_EAX: Command=0x10; break;
	case x86_EBX: Command=0x13; break;
	case x86_ECX: Command=0x11; break;
	case x86_EDX: Command=0x12; break;
	case x86_ESI: Command=0x16; break;
	case x86_EDI: Command=0x17;
	}
	PUTDST8(RecompPos,(pop==FALSE)?Command:(Command+0x8));
}
void fpuStoreIntegerQword(int * StackPos,void *Variable,BOOL pop) {
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST16(RecompPos,(pop==FALSE)?0x35DF:0x3DDF);
	PUTDST32(RecompPos,Variable);
}
void fpuStoreIntegerQwordFromX86Reg(int * StackPos,int x86Reg,BOOL pop) {
	BYTE Command;
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST8(RecompPos,0xDF);
	switch (x86Reg) {
	case x86_EAX: Command=0x30; break;
	case x86_EBX: Command=0x33; break;
	case x86_ECX: Command=0x31; break;
	case x86_EDX: Command=0x32; break;
	case x86_ESI: Command=0x36; break;
	case x86_EDI: Command=0x37;
	}
	PUTDST8(RecompPos,(pop==FALSE)?Command:(Command+0x8));
}
void fpuStoreQwordFromX86Reg(int * StackPos,int x86Reg,BOOL pop) {
	BYTE Command;
	if (pop) { *StackPos=(*StackPos+1) & 7; }
	PUTDST8(RecompPos,0xDD);
	switch (x86Reg) {
	case x86_EAX: Command=0x10; break;
	case x86_EBX: Command=0x13; break;
	case x86_ECX: Command=0x11; break;
	case x86_EDX: Command=0x12; break;
	case x86_ESI: Command=0x16; break;
	case x86_EDI: Command=0x17;
	}
	PUTDST8(RecompPos,(pop==FALSE)?Command:(Command+0x8));
}
void fpuStoreStatus(void) {
	PUTDST16(RecompPos,0xE0DF);
}
void fpuSubDword(void *Variable) {
	PUTDST16(RecompPos,0x25D8);
	PUTDST32(RecompPos,Variable);
}
void fpuSubDwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x20D8); break;
	case x86_EBX: PUTDST16(RecompPos,0x23D8); break;
	case x86_ECX: PUTDST16(RecompPos,0x21D8); break;
	case x86_EDX: PUTDST16(RecompPos,0x22D8); break;
	case x86_ESI: PUTDST16(RecompPos,0x26D8); break;
	case x86_EDI: PUTDST16(RecompPos,0x27D8);
	}
}
void fpuSubDwordReverse(void *Variable) {
	PUTDST16(RecompPos,0x2DD8);
	PUTDST32(RecompPos,Variable);
}
void fpuSubQword(void *Variable) {
	PUTDST16(RecompPos,0x25DC);
	PUTDST32(RecompPos,Variable);
}
void fpuSubQwordRegPointer(int x86Pointer) {
	switch (x86Pointer) {
	case x86_EAX: PUTDST16(RecompPos,0x20DC); break;
	case x86_EBX: PUTDST16(RecompPos,0x23DC); break;
	case x86_ECX: PUTDST16(RecompPos,0x21DC); break;
	case x86_EDX: PUTDST16(RecompPos,0x22DC); break;
	case x86_ESI: PUTDST16(RecompPos,0x26DC); break;
	case x86_EDI: PUTDST16(RecompPos,0x27DC);
	}
}
void fpuSubQwordReverse(void *Variable) {
	PUTDST16(RecompPos,0x2DDC);
	PUTDST32(RecompPos,Variable);
}
void fpuSubReg(int x86reg) {
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xE0D8); break;
	case x86_ST1: PUTDST16(RecompPos,0xE1D8); break;
	case x86_ST2: PUTDST16(RecompPos,0xE2D8); break;
	case x86_ST3: PUTDST16(RecompPos,0xE3D8); break;
	case x86_ST4: PUTDST16(RecompPos,0xE4D8); break;
	case x86_ST5: PUTDST16(RecompPos,0xE5D8); break;
	case x86_ST6: PUTDST16(RecompPos,0xE6D8); break;
	case x86_ST7: PUTDST16(RecompPos,0xE7D8);
	}
}
void fpuSubRegPop(int x86reg) {
	switch (x86reg) {
	case x86_ST0: PUTDST16(RecompPos,0xE8DE); break;
	case x86_ST1: PUTDST16(RecompPos,0xE9DE); break;
	case x86_ST2: PUTDST16(RecompPos,0xEADE); break;
	case x86_ST3: PUTDST16(RecompPos,0xEBDE); break;
	case x86_ST4: PUTDST16(RecompPos,0xECDE); break;
	case x86_ST5: PUTDST16(RecompPos,0xEDDE); break;
	case x86_ST6: PUTDST16(RecompPos,0xEEDE); break;
	case x86_ST7: PUTDST16(RecompPos,0xEFDE);
	}
}
void fpuDivDwordReverse(void *Variable) {
	PUTDST16(RecompPos,0x3DD8);
	PUTDST32(RecompPos,Variable);
}
void fpuDivQwordReverse(void *Variable) {
	PUTDST16(RecompPos,0x3DDC);
	PUTDST32(RecompPos,Variable);
}
// FPU Utility
unsigned int fpucontrol;
/* returns and pushes current fpu state,bool for set normal */
int fpuSaveControl(BOOL bSetNormal) {
	_asm {
		fnstcw word ptr [fpucontrol]
	}
	if (bSetNormal==TRUE) {
		unsigned short fpunormal=fpucontrol & 0xF3FF;
		_asm {
			fldcw word ptr [fpunormal]
		}
	}
	return fpucontrol;
}
/* returns and pops fpu state previously pushed */
int fpuRestoreControl() {
	_asm {
		fldcw word ptr [fpucontrol]
	}
	return fpucontrol;
}
void fpuSetupDouble(void) {
	int temp=0;
	_asm {
		fnstcw word ptr [temp]
		or [temp],0x300
		and [temp],0xFFFFF3FF
		fldcw word ptr [temp]
	}
}