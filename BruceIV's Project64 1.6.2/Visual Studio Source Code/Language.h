#if defined(__cplusplus)
extern "C" {
#endif
void LoadLanguage       (char * RegLocation);
void CreateLangList     (HMENU hMenu,int uPosition,int MenuID);
void SelectLangMenuItem (HMENU hMenu,int LangMenuID);
char * GS               (int StringID);
#if defined(__cplusplus)
}
#endif
/*********************************************************************************
* Meta Information                                                               *
*********************************************************************************/
#define LANGUAGE_NAME	                1
#define LANGUAGE_AUTHOR	                2
#define LANGUAGE_VERSION                3
#define LANGUAGE_DATE	                4
#define INI_CURRENT_LANG                5
#define INI_AUTHOR		        6
#define INI_VERSION		        7
#define INI_DATE		        8
#define LIMITFPS_ON			9
#define INI_CURRENT_GAME_INI                 10
#define INI_CURRENT_CHEAT_INI                 11
// Menu
#define MENU_FILE			12
	#define MENU_OPEN		13
	#define MENU_ROM_INFO		14
	#define MENU_START		15
	#define MENU_END		16
	#define MENU_CHOOSE_ROM		17
	#define MENU_REFRESH		18
	#define MENU_RECENT_ROM		19
	#define MENU_RECENT_DIR		20
	#define MENU_EXIT		21
	#define MENU_SYSTEM		22
	#define MENU_RESET		23
	#define MENU_PAUSE		24
        #define MENU_BITMAP             25
	#define MENU_LIMIT_FPS		26
	#define MENU_SAVE		27
	#define MENU_SAVE_AS		28
	#define MENU_RESTORE		29
	#define MENU_LOAD		30
	#define MENU_CURRENT_SAVE	31
	#define MENU_CHEAT		32
	#define MENU_GS_BUTTON		33
	#define MENU_RESUME		34
#define MENU_OPTIONS			35
	#define MENU_FULL_SCREEN	36
	#define MENU_ON_TOP		37
	#define MENU_CONFIG_GFX		38
	#define MENU_CONFIG_AUDIO	39
	#define MENU_CONFIG_CTRL	40
	#define SPEEDCAP		41
	#define MENU_SETTINGS		42
#define MENU_LANGUAGE			43
#define MENU_HELP			44
	#define MENU_USER_GUIDE		45
	#define MENU_ABOUT_INI		46
	#define MENU_ADVANCE		47
	#define MENU_UNINSTALL		48
// Menu Descriptions
#define MENUDES_OPEN			49
#define MENUDES_ROM_INFO		50
#define MENUDES_START			51
#define MENUDES_END			52
#define MENUDES_CHOOSE_ROM		53
#define MENUDES_REFRESH			54
#define MENUDES_EXIT			55
#define MENUDES_RESET			56
#define MENUDES_PAUSE			57
#define MENUDES_BITMAP                  58
#define MENUDES_LIMIT_FPS		59
#define MENUDES_SAVE			60
#define MENUDES_SAVE_AS			61
#define MENUDES_RESTORE			62
#define MENUDES_LOAD			63
#define MENUDES_CHEAT			64
#define MENUDES_GS_BUTTON		65
#define MENUDES_FULL_SCREEN		66
#define MENUDES_ON_TOP			67
#define MENUDES_CONFIG_GFX		68
#define MENUDES_CONFIG_AUDIO		69
#define MENUDES_CONFIG_CTRL		70
#define SPEEDCAP_ON			71
#define MENUDES_SETTINGS		72
#define MENUDES_USER_GUIDE		73
#define MENUDES_ABOUT_INI		74
#define MENUDES_RECENT_ROM		75
#define MENUDES_RECENT_DIR		76
#define MENUDES_LANGUAGES		77
#define MENUDES_GAME_SLOT		78
#define MENUDES_UNINSTALLAPP	79
#define MSG_LOADED_STATE		80
/*********************************************************************************
* Rom Browser                                                                    *
*********************************************************************************/
//Rom Browser Fields
#define RB_FILENAME			81
#define RB_INTERNALNAME			82
#define RB_GAMENAME			83
#define RB_STATUS			84
#define RB_ROMSIZE			85
#define RB_CART_ID			86
#define RB_CRC1				87
#define RB_CRC2				88
#define RB_CICCHIP			89
#define SHANKLE_AZI_AI_LANG		90
//Messages
#define UNKNOWN		        91
#define FIFTYNINEHERTZ_LANG		92
/*********************************************************************************
* Options                                                                        *
*********************************************************************************/
#define TAB_PLUGIN			93
#define TAB_DIRECTORY			94
#define TAB_ROMSELECTION		95
#define TAB_ADVANCED			96
#define TAB_ROMSETTINGS			97
#define ROMSETTWARN			98
#define TAB_ROMNOTES			99
//Plugin Dialog
#define PLUG_ABOUT			100
#define PLUG_RSP			101
#define PLUG_GFX			102
#define PLUG_AUDIO			103
#define PLUG_CTRL			104
#define DIR_ROM_DEFAULT			105
#define DIR_AUTO_SAVE			106
#define DIR_INSTANT_SAVE		107
#define DIR_SCREEN_SHOT                 108
#define SCREENSHOT_TAKEN		109
#define DIR_SELECT_ROM			110
#define DIR_SELECT_AUTO			111
#define DIR_SELECT_INSTANT		112
#define DIR_SELECT_SCREEN               113
//Options (general) Tab
#define OPTION_AUTO_SLEEP		114
#define OPTION_AUTO_FULLSCREEN	        115
#define OPTION_BASIC_MODE		116
#define OPTION_AUTO_HIDE		117
#define RB_MAX_ROMS			118
#define RB_ROMS				119
#define RB_MAX_DIRS			120
#define RB_DIRS				121
#define SPEEDCAP_DESC			122
#define RB_DIR_RECURSION		123
#define RB_AVAILABLE_FIELDS		124
#define RB_SHOW_FIELDS			125
#define RB_ADD				126
#define RB_REMOVE			127
#define SyncGametoAudio_LANG			128
#define N64_CRASH			129
#define ON			130
#define DEFAULT_TEXT		131
#define FORCE_AUTO4kbit			132
#define NUMBER_1			133
#define SPECIAL_BREAK			134
#define JUMPER_PAK				135
#define Allocate_SD_Info		136
#define ROM_AUDIO_SIGNAL		137
#define ROM_SAVE_TYPE			138
#define ROM_COUNTER_FACTOR		139
#define ROM_JAI		140
#define ROM_USE_TLB			141
#define REG_CACHE			142
#define ROM_DELAY_SI			143
#define CF1_CF0			144
#define ALTERNATE_EMUAI_LANG	145
#define LIMITFPS_OFF			146
#define ROM_DELAY_RDP			147
#define ROM_DELAY_RSP			148
#define USUALLYONTOP_ON			149
//Core Styles
#define OPTIONAL_CRASH			150
#define CORE_RECOMPILER			151
//RDRAM Size
#define AllocateCompile_SD		152
#define AllocateCompile_SD_MENUDES	153
//Core Styles
#define EXTRA_PROJECT64			154
#define MSG_EMULATION_RESET		155
#define SMCM_PROTECTED			156
#define FORCE_RSP_RECOMPILER	157
#define USUALLYONTOP_OFF		158
#define MSG_PLS_START			159
#define SPEEDCAP_OFF			160
#define FORCE_DISABLE_TLB		161
#define FORCE_DISABLE_REGISTERCACHING	162
#define SAVE_4K_EEPROM			163
#define SAVE_16K_EEPROM			164
#define SAVE_SRAM			165
#define SAVE_FlashRAM			166
//Rom Notes
#define NOTE_STATUS			167
#define NOTE_CORE			168
#define NOTE_PLUGIN			169
/*********************************************************************************
* ROM Information                                                                *
*********************************************************************************/
//Rom Info Title
#define INFO_TITLE			170
#define INFO_ROM_NAME_TEXT		171
#define INFO_FILE_NAME_TEXT		172
#define INFO_LOCATION_TEXT		173
#define INFO_SIZE_TEXT			174
#define INFO_CART_ID_TEXT		175
#define INFO_CRC1_TEXT			176
#define INFO_CRC2_TEXT			177
#define INFO_CIC_CHIP_TEXT		178
#define ROM_ALIGN_DMA			179
/*********************************************************************************
* Messages                                                                       *
*********************************************************************************/
#define MSG_CPU_PAUSED			180
#define MSG_CPU_RESUMED			181
#define MSG_MEM_ALLOC_ERROR             182
#define MSG_FAIL_INIT_GFX               183
#define MSG_FAIL_INIT_AUDIO             184
#define MSG_FAIL_INIT_RSP               185
#define MSG_FAIL_INIT_CONTROL           186
#define MSG_FAIL_LOAD_PLUGIN            187
#define MENU_ADVANCE_DESC		188
#define MSG_UNABLE_SAVE_STATE              189
#define MSG_FAIL_OPEN_SAVE				190
#define MSG_FAIL_OPEN_ZIP               191
#define MSG_RB_INFO			192
#define OPENROM_FAILED                  193
#define MSG_FAIL_IMAGE                  194
#define MSG_UNKNOWN_FILE_FORMAT         195
#define MSG_FAIL_OPEN_MEMPAK            196
#define MSG_ERROR_TITLE					197
#define Compile_SD_Info					198
#define THREAD_EXIT						199
#define MSG_EMULATION_STARTED           200
#define MSG_UNABLE_LOAD_STATE			201
#define MSG_EMULATION_ENDED             202
#define MSG_CONFIRMATION_UNINSTALL      203
#define MSG_SAVED_STATE			204
#define GS_PRESS                        205
#define FPS_DISPLAY                     206
#define FORCE_ENABLE_DMA		207
#define NUMBER_2		        208
#define NUMBER_3		        209
#define OFF		210
#define MENUDES_GAME_SETTINGS		211