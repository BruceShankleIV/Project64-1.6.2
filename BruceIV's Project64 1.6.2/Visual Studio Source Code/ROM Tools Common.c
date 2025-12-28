#include <Windows.h>
#include <stdio.h>
#include "ROM Tools Common.h"
int RomRegion (BYTE Country) {
	switch(Country) {
		case 0x44: // Germany
		case 0x46: // French
		case 0x49: // Italian
		case 0x50: // Europe
		case 0x53: // Spanish
		case 0x55: // Australia
		case 0x58: // X (PAL)
		case 0x59: // Y (PAL)
		return PAL_Region;
	}
	return 1;
}