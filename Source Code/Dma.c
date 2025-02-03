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
#include <stdio.h>
#include "main.h"
#include "CPU.h"
int DMAUsed;
void FirstDMA (void) {
	switch (GetCicChipID(ROM)) {
	case 1: *(DWORD *)&N64MEM[0x318] = RDRAMsize; break;
	case 2: *(DWORD *)&N64MEM[0x318] = RDRAMsize; break;
	case 3: *(DWORD *)&N64MEM[0x318] = RDRAMsize; break;
	case 5: *(DWORD *)&N64MEM[0x3F0] = RDRAMsize; break;
	case 6: *(DWORD *)&N64MEM[0x318] = RDRAMsize; break;
	case 9: *(DWORD *)&N64MEM[0x318] = RDRAMsize; break;
	}
}
void PI_DMA_READ (void) {
	if ( PI_DRAM_ADDR_REG + PI_RD_LEN_REG + 1 > RDRAMsize) {
		PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
		MI_INTR_REG |= MI_INTR_PI;
		CheckInterrupts();
		return;
	}
	if ( PI_CART_ADDR_REG >= 0x08000000 && PI_CART_ADDR_REG <= 0x08010000) {
		if (SaveUsing == Auto) { SaveUsing = SRAM; }
		if (SaveUsing == SRAM) {
			DmaToSRAM(
				N64MEM+PI_DRAM_ADDR_REG,
				PI_CART_ADDR_REG - 0x08000000,
				PI_RD_LEN_REG + 1
			);
			PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
			MI_INTR_REG |= MI_INTR_PI;
			CheckInterrupts();
			return;
		}
		if (SaveUsing == FlashRAM) {
			DmaToFlashRAM(
				N64MEM+PI_DRAM_ADDR_REG,
				PI_CART_ADDR_REG - 0x08000000,
				PI_WR_LEN_REG + 1
			);
			PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
			MI_INTR_REG |= MI_INTR_PI;
			CheckInterrupts();
			return;
		}
	}
	if (SaveUsing == FlashRAM) {
		PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
		MI_INTR_REG |= MI_INTR_PI;
		CheckInterrupts();
		return;
	}
	PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
	MI_INTR_REG |= MI_INTR_PI;
	CheckInterrupts();
	return;
}
void PI_DMA_WRITE (void) {
	DWORD i;
	PI_STATUS_REG |= PI_STATUS_DMA_BUSY;
	if ( PI_DRAM_ADDR_REG + PI_WR_LEN_REG + 1 > RDRAMsize) {
		PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
		MI_INTR_REG |= MI_INTR_PI;
		CheckInterrupts();
		return;
	}
	if ( PI_CART_ADDR_REG >= 0x08000000 && PI_CART_ADDR_REG <= 0x08010000) {
		if (SaveUsing == Auto) { SaveUsing = SRAM; }
		if (SaveUsing == SRAM) {
			DmaFromSRAM(
				N64MEM+PI_DRAM_ADDR_REG,
				PI_CART_ADDR_REG - 0x08000000,
				PI_WR_LEN_REG + 1
			);
			PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
			MI_INTR_REG |= MI_INTR_PI;
			CheckInterrupts();
			return;
		}
		if (SaveUsing == FlashRAM) {
			DmaFromFlashRAM(
				N64MEM+PI_DRAM_ADDR_REG,
				PI_CART_ADDR_REG - 0x08000000,
				PI_WR_LEN_REG + 1
			);
			PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
			MI_INTR_REG |= MI_INTR_PI;
			CheckInterrupts();
		}
		return;
	}
	if ( PI_CART_ADDR_REG >= 0x06000000 && PI_CART_ADDR_REG < 0x08000000) {
		PI_CART_ADDR_REG -= 0x06000000;
		if (PI_CART_ADDR_REG + PI_WR_LEN_REG + 1 < RomFileSize) {
			for (i = 0; i < PI_WR_LEN_REG + 1; i ++) {
				*(N64MEM+((PI_DRAM_ADDR_REG + i) ^ 3)) =  *(ROM+((PI_CART_ADDR_REG + i) ^ 3));
			}
		} else if (RomFileSize > PI_CART_ADDR_REG) {
			DWORD Len;
			Len = RomFileSize - PI_CART_ADDR_REG;
			for (i = 0; i < Len; i ++) {
				*(N64MEM+((PI_DRAM_ADDR_REG + i) ^ 3)) =  *(ROM+((PI_CART_ADDR_REG + i) ^ 3));
			}
			for (i = Len; i < PI_WR_LEN_REG + 1 - Len; i ++) {
				*(N64MEM+((PI_DRAM_ADDR_REG + i) ^ 3)) =  0;
			}
		}
		PI_CART_ADDR_REG += 0x06000000;
		if (!DMAUsed) {
			DMAUsed = TRUE;
			FirstDMA();
		}
		PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
		MI_INTR_REG |= MI_INTR_PI;
		CheckInterrupts();
		CheckTimer();
		return;
	}
	if ( PI_CART_ADDR_REG >= 0x10000000 && PI_CART_ADDR_REG <= 0x1FBFFFFF) {
		PI_CART_ADDR_REG -= 0x10000000;
		if (PI_CART_ADDR_REG + PI_WR_LEN_REG + 1 < RomFileSize) {
			for (i = 0; i < PI_WR_LEN_REG + 1; i ++) {
				*(N64MEM+((PI_DRAM_ADDR_REG + i) ^ 3)) =  *(ROM+((PI_CART_ADDR_REG + i) ^ 3));
			}
		} else if (RomFileSize > PI_CART_ADDR_REG) {
			DWORD Len;
			Len = RomFileSize - PI_CART_ADDR_REG;
			for (i = 0; i < Len; i ++) {
				*(N64MEM+((PI_DRAM_ADDR_REG + i) ^ 3)) =  *(ROM+((PI_CART_ADDR_REG + i) ^ 3));
			}
			for (i = Len; i < PI_WR_LEN_REG + 1 - Len; i ++) {
				*(N64MEM+((PI_DRAM_ADDR_REG + i) ^ 3)) =  0;
			}
		}
		PI_CART_ADDR_REG += 0x10000000;
		if (!DMAUsed) {
			DMAUsed = TRUE;
			FirstDMA();
		}
		PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
		MI_INTR_REG |= MI_INTR_PI;
		CheckInterrupts();
		CheckTimer();
		return;
	}
	PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
	MI_INTR_REG |= MI_INTR_PI;
	CheckInterrupts();
}
void SI_DMA_READ (void) {
	BYTE * PifRamPos = &PIF_Ram[0];
	if ((int)SI_DRAM_ADDR_REG > (int)RDRAMsize) {
		return;
	}
	PifRamRead();
	SI_DRAM_ADDR_REG &= 0xFFFFFFF8;
	if ((int)SI_DRAM_ADDR_REG < 0) {
		int count, RDRAMPos;
		RDRAMPos = (int)SI_DRAM_ADDR_REG;
		for (count = 0; count < 0x40; count++, RDRAMPos++) {
			if (RDRAMPos < 0) { continue; }
			N64MEM[RDRAMPos ^3] = PIF_Ram[count];
		}
	} else {
		_asm {
			mov edi, dword ptr [RegSI]
			mov edi, dword ptr [edi]
			add edi, N64MEM
			mov ecx, PifRamPos
			mov edx, 0
	memcpyloop:
			mov eax, dword ptr [ecx + edx]
			bswap eax
			mov  dword ptr [edi + edx],eax
			mov eax, dword ptr [ecx + edx + 4]
			bswap eax
			mov  dword ptr [edi + edx + 4],eax
			mov eax, dword ptr [ecx + edx + 8]
			bswap eax
			mov  dword ptr [edi + edx + 8],eax
			mov eax, dword ptr [ecx + edx + 12]
			bswap eax
			mov  dword ptr [edi + edx + 12],eax
			add edx, 16
			cmp edx, 64
			jb memcpyloop
		}
	}
	if (DelaySI) {
		ChangeTimer(SiTimer,0x900);
	} else {
		MI_INTR_REG |= MI_INTR_SI;
		SI_STATUS_REG |= SI_STATUS_INTERRUPT;
		CheckInterrupts();
	}
}
void SI_DMA_WRITE (void) {
	BYTE * PifRamPos = &PIF_Ram[0];
	if ((int)SI_DRAM_ADDR_REG > (int)RDRAMsize) {
		return;
	}
	SI_DRAM_ADDR_REG &= 0xFFFFFFF8;
	if ((int)SI_DRAM_ADDR_REG < 0) {
		int count, RDRAMPos;
		RDRAMPos = (int)SI_DRAM_ADDR_REG;
		for (count = 0; count < 0x40; count++, RDRAMPos++) {
			if (RDRAMPos < 0) { PIF_Ram[count] = 0; continue; }
			PIF_Ram[count] = N64MEM[RDRAMPos ^3];
		}
	} else {
		_asm {
			mov ecx, dword ptr [RegSI]
			mov ecx, dword ptr [ecx]
			add ecx, N64MEM
			mov edi, PifRamPos
			mov edx, 0
	memcpyloop:
			mov eax, dword ptr [ecx + edx]
			bswap eax
			mov  dword ptr [edi + edx],eax
			mov eax, dword ptr [ecx + edx + 4]
			bswap eax
			mov  dword ptr [edi + edx + 4],eax
			mov eax, dword ptr [ecx + edx + 8]
			bswap eax
			mov  dword ptr [edi + edx + 8],eax
			mov eax, dword ptr [ecx + edx + 12]
			bswap eax
			mov  dword ptr [edi + edx + 12],eax
			add edx, 16
			cmp edx, 64
			jb memcpyloop
		}
	}
	PifRamWrite();
	if (DelaySI) {
		ChangeTimer(SiTimer,0x900);
	} else {
		MI_INTR_REG |= MI_INTR_SI;
		SI_STATUS_REG |= SI_STATUS_INTERRUPT;
		CheckInterrupts();
	}
}
void SP_DMA_READ (void) {
	SP_DRAM_ADDR_REG &= 0x1FFFFFFF;
	if (SP_DRAM_ADDR_REG > RDRAMsize) {
		SP_DMA_BUSY_REG = 0;
		SP_STATUS_REG  &= ~SP_STATUS_DMA_BUSY;
		return;
	}
	if (SP_RD_LEN_REG + 1  + (SP_MEM_ADDR_REG & 0xFFF) > 0x1000) {
		SP_DMA_BUSY_REG = 0;
		SP_STATUS_REG  &= ~SP_STATUS_DMA_BUSY;
		return;
	}
	if ((SP_MEM_ADDR_REG & 3) != 0 || (SP_DRAM_ADDR_REG & 3) != 0 || ((SP_RD_LEN_REG + 1) & 3) != 0) ExitThread(0);
	memcpy( DMEM + (SP_MEM_ADDR_REG & 0x1FFF), N64MEM + SP_DRAM_ADDR_REG,
		SP_RD_LEN_REG + 1 );
	SP_DMA_BUSY_REG = 0;
	SP_STATUS_REG  &= ~SP_STATUS_DMA_BUSY;
}
void SP_DMA_WRITE (void) {
	if (SP_DRAM_ADDR_REG > RDRAMsize) {
		return;
	}
	if (SP_WR_LEN_REG + 1 + (SP_MEM_ADDR_REG & 0xFFF) > 0x1000) {
		return;
	}
	if ((SP_MEM_ADDR_REG & 3) != 0) { _asm int 3 }
	if ((SP_DRAM_ADDR_REG & 3) != 0) { _asm int 3 }
	if (((SP_WR_LEN_REG + 1) & 3) != 0) { _asm int 3 }
	memcpy( N64MEM + SP_DRAM_ADDR_REG, DMEM + (SP_MEM_ADDR_REG & 0x1FFF),
		SP_WR_LEN_REG + 1);
	SP_DMA_BUSY_REG = 0;
	SP_STATUS_REG  &= ~SP_STATUS_DMA_BUSY;
}