#include <windows.h>
#include <stdio.h>
#include "Main.h" // for hinst
#include "Language.h"
#include "resource.h"
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
LANG_STR DefaultString[] = {
    { LANGUAGE_AUTHOR,     "Bruce IV"			  },
    { LANGUAGE_VERSION,    "1.6.2 Canary"                 },
    { LANGUAGE_DATE,       "February 2025"                },
	{ INI_CURRENT_LANG,    "Language Database (.LDB)" },
	{ INI_AUTHOR,          "Author"                   },
	{ INI_VERSION,         "Version"                  },
	{ INI_DATE,            "Date"                     },
	{ LIMITFPS_ON,	       "Limit FPS on"		  },
	{ INI_CURRENT_RDB,     "ROM Database (.RDB)"      },
	{ INI_CURRENT_CHT,     "Cheat Database (.CDB)"    },
	        { MENU_FILE,       "File"                    },
		{ MENU_OPEN,       "Open ROM"                },
		{ MENU_ROM_INFO,   "ROM Info"                },
		{ MENU_START,      "Start Emulation"         },
		{ MENU_END,        "End Emulation"           },
		{ MENU_CHOOSE_ROM, "Choose ROM Directory"    },
		{ MENU_REFRESH,    "Refresh ROM List"        },
		{ MENU_RECENT_ROM, "Recent ROM"              },
		{ MENU_RECENT_DIR, "Recent ROM Directories"  },
		{ MENU_EXIT,       "Exit"                    },
	        { MENU_SYSTEM,     "System"                  },
		{ MENU_RESET,       "Reset"                  },
		{ MENU_PAUSE,       "Pause"                  },
                { MENU_BITMAP,      "Screenshot"             },
		{ MENU_LIMIT_FPS,   "Limit FPS"              },
		{ MENU_SAVE,        "Save"                   },
		{ MENU_SAVE_AS,     "Save as"                },
		{ MENU_RESTORE,     "Restore"                },
		{ MENU_LOAD,        "Load from"              },
		{ MENU_CURRENT_SAVE,"Current Save State"     },
		{ MENU_CHEAT,       "Cheats"                 },
		{ MENU_GS_BUTTON,   "GS Button"              },
		{ MENU_RESUME,      "Resume"                 },
	        { MENU_OPTIONS,  "Options"                   },
		{ MENU_FULL_SCREEN, "Full Screen"                 },
		{ MENU_ON_TOP,      "Always On Top"               },
		{ MENU_CONFIG_GFX,   "Configure Graphics Plugin"  },
		{ MENU_CONFIG_AUDIO, "Configure Audio Plugin"     },
		{ MENU_CONFIG_CTRL,  "Configure Controller Plugin"},
		{ MENU_CONFIG_RSP,   "Configure RSP Plugin"       },
		{ MENU_SETTINGS,    "Settings"                    },
	        { MENU_LANGUAGE, "Language"                       },
	        { MENU_HELP,     "Help"                           },
		{ MENU_USER_GUIDE,    "User Guide"                },
		{ MENU_ABOUT_INI,   "About Databases"             },
		{ MENU_ADVANCE,		  "Frame Advance"},
		{ MENU_UNINSTALL,   "Factory Reset"               },
// Menu Descriptions
        { MENUDES_OPEN, "Lets you open N64 ROMs" },
        { MENUDES_ROM_INFO, "Displays ROM-specific information about a running ROM" },
        { MENUDES_START, "Starts emulation of the most recently opened ROM" },
        { MENUDES_END, "Ends emulation of the most recently opened ROM" },
        { MENUDES_CHOOSE_ROM, "Lets you choose a ROM directory (ROMs folder by default)" },
        { MENUDES_REFRESH, "Refreshes current list of ROMs in the ROM browser" },
        { MENUDES_EXIT, "Closes Project64" },
        { MENUDES_RESET, "Resets the current ROM" },
        { MENUDES_PAUSE, "Pauses/resumes emulation of the current ROM" },
        { MENUDES_BITMAP, "Attempts to screenshot (plugin-dependent, freezes on D3D8 1.7 plugins w/FSAA)" },
        { MENUDES_LIMIT_FPS, "Lets you toggle >100% speeds without Sync game to Audio" },
        { MENUDES_SAVE, "Saves last selected save state slot" },
        { MENUDES_SAVE_AS, "Lets you choose a directory to make a save state" },
        { MENUDES_RESTORE, "Loads last selected save state slot" },
        { MENUDES_LOAD, "Lets you choose a save state to load" },
        { MENUDES_CHEAT, "Enables/disables GameShark cheats" },
        { MENUDES_GS_BUTTON, "Activates GS cheats when enabled in the cheat menu" },
        { MENUDES_FULL_SCREEN, "Enters current plugin's fullscreen mode (problems with D3D8)" },
        { MENUDES_ON_TOP, "Makes Project64 stay in front of all other windows" },
        { MENUDES_CONFIG_GFX, "Lets you change settings inside the Graphics plugin" },
        { MENUDES_CONFIG_AUDIO, "Lets you change settings inside the Audio plugin" },
        { MENUDES_CONFIG_CTRL, "Lets you change settings inside the Controller plugin" },
        { MENUDES_CONFIG_RSP, "Lets you change settings inside the RSP plugin" },
        { MENUDES_SETTINGS, "Lets you change settings for Project64" },
        { MENUDES_USER_GUIDE, "Opens the user guide" },
        { MENUDES_ABOUT_INI, "Displays metadata for Project64 databases" },
        { MENUDES_RECENT_ROM, "Lets you open a previously opened ROM path" },
        { MENUDES_RECENT_DIR, "Lets you choose a ROM directory" },
        { MENUDES_LANGUAGES, "Lets you swap to a different language database if provided" },
        { MENUDES_GAME_SLOT, "Selects a save state slot" },
        { MENUDES_GAME_SETTINGS, "Lets you edit ROM settings for a selected ROM" },
        { MENUDES_GAME_CHEATS, "Lets you edit cheat codes for a selected ROM (available when remembered)" },
        { MENUDES_UNINSTALLAPP, "Attempts to terminate this instance and factory reset included registry configurations" },
	{ FULLSCREEN_EXITED, "Fullscreen mode exited"},
/*********************************************************************************
* ROM Browser                                                                    *
*********************************************************************************/
//ROM Browser Fields
	{ RB_FILENAME,     "File Name" },
	{ RB_INTERNALNAME, "Internal Name" },
	{ RB_GAMENAME,     "Game Name" },
	{ RB_STATUS,       "Status" },
	{ RB_ROMSIZE,      "Size" },
	{ RB_NOTES_CORE,   "Core Note" },
	{ RB_NOTES_PLUGIN, "Plugin Note" },
	{ RB_CART_ID,      "ID" },
	{ RB_CRC1,         "1st CRC" },
	{ RB_CRC2,         "2nd CRC" },
	{ RB_CICCHIP,      "CIC Chip" },
	{ SELECT_ROM_DIR,  "Choose your ROM directory" },
//Messages
	{ RB_NOT_IN_RDB,"Unknown" },
        { RB_HACK, "Unknown (Hack/Prototype/Public Domain?)" },
/*********************************************************************************
* Options                                                                        *
*********************************************************************************/
//Tabs
	{ TAB_PLUGIN,"Plugins"},
	{ TAB_DIRECTORY,"Directories"},
	{ TAB_ROMSELECTION,"ROM Selection"},
	{ TAB_ADVANCED,"Advanced"},
	{ TAB_ROMSETTINGS,"ROM Settings"},
	{ TAB_SHELLINTERGATION,"Shell Integration"},
	{ TAB_ROMNOTES,"ROM Notes"},
//Plugin Dialog
	{ PLUG_ABOUT, "About"},
	{ PLUG_RSP,   " Reality Signal Processor plugin: "},
	{ PLUG_GFX,   " Video (graphics) plugin: "},
	{ PLUG_AUDIO, " Audio (sound) plugin: "},
	{ PLUG_CTRL,  " Input (controller) plugin: "},

	{ DIR_ROM_DEFAULT,   "Last folder of ROM Opening"},
	{ DIR_AUTO_SAVE,     "Save Data"},
	{ DIR_INSTANT_SAVE,  "Save States"},
        { DIR_SCREEN_SHOT,   "Screenshots"},
	{ SCREENSHOT_TAKEN,  "Screenshotting attempted"},
	{ DIR_SELECT_ROM,    "Select ROM Directory"},
	{ DIR_SELECT_AUTO,   "Select Save Data Directory"},
	{ DIR_SELECT_INSTANT,"Select Save States Directory"},
        { DIR_SELECT_SCREEN, "Select Screenshots Directory"},
//Options (general) Tab
	{ OPTION_AUTO_SLEEP,      "Pause CPU Upon Focus Loss"},
	{ OPTION_AUTO_FULLSCREEN, "Enter Fullscreen Mode Upon ROM Opening"},
	{ OPTION_BASIC_MODE,      "Hide Advanced Settings"},
	{ OPTION_REMEMBER_CHEAT,  "Remember Selected Cheats"},
//ROM Browser Tab
	{ RB_MAX_ROMS,         "Max # of ROMs Remembered (Max 10):"},
	{ RB_ROMS,             "ROMs"},
	{ RB_MAX_DIRS,         "Max # of ROM Dirs Remembered (Max 10):"},
	{ RB_DIRS,             "Dirs"},
	{ RB_USE,              "ROM Browser"},
	{ RB_DIR_RECURSION,    "Directory Recursion"},
	{ RB_AVAILABLE_FIELDS, "Available fields:"},
	{ RB_SHOW_FIELDS,      "Show fields in this order:"},
	{ RB_ADD,              "Add ->"},
	{ RB_REMOVE,           "<- Remove"},
	{ RB_UP,               "Up"},
	{ RB_DOWN,             "Down"},

	{ ADVANCE_INFO,        "Note Interpreter is not affected by the following settings:\nSCM, ABL, Larger Compile Buffer, and Register Caching"},
	{ ADVANCE_DEFAULTS,    "Default"},
	{ USERDB,	       "Use RDB"},
	{ NUMBER_1,	       "1 (Lagless, Unstable)"},
	{ ROM_CPU_STYLE,       "CPU Core Style:"},
	{ ROM_SMCM,            "Self-modifying Code Method:"},
	{ ROM_MEM_SIZE,        "Memory Size:" },
	{ ROM_ABL,             "ABL"},
	{ ROM_SAVE_TYPE,       "Save Type:"},
	{ ROM_COUNTER_FACTOR,  "Counter Factor:"},
	{ ROM_LARGE_BUFFER,    "Larger Compile Buffer"},
	{ ROM_USE_TLB,         "TLB"},
	{ ROM_REG_CACHE,       "Register Caching"},
	{ ROM_DELAY_SI,        "Delay SI"},
	{ ROM_SP_HACK,         "SP Hack"},
	{ ROM_SAVE,	       "Autodetected"},
	{ LIMITFPS_OFF,	       "Limit FPS off"},
	{ ROM_DELAY_RDP,       "Delay RDP"},
	{ ROM_DELAY_RSP,       "Delay RSP"},
	{ MENU_SLOW,	       "LLE Slow Mode (Experimental)"},
//Core Styles
	{ CORE_INTERPRETER,    "Interpreter"},
	{ CORE_RECOMPILER,     "Recompiler"},
//RDRAM Size
	{ RDRAM_4MB,           "4 MB" },
	{ RDRAM_8MB,           "8 MB" },
//Self Mod Methods
	{ SMCM_NONE,           "None"},
	{ SMCM_CACHE,          "Cache"},
	{ SMCM_PROTECTED,      "Protect Memory"},
	{ SMCM_CHECK_MEM,      "Check Memory & Cache"},
	{ SMCM_CHANGE_MEM,     "Change Memory & Cache"},
	{ SMCM_CHECK_ADV,      "Check Memory Advance"},

	{ MENUDES_SLOW,	       "Attempts to terminate this instance and set various slow registry configurations"},
	{ REGEDIT_MENUDES,     "Opens the registry editor to view/edit/export registries"},
	{ SMCM_CHECK_RETURN,   "Check Memory & Return"},
	{ SAVE_4K_eepROM,      "4kbit eepROM"},
	{ SAVE_16K_eepROM,     "16kbit eepROM"},
	{ SAVE_SRAM,           "32kbytes SRAM"},
	{ SAVE_FlashRAM,       "FlashRAM"},
//Shell Intergration Tab
	{ SHELL_TEXT,          "File extension association:"},
//ROM Notes
	{ NOTE_STATUS,         "ROM Status:"},
	{ NOTE_CORE,           "Core Note:"},
	{ NOTE_PLUGIN,         "Plugin Note:"},
/*********************************************************************************
* ROM Information                                                                *
*********************************************************************************/
//ROM Info Title
	{ INFO_TITLE,             "ROM Information"},
//ROM Info Text
	{ INFO_ROM_NAME_TEXT,     "ROM Name:"},
	{ INFO_FILE_NAME_TEXT,    "File Name:"},
	{ INFO_LOCATION_TEXT,     "Location:"},
	{ INFO_SIZE_TEXT,         "Size:"},
	{ INFO_CART_ID_TEXT,      "ID:"},
	{ INFO_CRC1_TEXT,         "1st CRC:"},
	{ INFO_CRC2_TEXT,         "2nd CRC:"},
	{ INFO_CIC_CHIP_TEXT,     "CIC Chip:"},
/*********************************************************************************
* Cheats                                                                         *
*********************************************************************************/
//Cheat List
	{ CHEAT_LIST_FRAME,      "Cheats:"},
	{ CHEAT_NOTES_FRAME,     " Notes: "},
	{ CHEAT_MARK_NONE,       "Unmark All"},
//Add Cheat
	{ CHEAT_ADDCHEAT_NAME,   "Name:"},
	{ CHEAT_ADDCHEAT_CODE,   "Code:"},
	{ CHEAT_ADDCHEAT_NOTES,  " Cheat Notes: "},
	{ CHEAT_ADDCHEAT_ADD,    "Add Cheat"},
	{ CHEAT_ADDCHEAT_NEW,    "New Cheat"},
	{ CHEAT_ADDCHEAT_CODEDES,"<address> <value>"},
	{ CHEAT_ADDCHEAT_OPT,    "Options:"},
	{ CHEAT_ADDCHEAT_OPTDES, "<value> <label>"},
//Code extension
	{ CHEAT_CODE_EXT_TITLE,  "Code Extensions"},
	{ CHEAT_CODE_EXT_TXT,    "Please choose a value to be used for:"},
//Edit Cheat
	{ CHEAT_EDITCHEAT_WINDOW,"Edit Cheat"},
	{ CHEAT_EDITCHEAT_UPDATE,"Update Cheat"},
//Cheat Popup Menu
	{ CHEAT_ADDNEW,          "Add New Cheat"},
	{ CHEAT_EDIT,            "Edit"},
	{ CHEAT_DELETE,          "Delete"},
/*********************************************************************************
* Messages                                                                       *
*********************************************************************************/
	{ MSG_CPU_PAUSED,         "CPU paused"},
	{ MSG_CPU_RESUMED,        "CPU resumed"},
	{ MSG_PERM_LOOP,          "In a permanent loop that cannot be exited.\n\nExiting thread"},
	{ MSG_MEM_ALLOC_ERROR,    "Failed to allocate memory.\n\nExiting thread"},
	{ MSG_FAIL_INIT_GFX,      "The default or selected video plugin is missing or invalid.\n\nYou need to select a video (graphics) plugin"},
	{ MSG_FAIL_INIT_AUDIO,    "The default or selected audio plugin is missing or invalid.\n\nYou need to select an audio (sound) plugin"},
	{ MSG_FAIL_INIT_RSP,      "The default or selected RSP plugin is missing or invalid.\n\nYou need to select an RSP plugin\n\n(A possible cause of this error can be Data Execution Prevention interfering with plugin initialization)"},
	{ MSG_FAIL_INIT_CONTROL,  "The default or selected input plugin is missing or invalid.\n\nYou need to select an input (controller) plugin"},
	{ MSG_FAIL_LOAD_PLUGIN,   "Failed to load plugin"},
	{ MENU_ADVANCE_DESC,	  "Advances through a game in single frames at a time (Sync game to Audio suggested)"},
	{ MSG_FAIL_OPEN_SAVE,     "Failed to save this savestate. Save your savestate in a new file or slot, or exit and re-enter Project64 to deal with the issue"},
        { MSG_FAIL_OPEN_FLASH,    "Cannot access save data. You may be running the application from an admin rights directory, or you may have multiple instances of Project64 open. Stop doing either of those to make this error go away"},
	{ MSG_FAIL_OPEN_ZIP,      "Attempt to open zip file failed.\n\nPotentially is a corrupt zip file - try unzipping ROM manually"},
	{ MSG_RB_INFO,		  "Displays ROM-specific information about a selected ROM"},
	{ MSG_FAIL_ZIP,           "Error occured when trying to open zip file"},
	{ MSG_FAIL_IMAGE,         "File loaded does not appear to be a valid ROM.\n\nVerify that your ROM's checksum is valid"},
	{ MSG_UNKNOWN_FILE_FORMAT,"Unknown file format"},
	{ MSG_UNKNOWN_MEM_ACTION, "Unknown memory action.\n\nExiting thread"},
	{ MSG_UNHANDLED_OP,       "Unhandled R4300i OpCode at"},
        { MSG_FAIL_OPEN_MEMPAK,   "Failed to open mempak"},
	{ MSG_NONMAPPED_SPACE,    "Executing from non-mapped space.\n\nExiting thread\n(Try another RSP plugin)\n(Try Check Memory & Return)\n(Try 8MB Memory Size)"},
	{ MSG_MSGBOX_TITLE,       "Error"},
	{ MSG_PIF2_ERROR,         "Copyright sequence not found in LUT. Game will no longer function"},
	{ MSG_PIF2_TITLE,         "Copy Protection Failure"},
	{ MSG_EMULATION_STARTED,  "Emulation started"},
        { MSG_UNABLED_LOAD_STATE, "Unable to load save state"},
        { MSG_EMULATION_ENDED,    "Emulation ended"},
	{ MSG_PLUGIN_NOT_INIT,    "Cannot open a ROM because plugins haven't successfully initialized"},
	{ MSG_CHEAT_NAME_IN_USE,  "Cheat name is currently in use"},
	{ MSG_MAX_CHEATS,         "You have reached the maximum amount of cheats allowed for this ROM"},
	{ GZ_ROMS,                "Exiting thread\n(Try Check Memory & Return)\n(Try Interpreter CPU Core Style)"},
        { MSG_CONFIRMATION_UNINSTALL, "This action will attempt to terminate this instance and factory reset included registry configurations"},
        { MSG_PLUGIN_HOT_SWAP,    "Plugin hot swapping in progress"},
        { HOT_SWAP_COMPLETE,      "Plugin hot swapping completed"},
        { FULLSCREEN_ENTERED,     "Fullscreen mode entered"},
        { GS_PRESS,               "GS button pressed"},
	{ FPS_DISPLAY,            "Frames Per Second"},
	{ OPEN_REGEDIT,		  "Open REGEDIT"},
	{ SLOW_WARN,		  "This action will attempt to terminate this instance and set various LLE slow registry configurations (Interpreter x CF3 x Icepir8's plugins). It's experimental, you should only use this mode if you are having technical difficulties getting a ROM to work properly and you don't know what settings to use"},
	{ MENU_SM64,		  "SM64 ROMhacks Mode"},
	{ MENUDES_SM64,		  "Attempts to terminate this instance and set various SM64 registry configurations"},
	{ SM64_WARN,		  "This action will attempt to terminate this instance and set various SM64 registry configurations for SM64 ROMhacks settings. Change to a lower-end video plugin like Jabo's Direct3D8 1.6.2 after the registries have been reconfigured if your PC does not support GLideN64"},
	{ MENU_STANDARD,	  "Standard Mode"},
	{ MENUDES_STANDARD,	  "Attempts to terminate this instance and set various standard registry configurations"},
	{ STANDARD_WARN,	  "This action will attempt to terminate this instance and set various standard registry configurations for standard settings"},
	{ REGISTRY,		  "Configure Registries"},
	{ NUMBER_2,		  "2 (Laggy, Semi-stable)"},
	{ NUMBER_3,               "3 (Laggard, Stable)"},
	{ PLUGINS,		  "Icepir8's Legacy LLE = High-end LLE Graphics\nJabo's Direct3D8 1.6.2 = Low-end HLE Graphics\nANGLE GLideN64 = SM64 ROMhacks Graphics"},
	{ ROM_ALIGN_DMA,	  "Align DMA"},
	{ ROMSETTWARN,		  "These settings affect how a ROM is emulated.\nChange with utmost caution and awareness."},
};
class CLanguage  {
	void FindLangName  ( int Index );
	void LoadStrings   ( char * FileName );
	void SaveCurrentLang ( char * String );
public:
	CLanguage();
	void CreateLangList ( HMENU hMenu, int uPosition, int MenuID );
	char * GetString    ( int StringID );
	void LoadLangList   ( void );
	void LoadLanguage  ( char * RegLocation );
	const char * LangName  ( int index );
	int  GetNumberLang  ( void );
	void SetCurrentLang ( HMENU hMenu, int MenuIndx );
	int  SetMenuBase    ( int MenuBase );
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
	m_NoOflangs = 0;
	m_NoOfStrings = 0;
	strcpy(m_CurrentLangName,"");
}
int CLanguage::GetNumberLang(void) {
	return m_NoOflangs;
}
const char * CLanguage::LangName  ( int index ) {
	if (index >= MAX_LANGUAGES) { return NULL; }
	return m_LangName[index];
}
void CLanguage::LoadLanguage  ( char * RegLocation ) {
	strncpy(m_RegKey, RegLocation, sizeof(m_RegKey));
	LoadLangList();
	strcpy(m_CurrentLangName,"");
	HKEY hKeyResults = 0;
	long lResult = RegOpenKeyEx( HKEY_CURRENT_USER,RegLocation,0, KEY_ALL_ACCESS,&hKeyResults);
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Bytes = sizeof(m_CurrentLangName);
		lResult = RegQueryValueEx(hKeyResults,"Language",0,&Type,(LPBYTE)(m_CurrentLangName),&Bytes);
		if (lResult != ERROR_SUCCESS) { strcpy(m_CurrentLangName,""); }
	}
	RegCloseKey(hKeyResults);
	bool Found = false;
	for (int count = 0; count < m_NoOflangs; count++) {
		if (strcmp(m_LangName[count], m_CurrentLangName) == 0) {
			LoadStrings(m_filenames[count]);
			Found = true;
			break;
		}
	}
	if (!Found) {
		strcpy(m_CurrentLangName, "English");
	}
}
void CLanguage::CreateLangList ( HMENU hMenu, int uPosition, int MenuID ) {
	LoadLangList();
	HMENU hSubMenu = CreateMenu();
	MENUITEMINFO menuinfo;
	char String[100];
	menuinfo.cbSize = sizeof(MENUITEMINFO);
	menuinfo.fMask = MIIM_TYPE|MIIM_ID;
	menuinfo.fType = MFT_STRING;
	menuinfo.dwTypeData = String;
	menuinfo.cch = sizeof(String);
	if (GetNumberLang() == 0) {
		menuinfo.wID = MenuID;
		strcpy(String,"English");
		InsertMenuItem(hSubMenu, 0, TRUE, &menuinfo);
		CheckMenuItem(hSubMenu, MenuID, MF_BYCOMMAND | MFS_CHECKED );
		EnableMenuItem(hSubMenu,MenuID, MFS_DISABLED|MF_BYCOMMAND);
	}
	for (int count = 0; count < GetNumberLang(); count ++) {
		menuinfo.wID = MenuID + count;
		strcpy(String,lng.LangName(count));
		InsertMenuItem(hSubMenu, 0, TRUE, &menuinfo);
		if (strcmp(m_CurrentLangName, String) == 0) {
			CheckMenuItem(hSubMenu, menuinfo.wID, MF_BYCOMMAND | MFS_CHECKED );
		}
	}
	ModifyMenu(hMenu,uPosition,MF_STRING|MF_POPUP|MF_BYPOSITION,(DWORD)hSubMenu,GS(MENU_LANGUAGE));
}
char * CLanguage::GetString (int StringID) {
	for (int count = 0; count < m_NoOfStrings; count ++) {
		if (m_Strings[count].ID == StringID) { return m_Strings[count].Str; }
	}
	return NULL;
}
void CLanguage::LoadLangList (void) {
	char Directory[_MAX_PATH], SearchString[_MAX_PATH];
	{
		char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
		char fname[_MAX_FNAME],ext[_MAX_EXT];
		GetModuleFileName(NULL,path_buffer,sizeof(path_buffer));
		_splitpath( path_buffer, drive, dir, fname, ext );
		sprintf(Directory,"%s%sPJ64DB\\",drive,dir);
		sprintf(SearchString,"%s*.ldb",Directory);
	}
	WIN32_FIND_DATA find_data;
	HANDLE   search_handle;
	search_handle = FindFirstFile(SearchString, &find_data);
	m_NoOflangs=0;
	if(search_handle !=INVALID_HANDLE_VALUE){
		do {
			strcpy( m_filenames[m_NoOflangs], Directory );
			strcat( m_filenames[m_NoOflangs], find_data.cFileName );
			m_NoOflangs += 1;
		} while (FindNextFile(search_handle, &find_data ) && search_handle != INVALID_HANDLE_VALUE);
		FindClose(search_handle);
	}
	for (int count = 0; count < m_NoOflangs; count ++) { FindLangName(count); }
}
void CLanguage::LoadStrings  ( char * FileName ) {
	m_NoOfStrings = 0;
	FILE *file = fopen(FileName, "rb");
	if (file == NULL) { return; }
	char  token=0;
	while(!feof(file)){
		token = 0;
		//Search for token #
		while(token!='#' && !feof(file)) { fread(&token, 1, 1, file); }
		if(feof(file)){ continue; }
		//get StringID after token
		fscanf(file, "%d", &m_Strings[m_NoOfStrings].ID);
		//Search for token #
		while(token!='#' && !feof(file)) { fread(&token, 1, 1, file); }
		if(feof(file)){ continue; }
		//Search for start of string '"'
		while(token!='"' && !feof(file)) { fread(&token, 1, 1, file); }
		if(feof(file)){ continue; }
		int pos = 0;
		fread(&token, 1, 1, file);
		while(token!='"' && !feof(file)){
			m_Strings[m_NoOfStrings].Str[pos++] = token;
			fread(&token, 1, 1, file);
			if (pos == MAX_STRING_LEN - 2) { token = '"'; }
		}
		m_Strings[m_NoOfStrings].Str[pos++] = 0;
		m_NoOfStrings += 1;
		if (m_NoOfStrings == MAX_STRINGS) { break; }
	}
	fclose(file);
}
void CLanguage::FindLangName  ( int Index ) {
	strcpy(m_LangName[Index],GS(RB_NOT_IN_RDB));
	FILE *file = fopen(m_filenames[Index], "rb");
	if (file == NULL) { return; }
	char  token=0;
	int   StringID;
	while(!feof(file)){
		token = 0;
		//Search for token #
		while(token!='#' && !feof(file)) { fread(&token, 1, 1, file); }
		if(feof(file)){ continue; }
		//get StringID after token
		fscanf(file, "%d", &StringID);
		//Search for token #
		while(token!='#' && !feof(file)) { fread(&token, 1, 1, file); }
		if(feof(file)){ continue; }
		if (StringID != LANGUAGE_NAME) { continue; }
		//Search for start of string '"'
		while(token!='"' && !feof(file)) { fread(&token, 1, 1, file); }
		if(feof(file)){ continue; }
		int pos = 0;
		fread(&token, 1, 1, file);
		while(token!='"' && !feof(file)){
			m_LangName[Index][pos++] = token;
			fread(&token, 1, 1, file);
			if (pos == sizeof(m_LangName[Index]) - 2) { token = '"'; }
		}
		m_LangName[Index][pos++] = 0;
	}
	fclose(file);
}
void CLanguage::SaveCurrentLang ( char * String ) {
	long lResult;
	HKEY hKeyResults = 0;
	DWORD Disposition = 0;
	lResult = RegCreateKeyEx( HKEY_CURRENT_USER, m_RegKey,0,"", REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueEx(hKeyResults,"Language",0, REG_SZ,(CONST BYTE *)String,strlen(String));
		strcpy(m_CurrentLangName,String);
	}
	RegCloseKey(hKeyResults);
	for (int count = 0; count < m_NoOflangs; count++) {
		if (strcmp(m_LangName[count], m_CurrentLangName) == 0) {
			LoadStrings(m_filenames[count]);
			break;
		}
	}
}
void CLanguage::SetCurrentLang ( HMENU hMenu, int MenuIndx ) {
	MENUITEMINFO menuinfo;
	char String[MAX_LANNAME_LEN];
	menuinfo.cbSize = sizeof(MENUITEMINFO);
	menuinfo.fMask = MIIM_TYPE;
	menuinfo.fType = MFT_STRING;
	menuinfo.dwTypeData = String;
	menuinfo.cch = sizeof(String);
	GetMenuItemInfo(hMenu,MenuIndx,FALSE,&menuinfo);
	SaveCurrentLang(String);
}
char * GS (int StringID) {
	int count;
	char * Ret = lng.GetString(StringID);
	if (Ret != NULL) { return Ret; }
	for (count = 0; count < (sizeof(DefaultString) / sizeof(LANG_STR)); count ++) {
		if (DefaultString[count].ID == StringID) { return DefaultString[count].Str; }
	}
	return "";
}
void CreateLangList (HMENU hMenu, int uPosition, int MenuID) {
	lng.CreateLangList(hMenu,uPosition,MenuID);
}
void LoadLanguage ( char * RegLocation ) {
	lng.LoadLanguage(RegLocation);
}
void SelectLangMenuItem ( HMENU hMenu, int LangMenuID) {
	lng.SetCurrentLang(hMenu,LangMenuID);
}