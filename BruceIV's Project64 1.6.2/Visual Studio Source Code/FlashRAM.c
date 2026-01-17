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
#include <windows.h>
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
enum TFlashRAM_Modes {
	FlashRAM_MODE_NOPES=0,
	FlashRAM_MODE_ERASE=1,
	FlashRAM_MODE_WRITE,
	FlashRAM_MODE_READ,
	FlashRAM_MODE_STATUS,
};
BOOL LoadFlashRAM (void);
DWORD FlashRAM_Offset,FlashFlag=FlashRAM_MODE_NOPES;
static HANDLE hFlashRAMFile=NULL;
BYTE*FlashRAMPointer;
QWORD FlashStatus=0;
void DMAfromFlashRAM(BYTE*dest,int StartOffset,int len) {
	BYTE FlipBuffer[0x10000];
	DWORD dwRead,count;
	switch (FlashFlag) {
	case FlashRAM_MODE_READ:
		if (hFlashRAMFile==NULL) {
			if (!LoadFlashRAM()) return;
		}
		if (len>0x10000) {
			len=0x10000;
		}
		if ((len&3)!=0) {
			return;
		}
		memset(FlipBuffer,0,sizeof(FlipBuffer));
		StartOffset=StartOffset<<1;
		SetFilePointer(hFlashRAMFile,StartOffset,NULL,FILE_BEGIN);
		ReadFile(hFlashRAMFile,FlipBuffer,len,&dwRead,NULL);
		for (count=dwRead; (int)count<len; count++) {
			FlipBuffer[count]=0xFF;
		}
		_asm {
			mov edi,dest
			lea ecx,[FlipBuffer]
			mov edx,0
			mov ebx,len
		memcpyloop:
			mov eax,dword ptr [ecx+edx]
			;bswap eax
			mov  dword ptr [edi+edx],eax
			add edx,4
			cmp edx,ebx
			jb memcpyloop
		}
		break;
	case FlashRAM_MODE_STATUS:
		if (StartOffset!=0&&len!=8) {
		}
		*((DWORD*)(dest))=(DWORD)(FlashStatus>>32);
		*((DWORD*)(dest)+1)=(DWORD)(FlashStatus);
		break;
	}
}
void DMAtoFlashRAM(BYTE*Source,int StartOffset,int len) {
	switch (FlashFlag) {
	case FlashRAM_MODE_WRITE:
		FlashRAMPointer=Source;
		break;
	}
}
DWORD ReadFromFlashStatus () {
	return (DWORD)(FlashStatus>>32);
}
BOOL LoadFlashRAM (void) {
	char File[255],Directory[255];
	GetAutoSaveDir(Directory);
	sprintf(File,"%s%s.fla",Directory,RomName);
	hFlashRAMFile=CreateFile(File,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
	if (hFlashRAMFile==INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_PATH_NOT_FOUND:
			CreateDirectory(Directory,NULL);
			hFlashRAMFile=CreateFile(File,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
			if (hFlashRAMFile==INVALID_HANDLE_VALUE) {
				DisplayError(GS(MSG_FAIL_OPEN_SAVE));
				return FALSE;
			}
			break;
		default:
			DisplayError(GS(MSG_FAIL_OPEN_SAVE));
			return FALSE;
		}
	}
	return TRUE;
}
void WriteToFlashCommand(DWORD FlashRAM_Command) {
	BYTE EmptyBlock[128],FlipBuffer[128];
	DWORD dwWritten;
	switch (FlashRAM_Command&0xFF000000) {
	case 0xD2000000:
		switch (FlashFlag) {
		case FlashRAM_MODE_ERASE:
			memset(EmptyBlock,0xFF,sizeof(EmptyBlock));
			if (hFlashRAMFile==NULL) {
				if (!LoadFlashRAM()) return;
			}
			SetFilePointer(hFlashRAMFile,FlashRAM_Offset,NULL,FILE_BEGIN);
			WriteFile(hFlashRAMFile,EmptyBlock,128,&dwWritten,NULL);
		case FlashRAM_MODE_NOPES:
		case FlashRAM_MODE_READ:
		case FlashRAM_MODE_STATUS: break;
		case FlashRAM_MODE_WRITE:
			if (hFlashRAMFile==NULL) {
				if (!LoadFlashRAM()) return;
			}
			memset(FlipBuffer,0,sizeof(FlipBuffer));
			_asm {
				lea edi,[FlipBuffer]
				mov ecx,FlashRAMPointer
				mov edx,0
			memcpyloop:
				mov eax,dword ptr [ecx+edx]
				;bswap eax
				mov  dword ptr [edi+edx],eax
				add edx,4
				cmp edx,128
				jb memcpyloop
			}
			SetFilePointer(hFlashRAMFile,FlashRAM_Offset,NULL,FILE_BEGIN);
			WriteFile(hFlashRAMFile,FlipBuffer,128,&dwWritten,NULL);
			break;
		}
		FlashFlag=FlashRAM_MODE_NOPES;
		break;
	case 0xE1000000:
		FlashFlag=FlashRAM_MODE_STATUS;
		FlashStatus=0x1111800100C20000;
		break;
	case 0xF0000000:
		FlashFlag=FlashRAM_MODE_READ;
		FlashStatus=0x11118004F0000000;
		break;
	case 0x4B000000:
		FlashRAM_Offset=(FlashRAM_Command&0xffff)*128;
		break;
	case 0x78000000:
		FlashFlag=FlashRAM_MODE_ERASE;
		FlashStatus=0x1111800800C20000;
		break;
	case 0xB4000000:
		FlashFlag=FlashRAM_MODE_WRITE;
		break;
	case 0xA5000000:
		FlashRAM_Offset=(FlashRAM_Command&0xffff)*128;
		FlashStatus=0x1111800400C20000;
		break;
	}
}