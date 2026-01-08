#include <windows.h>
#include <stdio.h>
#include "Main.h" // for hinst
#include "Language.h"
#include "Resource.h"
/*******************************************************************************
* Definitions                                                                  *
*******************************************************************************/
#define MAX_LANGUAGES	45
#define MAX_LANNAME_LEN	100
#define MAX_STRINGS		300
#define MAX_STRING_LEN	400
typedef struct {
	int    ID;
	char   Str[MAX_STRING_LEN];
} LANG_STR;
LANG_STR DefaultString[]={
    { LANGUAGE_AUTHOR,    "BruceIV"			  },
    { LANGUAGE_VERSION,   "19"				  }, // Update to current source version once this gets changed again
    { LANGUAGE_DATE,      "2026 January"		  },
	{ INI_CURRENT_LANG,   "Language Database (.txt)" },
	{ INI_AUTHOR,         "Author"                   },
	{ INI_VERSION,        "Version"                  },
	{ INI_DATE,           "Date"                     },
	{ LIMITFPS_ON,	       "Limit FPS on"		  },
	{ INI_CURRENT_GAME_INI,    "ROM Database (Game.ini)"},
	{ INI_CURRENT_CHEAT_INI,    "Cheat Database (Cheat.ini)"},
	        { MENU_FILE,      "File"                    },
		{ MENU_OPEN,      "Open ROM"                },
		{ MENU_ROM_INFO,  "ROM Info"                },
		{ MENU_START,     "Start Emulation"         },
		{ MENU_END,       "End Emulation"           },
		{ MENU_CHOOSE_ROM,"Choose ROM Directory"    },
		{ MENU_REFRESH,   "Refresh ROM List"        },
		{ MENU_RECENT_ROM,"Recent ROM"              },
		{ MENU_RECENT_DIR,"Recent ROM Directories"  },
		{ MENU_EXIT,      "Exit"                    },
	        { MENU_SYSTEM,    "System"                  },
		{ MENU_RESET,      "Reset"                  },
		{ MENU_PAUSE,      "Pause"                  },
                { MENU_BITMAP,     "Screenshot"             },
		{ MENU_LIMIT_FPS,  "Limit FPS"              },
		{ MENU_SAVE,       "Save"                   },
		{ MENU_SAVE_AS,    "Save as"                },
		{ MENU_RESTORE,    "Restore"                },
		{ MENU_LOAD,       "Load from"              },
		{ MENU_CURRENT_SAVE,"Current Save State"     },
		{ MENU_CHEAT,      "Cheats"                 },
		{ MENU_GS_BUTTON,  "GS Button"              },
		{ MENU_RESUME,     "Resume"                 },
	        { MENU_OPTIONS, "Options"                   },
		{ MENU_FULL_SCREEN,"Full Screen"                 },
		{ MENU_ON_TOP,     "Usually on Top"              },
		{ MENU_CONFIG_GFX,  "Configure Graphics Plugin"  },
		{ MENU_CONFIG_AUDIO,"Configure Audio Plugin"     },
		{ MENU_CONFIG_CTRL, "Configure Controller Plugin"},
		{ SPEEDCAP,		  "2x Speed Cap"},
		{ MENU_SETTINGS,   "Settings"                    },
	        { MENU_LANGUAGE,"Language"                       },
	        { MENU_HELP,    "Help"                           },
		{ MENU_USER_GUIDE,   "User Guide"                },
		{ MENU_ABOUT_INI,  "About Databases"             },
		{ MENU_ADVANCE,		  "Frame Advance"},
		{ MENU_UNINSTALL,  "Factory Reset"               },
// Menu Descriptions
        { MENUDES_OPEN,"Lets you open N64 ROMs" },
        { MENUDES_ROM_INFO,"Displays ROM-specific information" },
        { MENUDES_START,"Starts emulation of the most recently opened ROM" },
        { MENUDES_END,"Ends emulation of the most recently opened ROM" },
        { MENUDES_CHOOSE_ROM,"Lets you choose a ROM directory" },
        { MENUDES_REFRESH,"Refreshes current list of ROMs in the ROM browser" },
        { MENUDES_EXIT,"Closes Project64" },
        { MENUDES_RESET,"Resets the current ROM" },
        { MENUDES_PAUSE,"Pauses/resumes emulation of the current ROM" },
        { MENUDES_BITMAP,"Takes a screenshot (plugin-dependent)" },
        { MENUDES_LIMIT_FPS,"Lets you toggle >1x speeds (plugin-dependent)" },
        { MENUDES_SAVE,"Saves last selected save state slot" },
        { MENUDES_SAVE_AS,"Lets you choose a directory to make a save state" },
        { MENUDES_RESTORE,"Loads last selected save state slot" },
        { MENUDES_LOAD,"Lets you choose a save state to load" },
        { MENUDES_CHEAT,"Lets you edit cheat codes for a selected ROM" },
        { MENUDES_GS_BUTTON,"Activates GS cheats when enabled in the cheat menu" },
        { MENUDES_FULL_SCREEN,"Enters current plugin's fullscreen mode (plugin-dependent)" },
        { MENUDES_ON_TOP,"Usually makes Project64 stay on top of all other windows" },
        { MENUDES_CONFIG_GFX,"Lets you change settings inside the Graphics plugin" },
        { MENUDES_CONFIG_AUDIO,"Lets you change settings inside the Audio plugin" },
        { MENUDES_CONFIG_CTRL,"Lets you change settings inside the Controller plugin" },
	{ SPEEDCAP_ON,		  "2x Speed Cap applied to Limit FPS off"},
        { MENUDES_SETTINGS,"Lets you change settings for Project64" },
        { MENUDES_USER_GUIDE,"Opens the user guide (always contains credits)" },
        { MENUDES_ABOUT_INI,"Displays metadata for Project64 databases" },
        { MENUDES_RECENT_ROM,"Lets you open a previously opened ROM path" },
        { MENUDES_RECENT_DIR,"Lets you choose a ROM directory" },
        { MENUDES_LANGUAGES,"Lets you swap to a different language database if provided" },
        { MENUDES_GAME_SLOT,"Selects a save state slot" },
        { MENUDES_UNINSTALLAPP,"Attempts to terminate this instance and factory reset included registry configurations" },
	{ MSG_LOADED_STATE,	"Loaded a save state" },
/*********************************************************************************
* ROM Browser                                                                    *
*********************************************************************************/
//ROM Browser Fields
	{ RB_FILENAME,    "File Name" },
	{ RB_INTERNALNAME,"Internal Name" },
	{ RB_GAMENAME,    "Game Name" },
	{ RB_STATUS,      "Status" },
	{ RB_ROMSIZE,     "Size" },
	{ RB_CART_ID,     "ID" },
	{ RB_CRC1,        "1st CRC" },
	{ RB_CRC2,        "2nd CRC" },
	{ RB_CICCHIP,     "CIC Chip" },
	{ SHANKLE_AZI_AI_LANG,"Azi AI"},
//Messages
	{ UNKNOWN,"Unknown" },
	{ FIFTYNINEHERTZ_LANG,"59 Hz"},
/*********************************************************************************
* Options                                                                        *
*********************************************************************************/
	{ TAB_PLUGIN,"Plugins"},
	{ TAB_DIRECTORY,"Directories"},
	{ TAB_ROMSELECTION,"ROM Selection"},
	{ TAB_ADVANCED,"Advanced"},
	{ TAB_ROMSETTINGS,"ROM Settings"},
	{ ROMSETTWARN,"Reset, end, or start emulation again to apply any changes"},
	{ TAB_ROMNOTES,"ROM Notes"},
//Plugin Dialog
	{ PLUG_ABOUT,"About"},
	{ PLUG_RSP,  " RSP (reality signal processor) plugin: "},
	{ PLUG_GFX,  " Video (graphics) plugin: "},
	{ PLUG_AUDIO," Audio (sound) plugin: "},
	{ PLUG_CTRL, " Input (controller) plugin: "},
	{ DIR_ROM_DEFAULT,  "Last folder of ROM Opening"},
	{ DIR_AUTO_SAVE,    "Save Data"},
	{ DIR_INSTANT_SAVE, "Save States"},
        { DIR_SCREEN_SHOT,  "Screenshots"},
	{ SCREENSHOT_TAKEN, "Screenshot taken"},
	{ DIR_SELECT_ROM,   "Select ROM Directory"},
	{ DIR_SELECT_AUTO,  "Select Save Data Directory"},
	{ DIR_SELECT_INSTANT,"Select Save States Directory"},
        { DIR_SELECT_SCREEN,"Select Screenshots Directory"},
//Options (general) Tab
	{ OPTION_AUTO_SLEEP,     "Pause CPU Upon Focus Loss"},
	{ OPTION_AUTO_FULLSCREEN,"Enter Fullscreen Mode Upon ROM Opening"},
	{ OPTION_BASIC_MODE,     "Hide Advanced Settings"},
	{ OPTION_AUTO_HIDE,	  "Always Hide Cursor in Fullscreen"},
	{ RB_MAX_ROMS,        "Max # of ROMs Remembered (Max 10):"},
	{ RB_ROMS,            "ROMs"},
	{ RB_MAX_DIRS,        "Max # of ROM Dirs Remembered (Max 10):"},
	{ RB_DIRS,            "Dirs"},
	{ SPEEDCAP_DESC,      "Caps Limit FPS off's maximum speed to 2x full speed" },
	{ RB_DIR_RECURSION,   "Directory Recursion"},
	{ RB_AVAILABLE_FIELDS,"Available fields:"},
	{ RB_SHOW_FIELDS,     "Show fields in this order:"},
	{ RB_ADD,             "Add ->"},
	{ RB_REMOVE,          "<- Remove"},
	{ SyncGametoAudio_LANG,"Sync Game to Audio" },
	{ N64_CRASH,		  "Nintendo 64 hardware would probably crash on the next frame at this location:" },
	{ ON,		  "ON" },
	{ DEFAULT_TEXT,   "Default"},
	{ FORCE_AUTO4kbit,	       "Always Autodetect With 16kbit EEPROM" },
	{ NUMBER_1,	       "1"},
	{ ROM_CPU_STYLE,      "CPU Core Style:"},
	{ ROM_SMCM,           "Self-modifying Code Method:"},
	{ ROM_MEM_SIZE,       "Memory Size:" },
	{ ROM_AUDIO_SIGNAL,            "Signal"},
	{ ROM_SAVE_TYPE,      "Save Type:"},
	{ ROM_COUNTER_FACTOR, "Counter Factor:"},
	{ ROM_JAI,   "Jabo AI"},
	{ ROM_USE_TLB,        "TLB"},
	{ REG_CACHE,      "Register Caching:"},
	{ ROM_DELAY_SI,       "Delay SI"},
	{ CF1_CF0,        "CF1-->0"},
	{ ALTERNATE_EMUAI_LANG,"Legacy AiLenChanged"},
	{ LIMITFPS_OFF,	       "Limit FPS off"},
	{ ROM_DELAY_RDP,      "Delay RDP"},
	{ ROM_DELAY_RSP,      "Delay RSP"},
	{ USUALLYONTOP_ON,	   "Usually on Top on"},
//Core Styles
	{ CORE_INTERPRETER,   "Interpreter"},
	{ CORE_RECOMPILER,    "Recompiler"},
//RDRAM Size
	{ RDRAM_4MB,          "4 MB" },
	{ RDRAM_8MB,          "8 MB" },
	{ EXTRA_PROJECT64,    "Project64 is already running.\n\nWould you like to force-kill the current Project64 before starting another one? Force-killing prevents save data issues but this may disrupt plugin behavior" },
	{ SMCM_CACHE,         "Cache"},
	{ SMCM_PROTECTED,     "Protect Memory"},
	{ FORCE_RSP_RECOMPILER,"RSP Recompiler"},
	{ USUALLYONTOP_OFF,	   "Usually on Top off" },
	{ SMCM_CHECK_ADV,     "Check Memory"},
	{ SPEEDCAP_OFF,	       "2x Speed Cap revoked from Limit FPS off" },
	{ FORCE_DISABLE_TLB,  "Always Disable TLB"},
	{ FORCE_DISABLE_REGISTERCACHING,"Always Disable Register Caching" },
	{ SAVE_4K_EEPROM,     "4kbit EEPROM"},
	{ SAVE_16K_EEPROM,    "16kbit EEPROM"},
	{ SAVE_SRAM,          "SRAM"},
	{ SAVE_FlashRAM,      "FlashRAM"},
//ROM Notes
	{ NOTE_STATUS,        "ROM Status:"},
	{ NOTE_CORE,          "Core Note:"},
	{ NOTE_PLUGIN,        "Plugin Note:"},
/*********************************************************************************
* ROM Information                                                                *
*********************************************************************************/
//ROM Info Title
	{ INFO_TITLE,            "ROM Information"},
	{ INFO_ROM_NAME_TEXT,    "ROM Name:"},
	{ INFO_FILE_NAME_TEXT,   "File Name:"},
	{ INFO_LOCATION_TEXT,    "Location:"},
	{ INFO_SIZE_TEXT,        "Size:"},
	{ INFO_CART_ID_TEXT,     "ID:"},
	{ INFO_CRC1_TEXT,        "1st CRC:"},
	{ INFO_CRC2_TEXT,        "2nd CRC:"},
	{ INFO_CIC_CHIP_TEXT,    "CIC Chip:"},
	{ ROM_ALIGN_DMA,		  "Align DMA"},
/*********************************************************************************
* Messages                                                                       *
*********************************************************************************/
	{ MSG_CPU_PAUSED,        "CPU paused"},
	{ MSG_CPU_RESUMED,       "CPU resumed"},
	{ MSG_MEM_ALLOC_ERROR,   "Failed to allocate memory"},
	{ MSG_FAIL_INIT_GFX,     "The default or selected video plugin is missing or invalid.\n\nYou need to select a video (graphics) plugin"},
	{ MSG_FAIL_INIT_AUDIO,   "The default or selected audio plugin is missing or invalid.\n\nYou need to select an audio (sound) plugin"},
	{ MSG_FAIL_INIT_RSP,     "The default or selected RSP plugin is missing or invalid.\n\nYou need to select a reality signal processor (RSP) plugin"},
	{ MSG_FAIL_INIT_CONTROL, "The default or selected input plugin is missing or invalid.\n\nYou need to select an input (controller) plugin"},
	{ MSG_FAIL_LOAD_PLUGIN,  "Failed to load plugin"},
	{ MENU_ADVANCE_DESC,	  "Advances through a game in single frames at a time (plugin-dependent)"},
	{ MSG_UNABLE_SAVE_STATE,    "Unable to save state"},
        { MSG_FAIL_OPEN_SAVE,   "Cannot access save data. You may be running the application from an admin rights directory"},
	{ MSG_FAIL_OPEN_ZIP,     "Attempt to open zip file failed.\n\nPotentially is a corrupt zip file - try unzipping ROM manually"},
	{ MSG_RB_INFO,		  "Displays ROM-specific information about a selected ROM"},
	{ OPENROM_FAILED,	  "Failed to open ROM.\n\nMake sure it exists in the location you tried to open it from"},
	{ MSG_FAIL_IMAGE,        "File loaded does not appear to be a valid ROM.\n\nVerify that your ROM's checksum is valid"},
	{ MSG_UNKNOWN_FILE_FORMAT,"Unknown file format"},
        { MSG_FAIL_OPEN_MEMPAK,  "Failed to open mempak"},
	{ MSG_ERROR_TITLE,      "App Error"},
	{ Compile_SD_Info,	  "This action will attempt to compile virtual SD card space back into AUTO0.\n\nYou will be able to save to or load from your virtual SD card again if this action is successful" },
	{ THREAD_EXIT,		  "ExitThread(0); will be called next because the app thinks something has gone wrong. So if things are okay,use Win+Shift+S/Print Screen of where you encountered this error and explain how it happened so it can be debugged" },
	{ MSG_EMULATION_STARTED, "Emulation started"},
        { MSG_UNABLE_LOAD_STATE,"Unable to load save state"},
        { MSG_EMULATION_ENDED,   "Emulation ended"},
        { MSG_CONFIRMATION_UNINSTALL,"This action will attempt to terminate this instance and factory reset included registry configurations"},
	{ MSG_SAVED_STATE,	  "Saved state"},
        { GS_PRESS,              "GS button pressed"},
	{ FPS_DISPLAY,           "Frames Per Second"},
	{ FORCE_ENABLE_DMA,	  "Always Enable Align DMA"},
	{ NUMBER_2,		  "2"},
	{ NUMBER_3,		  "3"},
	{ OFF,	  "OFF" },
	{ MENUDES_GAME_SETTINGS, "Lets you read the ROM's notes (plugin/core/status)"},
	{ MSG_EMULATION_RESET,	  "Emulation reset"},
	{ MSG_PLS_START,		  "You must start emulation to configure this plugin"},
	{ AllocateCompile_SD,	  "Manage SD Space"},
	{ AllocateCompile_SD_MENUDES,"Attempts to allocate or compile 254MB of space for virtual SD card emulation"},
	{ Allocate_SD_Info,	  "This action will attempt to Allocate 254MB of SD space on your hard drive in an SDCARD0 partition after decompiling AUTO0.\n\nYou will not be able to save to or load from your virtual SD card until you compile virtual SD card space back into AUTO0 by using this option again"},
	{ SPECIAL_BREAK,	  "The game may crash at the upcoming r4300i_SPECIAL_BREAK. Do you want to emulate this potential crash?"},
	{ OPTIONAL_CRASH,	  "Potential Upcoming Crash"},
};
class CLanguage  {
	void FindLangName  (int Index);
	void LoadStrings   (char * FileName);
	void SaveCurrentLang (char * String);
public:
	CLanguage();
	void CreateLangList (HMENU hMenu,int uPosition,int MenuID);
	char * GetString    (int StringID);
	void LoadLangList   (void);
	void LoadLanguage  (char * RegLocation);
	const char * LangName  (int index);
	int  GetNumberLang  (void);
	void SetCurrentLang (HMENU hMenu,int MenuIndx);
	int  SetMenuBase    (int MenuBase);
private:
	LANG_STR m_Strings[MAX_STRINGS];
	char m_filenames[MAX_LANGUAGES][_MAX_PATH];
	char m_LangName[MAX_LANGUAGES][MAX_LANNAME_LEN];
	char m_CurrentLangName[MAX_LANNAME_LEN];
	char m_RegKey[300];
	int m_NoOflangs;
	int m_NoOfStrings;
	int m_BaseMenuID;
};
/*******************************************************************************
* Variable                                                                    *
*******************************************************************************/
CLanguage lng;
/*******************************************************************************
* Code                                                                         *
*******************************************************************************/
CLanguage::CLanguage() {
	m_NoOflangs=0;
	m_NoOfStrings=0;
	strcpy(m_CurrentLangName,"");
}
int CLanguage::GetNumberLang(void) {
	return m_NoOflangs;
}
const char * CLanguage::LangName  (int index) {
	if (index>=MAX_LANGUAGES) { return NULL; }
	return m_LangName[index];
}
void CLanguage::LoadLanguage  (char * RegLocation) {
	strncpy(m_RegKey,RegLocation,sizeof(m_RegKey));
	LoadLangList();
	strcpy(m_CurrentLangName,"");
	HKEY hKeyResults=0;
	long lResult=RegOpenKeyEx(HKEY_CURRENT_USER,RegLocation,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Bytes=sizeof(m_CurrentLangName);
		lResult=RegQueryValueEx(hKeyResults,"Language",0,&Type,(LPBYTE)(m_CurrentLangName),&Bytes);
		if (lResult!=ERROR_SUCCESS) { strcpy(m_CurrentLangName,""); }
	}
	RegCloseKey(hKeyResults);
	bool Found=false;
	for (int count=0; count<m_NoOflangs; count++) {
		if (strcmp(m_LangName[count],m_CurrentLangName)==0) {
			LoadStrings(m_filenames[count]);
			Found=true;
			break;
		}
	}
	if (!Found) {
		strcpy(m_CurrentLangName,"English");
	}
}
void CLanguage::CreateLangList (HMENU hMenu,int uPosition,int MenuID) {
	LoadLangList();
	HMENU hSubMenu=CreateMenu();
	MENUITEMINFO menuinfo;
	char String[100];
	menuinfo.cbSize=sizeof(MENUITEMINFO);
	menuinfo.fMask=MIIM_TYPE|MIIM_ID;
	menuinfo.fType=MFT_STRING;
	menuinfo.dwTypeData=String;
	menuinfo.cch=sizeof(String);
	if (GetNumberLang()==0) {
		menuinfo.wID=MenuID;
		strcpy(String,"App English");
		InsertMenuItem(hSubMenu,0,TRUE,&menuinfo);
		CheckMenuItem(hSubMenu,MenuID,MF_BYCOMMAND|MFS_CHECKED);
		EnableMenuItem(hSubMenu,MenuID,MFS_DISABLED|MF_BYCOMMAND);
	}
	for (int count=0; count<GetNumberLang(); count ++) {
		menuinfo.wID=MenuID+count;
		strcpy(String,lng.LangName(count));
		InsertMenuItem(hSubMenu,0,TRUE,&menuinfo);
		if (strcmp(m_CurrentLangName,String)==0) {
			CheckMenuItem(hSubMenu,menuinfo.wID,MF_BYCOMMAND|MFS_CHECKED);
		}
	}
	ModifyMenu(hMenu,uPosition,MF_STRING|MF_POPUP|MF_BYPOSITION,(DWORD)hSubMenu,GS(MENU_LANGUAGE));
}
char * CLanguage::GetString (int StringID) {
	for (int count=0; count<m_NoOfStrings; count ++) {
		if (m_Strings[count].ID==StringID) { return m_Strings[count].Str; }
	}
	return NULL;
}
void CLanguage::LoadLangList (void) {
	char Directory[_MAX_PATH],SearchString[_MAX_PATH];
	{
		char path_buffer[_MAX_PATH],drive[_MAX_DRIVE],dir[_MAX_DIR];
		char fname[_MAX_FNAME],ext[_MAX_EXT];
		GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
		_splitpath(path_buffer,drive,dir,fname,ext);
		sprintf(Directory,"%s%sPJ64DB\\",drive,dir);
		sprintf(SearchString,"%s*.txt",Directory);
	}
	WIN32_FIND_DATA find_data;
	HANDLE   search_handle;
	search_handle=FindFirstFile(SearchString,&find_data);
	m_NoOflangs=0;
	if(search_handle!=INVALID_HANDLE_VALUE) {
		do {
			strcpy(m_filenames[m_NoOflangs],Directory);
			strcat(m_filenames[m_NoOflangs],find_data.cFileName);
			m_NoOflangs +=1;
		} while (FindNextFile(search_handle,&find_data)&&search_handle!=INVALID_HANDLE_VALUE);
		FindClose(search_handle);
	}
	for (int count=0; count<m_NoOflangs; count ++) { FindLangName(count); }
}
void CLanguage::LoadStrings  (char * FileName) {
	m_NoOfStrings=0;
	FILE *file=fopen(FileName,"rb");
	if (file==NULL) return;
	char  token=0;
	while(!feof(file)) {
		token=0;
		//Search for token #
		while(token!='#'&&!feof(file)) { fread(&token,1,1,file); }
		if(feof(file))continue;
		//get StringID after token
		fscanf(file,"%d",&m_Strings[m_NoOfStrings].ID);
		//Search for token #
		while(token!='#'&&!feof(file)) { fread(&token,1,1,file); }
		if(feof(file))continue;
		//Search for start of string '"'
		while(token!='"'&&!feof(file)) { fread(&token,1,1,file); }
		if(feof(file))continue;
		int pos=0;
		fread(&token,1,1,file);
		while(token!='"'&&!feof(file)) {
			m_Strings[m_NoOfStrings].Str[pos++]=token;
			fread(&token,1,1,file);
			if (pos==MAX_STRING_LEN-2) { token='"'; }
		}
		m_Strings[m_NoOfStrings].Str[pos++]=0;
		m_NoOfStrings +=1;
		if (m_NoOfStrings==MAX_STRINGS) { break; }
	}
	fclose(file);
}
void CLanguage::FindLangName  (int Index) {
	strcpy(m_LangName[Index],GS(UNKNOWN));
	FILE *file=fopen(m_filenames[Index],"rb");
	if (file==NULL) return;
	char  token=0;
	int   StringID;
	while(!feof(file)) {
		token=0;
		//Search for token #
		while(token!='#'&&!feof(file)) { fread(&token,1,1,file); }
		if(feof(file))continue;
		//get StringID after token
		fscanf(file,"%d",&StringID);
		//Search for token #
		while(token!='#'&&!feof(file)) { fread(&token,1,1,file); }
		if(feof(file))continue;
		if (StringID!=LANGUAGE_NAME) continue;
		//Search for start of string '"'
		while(token!='"'&&!feof(file)) { fread(&token,1,1,file); }
		if(feof(file))continue;
		int pos=0;
		fread(&token,1,1,file);
		while(token!='"'&&!feof(file)) {
			m_LangName[Index][pos++]=token;
			fread(&token,1,1,file);
			if (pos==sizeof(m_LangName[Index])-2) { token='"'; }
		}
		m_LangName[Index][pos++]=0;
	}
	fclose(file);
}
void CLanguage::SaveCurrentLang (char * String) {
	long lResult;
	HKEY hKeyResults=0;
	DWORD Disposition=0;
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER,m_RegKey,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult==ERROR_SUCCESS) {
		RegSetValueEx(hKeyResults,"Language",0,REG_SZ,(CONST BYTE *)String,strlen(String));
		strcpy(m_CurrentLangName,String);
	}
	RegCloseKey(hKeyResults);
	for (int count=0; count<m_NoOflangs; count++) {
		if (strcmp(m_LangName[count],m_CurrentLangName)==0) {
			LoadStrings(m_filenames[count]);
			break;
		}
	}
}
void CLanguage::SetCurrentLang (HMENU hMenu,int MenuIndx) {
	MENUITEMINFO menuinfo;
	char String[MAX_LANNAME_LEN];
	menuinfo.cbSize=sizeof(MENUITEMINFO);
	menuinfo.fMask=MIIM_TYPE;
	menuinfo.fType=MFT_STRING;
	menuinfo.dwTypeData=String;
	menuinfo.cch=sizeof(String);
	GetMenuItemInfo(hMenu,MenuIndx,FALSE,&menuinfo);
	SaveCurrentLang(String);
}
char * GS (int StringID) {
	int count;
	char * Ret=lng.GetString(StringID);
	if (Ret!=NULL) { return Ret; }
	for (count=0; count<(sizeof(DefaultString) / sizeof(LANG_STR)); count ++) {
		if (DefaultString[count].ID==StringID) { return DefaultString[count].Str; }
	}
	return "";
}
void CreateLangList (HMENU hMenu,int uPosition,int MenuID) {
	lng.CreateLangList(hMenu,uPosition,MenuID);
}
void LoadLanguage (char * RegLocation) {
	lng.LoadLanguage(RegLocation);
}
void SelectLangMenuItem (HMENU hMenu,int LangMenuID) {
	lng.SetCurrentLang(hMenu,LangMenuID);
}