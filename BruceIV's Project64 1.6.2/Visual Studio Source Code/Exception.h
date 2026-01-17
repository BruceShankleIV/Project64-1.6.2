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
#define	EXC_CODE(x)	((x)<<2)
#define	EXC_INT					EXC_CODE(0)	/*interrupt*/
#define	EXC_RMISS				EXC_CODE(2)	/*Read TLB Miss*/
#define	EXC_RADE				EXC_CODE(4)	/*Read Address Error*/
#define	EXC_WADE				EXC_CODE(5)	/*Write Address Error*/
#define	EXC_SYSCALL				EXC_CODE(8)	/*SYSCALL*/
#define	EXC_CPU					EXC_CODE(11)/*CoProcessor Unusable*/
void __cdecl AiCheckInterrupts      (void);
void __cdecl CheckInterrupts        (void);
void DoAddressError         (BOOL DelaySlot,DWORD BadVaddr,BOOL FromRead);
void _fastcall DoCopUnusableException (BOOL DelaySlot,int Coprocessor);
void DoIntrException        (BOOL DelaySlot);
void _fastcall DoTLBMiss              (BOOL DelaySlot,DWORD BadVaddr);
void _fastcall DoSysCallException (BOOL DelaySlot);