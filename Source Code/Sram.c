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
static HANDLE hSRAMFile = NULL;
void CloseSRAM (void) {
	if (hSRAMFile) {
		CloseHandle(hSRAMFile);
		hSRAMFile = NULL;
	}
}
  BOOL LoadSRAM (void) {
  int i = 0;
  char File[255], Directory[255];
  LPVOID lpMsgBuf;
  GetAutoSaveDir(Directory);
  sprintf(File,"%s%s.sra",Directory,RomName);
  for(;File[i] != 0;i++)
  {
    if(File[i] == '*' || File[i]== ';')
      File[i] = '_';
  }
	hSRAMFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (hSRAMFile == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_PATH_NOT_FOUND:
			CreateDirectory(Directory,NULL);
			hSRAMFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			if (hSRAMFile == INVALID_HANDLE_VALUE) {
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf,0,NULL
				);
				DisplayError(lpMsgBuf);
				LocalFree( lpMsgBuf );
				return FALSE;
			}
			break;
		default:
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,0,NULL
			);
			DisplayError(lpMsgBuf);
			LocalFree( lpMsgBuf );
			return FALSE;
		}
	}
	return TRUE;
}
void DmaFromSRAM(BYTE * dest, int StartOffset, int len) {
	DWORD dwRead;
	if (hSRAMFile == NULL) {
		if (!LoadSRAM()) {
			return;
		}
	}
	SetFilePointer(hSRAMFile,StartOffset,NULL,FILE_BEGIN);
	ReadFile(hSRAMFile,dest,len,&dwRead,NULL);
}
void DmaToSRAM(BYTE * Source, int StartOffset, int len) {
    DWORD dwWritten;
    if (hSRAMFile == NULL) {
        if (!LoadSRAM()) {
            return;
        }
    }
    StartOffset = ((StartOffset >> 3) & 0xFFFF8000) | (StartOffset & 0x7FFF);
    if (((StartOffset & 3) == 0) && ((((size_t)Source) & 3) == 0)) {
        SetFilePointer(hSRAMFile, StartOffset, NULL, FILE_BEGIN);
        WriteFile(hSRAMFile, Source, len, &dwWritten, NULL);
    } else {
        for (int i = 0; i < len; i++) {
            SetFilePointer(hSRAMFile, (StartOffset + i) ^ 3, NULL, FILE_BEGIN);
            WriteFile(hSRAMFile, (BYTE *)(((size_t)Source + i) ^ 3), 1, &dwWritten, NULL);
        }
    }
}