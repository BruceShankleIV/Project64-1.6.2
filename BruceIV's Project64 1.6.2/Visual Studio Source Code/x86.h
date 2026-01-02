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
enum x86RegValues {
	x86_Any	= 0,x86_EAX,x86_EBX,x86_ECX,x86_EDX,x86_ESI,x86_EDI,x86_EBP,x86_ESP,x86_Any8Bit
};
enum x86FpuValues {
	x86_ST0,x86_ST1,x86_ST2,x86_ST3,x86_ST4,x86_ST5,x86_ST6,x86_ST7
};
void AdcConstToVariable              (void *Variable,BYTE Constant);
void AdcVariableToX86reg             (int x86reg,void * Variable);
void AdcX86RegToX86Reg               (int Destination,int Source);
void AddConstToVariable              (DWORD Const,void *Variable);
void AddConstToX86Reg                (int x86Reg,DWORD Const);
void AddVariableToX86reg             (int x86reg,void * Variable);
void AddX86regToVariable             (int x86reg,void * Variable);
void AddX86RegToX86Reg               (int Destination,int Source);
void AndConstToVariable              (DWORD Const,void * Variable);
void AndConstToX86Reg                (int x86Reg,DWORD Const);
void AndVariableDispToX86Reg         (void * Variable,int x86Reg,int AddrReg,int Multiplier);
void AndVariableToX86Reg			 (void * Variable,int x86Reg);
void AndX86RegToX86Reg               (int Destination,int Source);
void Call_Direct                     (void * FunctAddress);
void CompConstToVariable             (DWORD Const,void * Variable);
void CompConstToX86reg               (int x86Reg,DWORD Const);
void CompX86regToVariable            (int x86Reg,void * Variable);
void CompX86RegToX86Reg              (int Destination,int Source);
void DecX86reg                       (int x86Reg);
void DivX86reg                       (int x86reg);
void idivX86reg                      (int x86reg);
void imulX86reg                      (int x86reg);
void IncX86reg                       (int x86Reg);
void JaeLabel8                       (BYTE Value);
void JaeLabel32                      (DWORD Value);
void JaLabel8                        (BYTE Value);
void JaLabel32                       (DWORD Value);
void JbLabel8                        (BYTE Value);
void JbLabel32                       (DWORD Value);
void JecxzLabel8                     (BYTE Value);
void JeLabel8                        (BYTE Value);
void JeLabel32                       (DWORD Value);
void JgeLabel32                      (DWORD Value);
void JgLabel8                        (BYTE Value);
void JgLabel32                       (DWORD Value);
void JleLabel8                       (BYTE Value);
void JleLabel32                      (DWORD Value);
void JlLabel8                        (BYTE Value);
void JlLabel32                       (DWORD Value);
void JmpDirectReg                    (int x86reg);
void JmpIndirectLabel32              (DWORD location);
void JmpIndirectReg                  (int x86reg);
void JmpLabel8                       (BYTE Value);
void JmpLabel32                      (DWORD Value);
void JneLabel8                       (BYTE Value);
void JneLabel32                      (DWORD Value);
void JnsLabel8                       (BYTE Value);
void JnsLabel32                      (DWORD Value);
void JsLabel32                       (DWORD Value);
void LeaRegReg                       (int x86RegDest,int x86RegSrc,int multiplier);
void LeaSourceAndOffset              (int x86DestReg,int x86SourceReg,int offset);
void MoveConstByteToN64Mem           (BYTE Const,int AddrReg);
void MoveConstHalfToN64Mem           (WORD Const,int AddrReg);
void MoveConstByteToVariable         (BYTE Const,void *Variable);
void MoveConstByteToX86regPointer    (BYTE Const,int AddrReg1,int AddrReg2);
void MoveConstHalfToVariable         (WORD Const,void *Variable);
void MoveConstHalfToX86regPointer    (WORD Const,int AddrReg1,int AddrReg2);
void MoveConstToMemoryDisp           (DWORD Const,int AddrReg,DWORD Disp);
void MoveConstToN64Mem               (DWORD Const,int AddrReg);
void MoveConstToN64MemDisp           (DWORD Const,int AddrReg,BYTE Disp);
void MoveConstToVariable             (DWORD Const,void *Variable);
void MoveConstToX86Pointer           (DWORD Const,int X86Pointer);
void MoveConstToX86reg               (DWORD Const,int x86reg);
void MoveConstToX86regPointer        (DWORD Const,int AddrReg1,int AddrReg2);
void MoveN64MemDispToX86reg          (int x86reg,int AddrReg,BYTE Disp);
void MoveN64MemToX86reg              (int x86reg,int AddrReg);
void MoveN64MemToX86regByte          (int x86reg,int AddrReg);
void MoveN64MemToX86regHalf          (int x86reg,int AddrReg);
void MoveSxByteX86regPointerToX86reg (int AddrReg1,int AddrReg2,int x86reg);
void MoveSxHalfX86regPointerToX86reg (int AddrReg1,int AddrReg2,int x86reg);
void MoveSxN64MemToX86regByte        (int x86reg,int AddrReg);
void MoveSxN64MemToX86regHalf        (int x86reg,int AddrReg);
void MoveSxVariableToX86regByte      (void *Variable,int x86reg);
void MoveSxVariableToX86regHalf      (void *Variable,int x86reg);
void MoveVariableDispToX86Reg        (void *Variable,int x86Reg,int AddrReg,int Multiplier);
void MoveVariableToX86reg            (void *Variable,int x86reg);
void MoveX86PointerToX86reg          (int x86reg,int X86Pointer);
void MoveX86regByteToN64Mem          (int x86reg,int AddrReg);
void MoveX86regByteToVariable        (int x86reg,void * Variable);
void MoveX86regByteToX86regPointer   (int x86reg,int AddrReg1,int AddrReg2);
void MoveX86regHalfToN64Mem          (int x86reg,int AddrReg);
void MoveX86regHalfToVariable        (int x86reg,void * Variable);
void MoveX86regHalfToX86regPointer   (int x86reg,int AddrReg1,int AddrReg2);
void MoveX86regPointerToX86reg       (int AddrReg1,int AddrReg2,int x86reg);
void MoveX86regPointerToX86regDisp8  (int AddrReg1,int AddrReg2,int x86reg,BYTE offset);
void MoveX86regToMemory              (int x86reg,int AddrReg,DWORD Disp);
void MoveX86regToN64Mem              (int x86reg,int AddrReg);
void MoveX86regToN64MemDisp          (int x86reg,int AddrReg,BYTE Disp);
void MoveX86regToVariable            (int x86reg,void * Variable);
void MoveX86RegToX86Reg              (int Source,int Destination);
void MoveX86regToX86Pointer          (int x86reg,int X86Pointer);
void MoveX86regToX86regPointer       (int x86reg,int AddrReg1,int AddrReg2);
void MoveZxByteX86regPointerToX86reg (int AddrReg1,int AddrReg2,int x86reg);
void MoveZxHalfX86regPointerToX86reg (int AddrReg1,int AddrReg2,int x86reg);
void MoveZxN64MemToX86regByte        (int x86reg,int AddrReg);
void MoveZxN64MemToX86regHalf        (int x86reg,int AddrReg);
void MoveZxVariableToX86regByte      (void *Variable,int x86reg);
void MoveZxVariableToX86regHalf      (void *Variable,int x86reg);
void MulX86reg                       (int x86reg);
void NotX86Reg                       (int x86Reg);
void OrConstToVariable               (DWORD Const,void * Variable);
void OrConstToX86Reg                 (DWORD Const,int  x86Reg);
void OrVariableToX86Reg              (void * Variable,int x86Reg);
void OrX86RegToVariable              (void * Variable,int x86Reg);
void OrX86RegToX86Reg                (int Destination,int Source);
void Popad                           (void);
void Pushad                          (void);
void Push					         (int x86reg);
void Pop					         (int x86reg);
void PushImm32                       (char * String,DWORD Value);
void Ret                             (void);
void Seta                            (int x86reg);
void Setae                           (int x86reg);
void SetaVariable                    (void * Variable);
void Setb                            (int x86reg);
void SetbVariable                    (void * Variable);
void Setg                            (int x86reg);
void SetgVariable                    (void * Variable);
void Setl                            (int x86reg);
void SetlVariable                    (void * Variable);
void Setz					         (int x86reg);
void Setnz					         (int x86reg);
void ShiftLeftDouble                 (int Destination,int Source);
void ShiftLeftDoubleImmed            (int Destination,int Source,BYTE Immediate);
void ShiftLeftSign                   (int x86reg);
void ShiftLeftSignImmed              (int x86reg,BYTE Immediate);
void ShiftRightDouble                (int Destination,int Source);
void ShiftRightDoubleImmed           (int Destination,int Source,BYTE Immediate);
void ShiftRightSign                  (int x86reg);
void ShiftRightSignImmed             (int x86reg,BYTE Immediate);
void ShiftRightUnsign                (int x86reg);
void ShiftRightUnsignImmed           (int x86reg,BYTE Immediate);
void SbbConstFromX86Reg              (int x86Reg,DWORD Const);
void SbbVariableFromX86reg           (int x86reg,void * Variable);
void SbbX86RegToX86Reg               (int Destination,int Source);
void SubConstFromVariable            (DWORD Const,void *Variable);
void SubConstFromX86Reg              (int x86Reg,DWORD Const);
void SubVariableFromX86reg           (int x86reg,void * Variable);
void SubX86RegToX86Reg               (int Destination,int Source);
void TestConstToX86Reg               (DWORD Const,int x86reg);
void TestVariable                    (DWORD Const,void * Variable);
void TestX86RegToX86Reg              (int Destination,int Source);
void XorConstToX86Reg                (int x86Reg,DWORD Const);
void XorX86RegToX86Reg               (int Source,int Destination);
void XorVariableToX86reg             (void *Variable,int x86reg);
void fpuAbs					         (void);
void fpuAddDword			         (void *Variable);
void fpuAddDwordRegPointer           (int x86Pointer);
void fpuAddQword			         (void *Variable);
void fpuAddQwordRegPointer           (int x86Pointer);
void fpuAddReg				         (int x86reg);
void fpuAddRegPop			         (int * StackPos,int x86reg);
void fpuComDword			         (void *Variable,BOOL Pop);
void fpuComDwordRegPointer           (int x86Pointer,BOOL Pop);
void fpuComQword			         (void *Variable,BOOL Pop);
void fpuComQwordRegPointer           (int x86Pointer,BOOL Pop);
void fpuComReg                       (int x86reg,BOOL Pop);
void fpuDivDword			         (void *Variable);
void fpuDivDwordRegPointer           (int x86Pointer);
void fpuDivQword			         (void *Variable);
void fpuDivQwordRegPointer           (int x86Pointer);
void fpuDivReg                       (int Reg);
void fpuDivRegPop			         (int x86reg);
void fpuExchange                     (int Reg);
void fpuFree                         (int Reg);
void fpuDecStack                     (int * StackPos);
void fpuIncStack                     (int * StackPos);
void fpuLoadControl			         (void *Variable);
void fpuLoadDword			         (int * StackPos,void *Variable);
void fpuLoadDwordFromX86Reg          (int * StackPos,int x86reg);
void fpuLoadDwordFromN64Mem          (int * StackPos,int x86reg);
void fpuLoadInt32bFromN64Mem         (int * StackPos,int x86reg);
void fpuLoadIntegerDword	         (int * StackPos,void *Variable);
void fpuLoadIntegerDwordFromX86Reg   (int * StackPos,int x86Reg);
void fpuLoadIntegerQword	         (int * StackPos,void *Variable);
void fpuLoadIntegerQwordFromX86Reg   (int * StackPos,int x86Reg);
void fpuLoadQword			         (int * StackPos,void *Variable);
void fpuLoadQwordFromX86Reg          (int * StackPos,int x86Reg);
void fpuLoadQwordFromN64Mem          (int * StackPos,int x86reg);
void fpuLoadReg                      (int * StackPos,int Reg);
void fpuMulDword                     (void *Variable);
void fpuMulDwordRegPointer           (int x86Pointer);
void fpuMulQword                     (void *Variable);
void fpuMulQwordRegPointer           (int x86Pointer);
void fpuMulReg                       (int x86reg);
void fpuMulRegPop                    (int x86reg);
void fpuNeg					         (void);
void fpuRound				         (void);
void fpuSqrt				         (void);
void fpuStoreControl		         (void *Variable);
void fpuStoreDword			         (int * StackPos,void *Variable,BOOL pop);
void fpuStoreDwordFromX86Reg         (int * StackPos,int x86Reg,BOOL pop);
void fpuStoreDwordToN64Mem	         (int * StackPos,int x86reg,BOOL Pop);
void fpuStoreIntegerDword            (int * StackPos,void *Variable,BOOL pop);
void fpuStoreIntegerDwordFromX86Reg  (int * StackPos,int x86Reg,BOOL pop);
void fpuStoreIntegerQword            (int * StackPos,void *Variable,BOOL pop);
void fpuStoreIntegerQwordFromX86Reg  (int * StackPos,int x86Reg,BOOL pop);
void fpuStoreQword			         (int * StackPos,void *Variable,char *VariableName,BOOL pop);
void fpuStoreQwordFromX86Reg         (int * StackPos,int x86Reg,BOOL pop);
void fpuStoreStatus			         (void);
void fpuSubDword			         (void *Variable);
void fpuSubDwordRegPointer           (int x86Pointer);
void fpuSubDwordReverse              (void *Variable);
void fpuSubQword			         (void *Variable);
void fpuSubQwordRegPointer           (int x86Pointer);
void fpuSubQwordReverse              (void *Variable);
void fpuSubReg				         (int x86reg);
void fpuSubRegPop			         (int x86reg);