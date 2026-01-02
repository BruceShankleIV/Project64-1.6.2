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
#include <windows.h>
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
static HANDLE hEEPROMFile = NULL;
BYTE EEPROM[0x800];
void CloseEEPROM (void) {
	if (hEEPROMFile) {
		CloseHandle(hEEPROMFile);
		hEEPROMFile = NULL;
	}
}
void EEPROMCommand (BYTE * Command) {
	if (SaveUsing == Auto) {
		if (ForceAuto16kbit) SaveUsing = EEPROM_16K;
		else SaveUsing = EEPROM_4K;
	}
	switch (Command[2]) {
	case 0: // check
		if (SaveUsing != EEPROM_4K &&  SaveUsing != EEPROM_16K) {
			Command[1] |= 0x80;
			break;
		}
		if (Command[1] != 3) {
			Command[1] |= 0x40;
			if ((Command[1] & 3) > 0) { Command[3] = 0x00; }
			if (SaveUsing == EEPROM_4K) {
				if ((Command[1] & 3) > 1) { Command[4] = 0x80; }
			} else {
				if ((Command[1] & 3) > 1) { Command[4] = 0xC0; }
			}
			if ((Command[1] & 3) > 2) { Command[5] = 0x00; }
		} else {
			Command[3] = 0x00;
			Command[4] = SaveUsing == EEPROM_4K?0x80:0xC0;
			Command[5] = 0x00;
		}
		break;
	case 4: // Read from EEPROM
		ReadFromEEPROM(&Command[4],Command[3]);
		break;
	case 5:
		WriteToEEPROM(&Command[4],Command[3]);
		break;
	}
}
void LoadEEPROM (void) {
	char File[255],Directory[255];
	DWORD dwRead;
	GetAutoSaveDir(Directory);
	sprintf(File,"%s%s.eep",Directory,RomName);
	hEEPROMFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,NULL);
	if (hEEPROMFile == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_PATH_NOT_FOUND:
			CreateDirectory(Directory,NULL);
			hEEPROMFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,NULL);
			if (hEEPROMFile == INVALID_HANDLE_VALUE) {
				DisplayError(GS(MSG_FAIL_OPEN_SAVE));
			}
			return;
		default:
			DisplayError(GS(MSG_FAIL_OPEN_SAVE));
			return;
		}
	}
	memset(EEPROM,0,sizeof(EEPROM));
	SetFilePointer(hEEPROMFile,0,NULL,FILE_BEGIN);
	ReadFile(hEEPROMFile,EEPROM,sizeof(EEPROM),&dwRead,NULL);
}
void ReadFromEEPROM(BYTE * Buffer,int line) {
	int i;
	if (hEEPROMFile == NULL) LoadEEPROM();
	for(i=0;i<8;i++) { Buffer[i]=EEPROM[line*8+i]; }
}
void WriteToEEPROM(BYTE * Buffer,int line) {
	DWORD dwWritten;
	int i;
	if (hEEPROMFile == NULL) LoadEEPROM();
	for(i=0;i<8;i++) { EEPROM[line*8+i]=Buffer[i]; }
	SetFilePointer(hEEPROMFile,line*8,NULL,FILE_BEGIN);
	WriteFile(hEEPROMFile,Buffer,8,&dwWritten,NULL);
}