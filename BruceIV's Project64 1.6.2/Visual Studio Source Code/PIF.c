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
#include "Plugin.h"
#include "N64 CIC-NUS-6105.h"
#include "ROM Tools Common.h"
#include "Real-Time Clock.h"
void ProcessControllerCommand (int Control,BYTE * Command);
void ReadControllerCommand (int Control,BYTE * Command);
BYTE PifRom[0x7C0],*PIF_Ram;
int GetCicChipID (char * RomData) {
	_int64 CRC=0;
	int count;
	for (count=0x40; count<0x1000; count +=4) {
		CRC +=*(DWORD *)(RomData+count);
	}
	switch (CRC) {
	case 0x000000D0027FDF31:
	case 0x000000CFFB631223: return 1;
	case 0x000000D057C85244: return 2;
	case 0x000000D6497E414B: return 3;
	case 0x000000EC932A00ED: return 4;
	case 0x0000011A49F60E96: return 5;
	case 0x000000D6D5BE5580: return 6;
	case 0x000001053BC19870: return 7;
	case 0x000000D2E53EF008: // N64DD IPL (J)
	case 0x000000D2E53EF39F: return 8;
	case 0x000000D2E53E5DDA: return 9;
	// TODO: More CIC Chip support
	}
	return -1;
}
void PifRamRead (void) {
	int Channel,CurPos;
	Channel=0;
	CurPos=0;
	if (PIF_Ram[0x3F]==0x2) {
		char hold[32],resp[32]={0};
		int i,j;
		for (i=0,j=48; i<32; i +=2,j++)
		{
			hold[i+1]=PIF_Ram[j] % 16;
			hold[i]=PIF_Ram[j] / 16;
		}
		N64_CIC_NUS_6105(hold,resp,32-2);
		for (i=48,j=0; i <=63; i++,j +=2) PIF_Ram[i]=resp[j] * 16+resp[j+1];
		return;
	}
	do {
		switch(PIF_Ram[CurPos]) {
		case 0x00:
			Channel +=1;
			if (Channel>6) { CurPos=0x40; }
			break;
		case 0xFE: CurPos=0x40;
		case 0xFF:
		case 0xB4: case 0x56: case 0xB8: /* ??? */
		break;
		default:
			if ((PIF_Ram[CurPos]&0xC0)==0) {
				if (Channel<4) {
					if (Controllers[Channel].Present&&Controllers[Channel].RawData) {
						if (ReadController) { ReadController(Channel,&PIF_Ram[CurPos]); }
					} else {
						ReadControllerCommand(Channel,&PIF_Ram[CurPos]);
					}
				}
				CurPos +=PIF_Ram[CurPos]+(PIF_Ram[CurPos+1]&0x3F)+1;
				Channel +=1;
			} else {
				CurPos=0x40;
			}
		}
		CurPos +=1;
	} while(CurPos<0x40);
	if (ReadController) { ReadController(-1,NULL); }
}
void PifRamWrite (void) {
	int Channel,CurPos;
	Channel=0;
	if (PIF_Ram[0x3F]>0x1) {
		switch (PIF_Ram[0x3F]) {
		case 0x08:
			PIF_Ram[0x3F]=0;
			MI_INTR_REG|=MI_INTR_SI;
			SI_STATUS_REG|=SI_STATUS_INTERRUPT;
			CheckInterrupts();
			break;
		case 0x10:
			memset(PifRom,0,0x7C0); break;
		case 0x30:
			PIF_Ram[0x3F]=0x80; break;
		case 0xC0:
			memset(PIF_Ram,0,0x40);
		}
		return;
	}
	for (CurPos=0; CurPos<0x40; CurPos++) {
		switch(PIF_Ram[CurPos])	{
		case 0x00:
			Channel +=1;
			if (Channel>6) { CurPos=0x40; }
			break;
		case 0xFE: CurPos=0x40;
		case 0xFF:
		case 0xB4: case 0x56: case 0xB8: /* ??? */
		break;
		default:
			if ((PIF_Ram[CurPos]&0xC0)==0) {
				if (Channel<4) {
					if (Controllers[Channel].Present&&Controllers[Channel].RawData) {
						if (ControllerCommand) { ControllerCommand(Channel,&PIF_Ram[CurPos]); }
					} else {
						ProcessControllerCommand(Channel,&PIF_Ram[CurPos]);
					}
				} else if (Channel==4) {
					if (RTC_Command(&PIF_Ram[CurPos])==FALSE) EEPROMCommand(&PIF_Ram[CurPos]);
				}
				CurPos +=PIF_Ram[CurPos]+(PIF_Ram[CurPos+1]&0x3F)+1;
				Channel +=1;
			} else {
				CurPos=0x40;
			}
		}
	}
	PIF_Ram[0x3F]=0;
	if (ControllerCommand) { ControllerCommand(-1,NULL); }
}
void ProcessControllerCommand (int Control,BYTE * Command) {
	switch (Command[2]) {
	case 0x00: // check
	case 0xFF: // reset&check ?
		if ((Command[1]&0x80)!=0) { break; }
		if (Controllers[Control].Present==TRUE) {
			Command[3]=0x05;
			Command[4]=0x00;
			switch (Controllers[Control].Plugin) {
			case PLUGIN_RUMBLE_PAK:
			case PLUGIN_MEMPAK:
			case PLUGIN_RAW: Command[5]=1; break;
			default: Command[5]=0; break;
			}
		} else {
			Command[1]|=0x80;
		}
		break;
	case 0x01: // read controller
		if (Controllers[Control].Present==FALSE) {
			Command[1]|=0x80;
		}
		break;
	case 0x02: //read from controller pack
		if (Controllers[Control].Present==TRUE) {
			DWORD address=((Command[3]<<8)|Command[4]);
			switch (Controllers[Control].Plugin) {
			case PLUGIN_RUMBLE_PAK:
				memset(&Command[5],(address>=0x8000&&address<0x9000)?0x80:0x00,0x20);
				Command[0x25]=Mempacks_CalulateCrc(&Command[5]);
				break;
			case PLUGIN_MEMPAK: ReadFromMempak(Control,address,&Command[5]); break;
			case PLUGIN_RAW: if (ControllerCommand) { ControllerCommand(Control,Command); } break;
			default:
				memset(&Command[5],0,0x20);
				Command[0x25]=0;
			}
		} else {
			Command[1]|=0x80;
		}
		break;
	case 0x03: //write controller pak
		if (Controllers[Control].Present==TRUE) {
			DWORD address=((Command[3]<<8)|Command[4]);
			switch (Controllers[Control].Plugin) {
			case PLUGIN_MEMPAK: WriteToMempak(Control,address,&Command[5]); break;
			case PLUGIN_RAW: if (ControllerCommand) { ControllerCommand(Control,Command); } break;
			case PLUGIN_RUMBLE_PAK:
				if (RumbleCommand!=NULL) {
					RumbleCommand(Control,*(BOOL *)(&Command[5]));
					break;
				}
			default:
				Command[0x25]=Mempacks_CalulateCrc(&Command[5]);
			}
		} else {
			Command[1]|=0x80;
		}
	}
}
void ReadControllerCommand (int Control,BYTE * Command) {
	switch (Command[2]) {
	case 0x01: // read controller
		if (Controllers[Control].Present==TRUE) {
			if (GetKeys) {
				BUTTONS Keys;
				GetKeys(Control,&Keys);
				*(DWORD *)&Command[3]=Keys.Value;
			} else {
				*(DWORD *)&Command[3]=0;
			}
		}
		break;
	case 0x02: //read from controller pack
		if (Controllers[Control].Present==TRUE) {
			switch (Controllers[Control].Plugin) {
			case PLUGIN_RAW: if (ControllerCommand) { ReadController(Control,Command); }
			}
		}
		break;
	case 0x03: //write controller pak
		if (Controllers[Control].Present==TRUE) {
			switch (Controllers[Control].Plugin) {
			case PLUGIN_RAW: if (ControllerCommand) { ReadController(Control,Command); }
			}
		}
	}
}
int LoadPifRom(int country) {
	char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT],PifRomName[255];
	HANDLE hPifFile;
	DWORD dwRead;
	GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
	_splitpath(path_buffer,drive,dir,fname,ext);
	switch(RomRegion(country)) {
	case PAL_Region:
		sprintf(PifRomName,"%s%sPIFROM\\PAL.raw",drive,dir); break;
	default:
		sprintf(PifRomName,"%s%sPIFROM\\NTSC.raw",drive,dir);
	}
	hPifFile=CreateFile(PifRomName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
	if (hPifFile==INVALID_HANDLE_VALUE) {
		memset(PifRom,0,0x7C0);
		return FALSE;
	}
	SetFilePointer(hPifFile,0,NULL,FILE_BEGIN);
	ReadFile(hPifFile,PifRom,0x7C0,&dwRead,NULL);
	CloseHandle(hPifFile);
	return TRUE;
}