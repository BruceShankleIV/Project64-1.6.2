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
#include "Main.h"
#include "CPU.h"
#include "Plugin.h"
void __cdecl AiCheckInterrupts (void) {
	CPU_Action.CheckInterrupts = TRUE;
	CPU_Action.DoSomething = TRUE;
}
void __cdecl CheckInterrupts (void) {
	MI_INTR_REG &= ~MI_INTR_AI;
	MI_INTR_REG |= (AudioIntrReg & MI_INTR_AI);
	if ((MI_INTR_MASK_REG & MI_INTR_REG) != 0) {
		FAKE_CAUSE_REGISTER |= CAUSE_IP2;
	} else  {
		FAKE_CAUSE_REGISTER &= ~CAUSE_IP2;
	}
	if ((STATUS_REGISTER & STATUS_IE) == 0 || (STATUS_REGISTER & STATUS_EXL) != 0 || (STATUS_REGISTER & STATUS_ERL) != 0) { return; }
	if ((STATUS_REGISTER & FAKE_CAUSE_REGISTER & 0xFF00) != 0) {
		if (!CPU_Action.DoInterrupt) {
			CPU_Action.DoSomething = TRUE;
			CPU_Action.DoInterrupt = TRUE;
		}
	}
}
void DoAddressError (BOOL DelaySlot,DWORD BadVaddr,BOOL FromRead) {
	if (FromRead) {
		CAUSE_REGISTER = EXC_RADE;
	} else {
		CAUSE_REGISTER = EXC_WADE;
	}
	BAD_VADDR_REGISTER = BadVaddr;
	if (DelaySlot) {
		CAUSE_REGISTER |= CAUSE_BD;
		EPC_REGISTER = PROGRAM_COUNTER - 4;
	} else {
		EPC_REGISTER = PROGRAM_COUNTER;
	}
	STATUS_REGISTER |= STATUS_EXL;
	PROGRAM_COUNTER = 0x80000180;
}
void _fastcall DoCopUnusableException (BOOL DelaySlot,int Coprocessor) {
	CAUSE_REGISTER = EXC_CPU;
	if (Coprocessor == 1) { CAUSE_REGISTER |= 0x10000000; }
	if (DelaySlot) {
		CAUSE_REGISTER |= CAUSE_BD;
		EPC_REGISTER = PROGRAM_COUNTER - 4;
	} else {
		EPC_REGISTER = PROGRAM_COUNTER;
	}
	STATUS_REGISTER |= STATUS_EXL;
	PROGRAM_COUNTER = 0x80000180;
}
void DoIntrException (BOOL DelaySlot) {
	if ((STATUS_REGISTER & STATUS_IE) == 0 || (STATUS_REGISTER & STATUS_EXL) != 0 || (STATUS_REGISTER & STATUS_ERL) != 0) { return; }
	CAUSE_REGISTER = FAKE_CAUSE_REGISTER;
	CAUSE_REGISTER |= EXC_INT;
	if (DelaySlot) {
		CAUSE_REGISTER |= CAUSE_BD;
		EPC_REGISTER = PROGRAM_COUNTER - 4;
	} else {
		EPC_REGISTER = PROGRAM_COUNTER;
	}
	STATUS_REGISTER |= STATUS_EXL;
	PROGRAM_COUNTER = 0x80000180;
}
void _fastcall DoTLBMiss (BOOL DelaySlot,DWORD BadVaddr) {
	CAUSE_REGISTER = EXC_RMISS;
	BAD_VADDR_REGISTER = BadVaddr;
	CONTEXT_REGISTER &= 0xFF80000F;
	CONTEXT_REGISTER |= (BadVaddr >> 9) & 0x007FFFF0;
	ENTRYHI_REGISTER = (BadVaddr & 0xFFFFE000);
	if ((STATUS_REGISTER & STATUS_EXL) == 0) {
		if (DelaySlot) {
			CAUSE_REGISTER |= CAUSE_BD;
			EPC_REGISTER = PROGRAM_COUNTER - 4;
		} else {
			EPC_REGISTER = PROGRAM_COUNTER;
		}
		if (AddressDefined(BadVaddr)) {
			PROGRAM_COUNTER = 0x80000180;
		} else {
			PROGRAM_COUNTER = 0x80000000;
		}
		STATUS_REGISTER |= STATUS_EXL;
	} else {
		PROGRAM_COUNTER = 0x80000180;
	}
}
void _fastcall DoSysCallException(BOOL DelaySlot) {
	CAUSE_REGISTER = EXC_SYSCALL;
	if (DelaySlot) {
		CAUSE_REGISTER |= CAUSE_BD;
		EPC_REGISTER = PROGRAM_COUNTER - 4;
	} else {
		EPC_REGISTER = PROGRAM_COUNTER;
	}
	STATUS_REGISTER |= STATUS_EXL;
	PROGRAM_COUNTER = 0x80000180;
}