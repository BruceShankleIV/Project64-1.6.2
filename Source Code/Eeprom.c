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
#include "main.h"
#include "CPU.h"
static HANDLE heepROMFile = NULL;
BYTE eepROM[0x800];
void CloseeepROM (void) {
	if (heepROMFile) {
		CloseHandle(heepROMFile);
		heepROMFile = NULL;
	}
}
void eepROMCommand ( BYTE * Command) {
	if (SaveUsing == Auto) {
		if (ForceAuto16) SaveUsing = eepROM_16K;
		else SaveUsing = eepROM_4K;
	}
	switch (Command[2]) {
	case 0: // check
		if (SaveUsing != eepROM_4K &&  SaveUsing != eepROM_16K) {
			Command[1] |= 0x80;
			break;
		}
		if (Command[1] != 3) {
			Command[1] |= 0x40;
			if ((Command[1] & 3) > 0) { Command[3] = 0x00; }
			if (SaveUsing == eepROM_4K) {
				if ((Command[1] & 3) > 1) { Command[4] = 0x80; }
			} else {
				if ((Command[1] & 3) > 1) { Command[4] = 0xC0; }
			}
			if ((Command[1] & 3) > 2) { Command[5] = 0x00; }
		} else {
			Command[3] = 0x00;
			Command[4] = SaveUsing == eepROM_4K?0x80:0xC0;
			Command[5] = 0x00;
		}
		break;
	case 4: // Read from eepROM
		ReadFromeepROM(&Command[4],Command[3]);
		break;
	case 5:
		WriteToeepROM(&Command[4],Command[3]);
		break;
	}
}
void LoadeepROM (void) {
	char File[255], Directory[255];
	DWORD dwRead;
	GetAutoSaveDir(Directory);
	sprintf(File,"%s%s.eep",Directory,RomName);
	heepROMFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (heepROMFile == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_PATH_NOT_FOUND:
			CreateDirectory(Directory,NULL);
			heepROMFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			if (heepROMFile == INVALID_HANDLE_VALUE) {
				DisplayError(GS(MSG_FAIL_OPEN_FLASH));
			}
			return;
			break;
		default:
			DisplayError(GS(MSG_FAIL_OPEN_FLASH));
			return;
		}
	}
	memset(eepROM,0,sizeof(eepROM));
	SetFilePointer(heepROMFile,0,NULL,FILE_BEGIN);
	ReadFile(heepROMFile,eepROM,sizeof(eepROM),&dwRead,NULL);
}
void ReadFromeepROM(BYTE * Buffer, int line) {
	int i;
	if (heepROMFile == NULL) {
		LoadeepROM();
	}
	for(i=0;i<8;i++) { Buffer[i]=eepROM[line*8+i]; }
}
void WriteToeepROM(BYTE * Buffer, int line) {
	DWORD dwWritten;
	int i;
	if (heepROMFile == NULL) {
		LoadeepROM();
	}
	for(i=0;i<8;i++) { eepROM[line*8+i]=Buffer[i]; }
	SetFilePointer(heepROMFile,line*8,NULL,FILE_BEGIN);
	WriteFile( heepROMFile,Buffer,8,&dwWritten,NULL );
}