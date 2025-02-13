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
#include <Windows.h>
#include "main.h"
#include "cpu.h"
void SetupTLB_Entry (int Entry);
FASTTLB FastTlb[64];
TLB tlb[32];
BOOL AddressDefined ( DWORD VAddr) {
	DWORD i;
	if (VAddr >= 0x80000000 && VAddr <= 0xBFFFFFFF) {
		return TRUE;
	}
	for (i = 0; i < 64; i++) {
		if (FastTlb[i].ValidEntry == FALSE) { continue; }
		if (VAddr >= FastTlb[i].VSTART && VAddr <= FastTlb[i].VEND) {
			return TRUE;
		}
	}
	return FALSE;
}
void InitializeTLB (void) {
	DWORD count;
	for (count = 0; count < 32; count++) { tlb[count].EntryDefined = FALSE; }
	for (count = 0; count < 64; count++) { FastTlb[count].ValidEntry = FALSE; }
	SetupTLB();
}
void SetupTLB (void) {
	DWORD count;
	memset(TLB_ReadMap,0,(0xFFFFF * sizeof(DWORD)));
	memset(TLB_WriteMap,0,(0xFFFFF * sizeof(DWORD)));
	for (count = 0x80000000; count < 0xC0000000; count += 0x1000) {
		TLB_ReadMap[count >> 12] = ((DWORD)N64MEM + (count & 0x1FFFFFFF)) - count;
		TLB_WriteMap[count >> 12] = ((DWORD)N64MEM + (count & 0x1FFFFFFF)) - count;
	}
	for (count = 0; count < 32; count ++) { SetupTLB_Entry(count); }
}
void SetupTLB_Entry (int Entry) {
	int FastIndx;
	if (!tlb[Entry].EntryDefined) { return; }
	FastIndx = Entry << 1;
	FastTlb[FastIndx].VSTART=tlb[Entry].EntryHi.VPN2 << 13;
	FastTlb[FastIndx].VEND = FastTlb[FastIndx].VSTART + (tlb[Entry].PageMask.Mask << 12) + 0xFFF;
	FastTlb[FastIndx].PHYSSTART = tlb[Entry].EntryLo0.PFN << 12;
	FastTlb[FastIndx].VALID = tlb[Entry].EntryLo0.V;
	FastTlb[FastIndx].DIRTY = tlb[Entry].EntryLo0.D;
	FastTlb[FastIndx].GLOBAL = tlb[Entry].EntryLo0.GLOBAL & tlb[Entry].EntryLo1.GLOBAL;
	FastTlb[FastIndx].ValidEntry = FALSE;
	FastIndx = (Entry << 1) + 1;
	FastTlb[FastIndx].VSTART=(tlb[Entry].EntryHi.VPN2 << 13) + ((tlb[Entry].PageMask.Mask << 12) + 0xFFF + 1);
	FastTlb[FastIndx].VEND = FastTlb[FastIndx].VSTART + (tlb[Entry].PageMask.Mask << 12) + 0xFFF;
	FastTlb[FastIndx].PHYSSTART = tlb[Entry].EntryLo1.PFN << 12;
	FastTlb[FastIndx].VALID = tlb[Entry].EntryLo1.V;
	FastTlb[FastIndx].DIRTY = tlb[Entry].EntryLo1.D;
	FastTlb[FastIndx].GLOBAL = tlb[Entry].EntryLo0.GLOBAL & tlb[Entry].EntryLo1.GLOBAL;
	FastTlb[FastIndx].ValidEntry = FALSE;
	for ( FastIndx = Entry << 1; FastIndx <= (Entry << 1) + 1; FastIndx++) {
		DWORD count;
		DWORD physend = FastTlb[FastIndx].PHYSSTART + (tlb[Entry].PageMask.Mask << 12) + 0xFFF;
		if (!FastTlb[FastIndx].VALID) {
			FastTlb[FastIndx].ValidEntry = TRUE;
			continue;
		}
		if (FastTlb[FastIndx].VEND <= FastTlb[FastIndx].VSTART) {
			continue;
		}
		if (FastTlb[FastIndx].VSTART >= 0x80000000 && FastTlb[FastIndx].VEND <= 0xBFFFFFFF) {
			continue;
		}
		if (FastTlb[FastIndx].PHYSSTART > 0x1FFFFFFF || physend > 0x1FFFFFFF) {
			continue;
		}
		//test if overlap
		FastTlb[FastIndx].ValidEntry = TRUE;
		for (count = FastTlb[FastIndx].VSTART; count < FastTlb[FastIndx].VEND; count += 0x1000) {
			TLB_ReadMap[count >> 12] = ((DWORD)N64MEM + (count - FastTlb[FastIndx].VSTART + FastTlb[FastIndx].PHYSSTART)) - count;
			if (!FastTlb[FastIndx].DIRTY) { continue; }
			TLB_WriteMap[count >> 12] = ((DWORD)N64MEM + (count - FastTlb[FastIndx].VSTART + FastTlb[FastIndx].PHYSSTART)) - count;
		}
	}
}
void TLB_Probe (void) {
	int Counter;
	INDEX_REGISTER |= 0x80000000;
	for (Counter = 0; Counter < 32; Counter ++) {
		DWORD TlbValue = tlb[Counter].EntryHi.Value & (~tlb[Counter].PageMask.Mask << 13);
		DWORD EntryHi = ENTRYHI_REGISTER & (~tlb[Counter].PageMask.Mask << 13);
		if (TlbValue == EntryHi) {
			BOOL Global = (tlb[Counter].EntryHi.Value & 0x100) != 0;
			BOOL SameAsid = ((tlb[Counter].EntryHi.Value & 0xFF) == (ENTRYHI_REGISTER & 0xFF));
			if (Global || SameAsid) {
				INDEX_REGISTER = Counter;
				return;
			}
		}
	}
}
void TLB_Read (void) {
	DWORD index = INDEX_REGISTER & 0x1F;
	PAGE_MASK_REGISTER = tlb[index].PageMask.Value ;
	ENTRYHI_REGISTER = (tlb[index].EntryHi.Value & ~tlb[index].PageMask.Value) ;
	ENTRYLO0_REGISTER = tlb[index].EntryLo0.Value;
	ENTRYLO1_REGISTER = tlb[index].EntryLo1.Value;
}
BOOL TranslateVaddr ( DWORD * Addr) {
	if (TLB_ReadMap[*Addr >> 12] == 0) { return FALSE; }
	*Addr = (DWORD)((BYTE *)(TLB_ReadMap[*Addr >> 12] + *Addr) - N64MEM);
	return TRUE;
}
void _fastcall WriteTLBEntry (int index) {
	int FastIndx;
	FastIndx = index << 1;
	if ((PROGRAM_COUNTER >= FastTlb[FastIndx].VSTART &&
		PROGRAM_COUNTER < FastTlb[FastIndx].VEND &&
		FastTlb[FastIndx].ValidEntry && FastTlb[FastIndx].VALID)
		||
		(PROGRAM_COUNTER >= FastTlb[FastIndx + 1].VSTART &&
		PROGRAM_COUNTER < FastTlb[FastIndx + 1].VEND &&
		FastTlb[FastIndx + 1].ValidEntry && FastTlb[FastIndx + 1].VALID))
		return;
	if (tlb[index].EntryDefined) {
		DWORD count;
		for ( FastIndx = index << 1; FastIndx <= (index << 1) + 1; FastIndx++) {
			if (!FastTlb[FastIndx].ValidEntry) { continue; }
			if (!FastTlb[FastIndx].VALID) { continue; }
			for (count = FastTlb[FastIndx].VSTART; count < FastTlb[FastIndx].VEND; count += 0x1000) {
				TLB_ReadMap[count >> 12] = 0;
				TLB_WriteMap[count >> 12] = 0;
			}
		}
	}
	tlb[index].PageMask.Value = PAGE_MASK_REGISTER;
	tlb[index].EntryHi.Value = ENTRYHI_REGISTER;
	tlb[index].EntryLo0.Value = ENTRYLO0_REGISTER;
	tlb[index].EntryLo1.Value = ENTRYLO1_REGISTER;
	tlb[index].EntryDefined = TRUE;
	SetupTLB_Entry(index);
}