#if defined(__cplusplus)
extern "C" {
#endif
void LoadLanguage       ( char * RegLocation );
void CreateLangList     ( HMENU hMenu, int uPosition, int MenuID );
void SelectLangMenuItem ( HMENU hMenu, int LangMenuID );
char * GS               ( int StringID );
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
#define INI_CURRENT_RDB                 10
#define INI_CURRENT_CHT                 11
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
	#define MENU_CONFIG_RSP		41
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
#define MENUDES_CONFIG_RSP		71
#define MENUDES_SETTINGS		72
#define MENUDES_USER_GUIDE		73
#define MENUDES_ABOUT_INI		74
#define MENUDES_RECENT_ROM		75
#define MENUDES_RECENT_DIR		76
#define MENUDES_LANGUAGES		77
#define MENUDES_GAME_SLOT		78
#define MENUDES_GAME_CHEATS		79
#define MENUDES_UNINSTALLAPP            80
#define FULLSCREEN_EXITED		81
/*********************************************************************************
* Rom Browser                                                                    *
*********************************************************************************/
//Rom Browser Fields
#define RB_FILENAME			82
#define RB_INTERNALNAME			83
#define RB_GAMENAME			84
#define RB_STATUS			85
#define RB_ROMSIZE			86
#define RB_CART_ID			87
#define RB_CRC1				88
#define RB_CRC2				89
#define RB_CICCHIP			90
#define SELECT_ROM_DIR			91
//Messages
#define RB_NOT_IN_RDB		        92
#define RB_HACK                         93
/*********************************************************************************
* Options                                                                        *
*********************************************************************************/

#define TAB_PLUGIN			94
#define TAB_DIRECTORY			95
#define TAB_ROMSELECTION		96
#define TAB_ADVANCED			97
#define TAB_ROMSETTINGS			98
#define ROMSETTWARN			99
#define TAB_ROMNOTES			100
//Plugin Dialog
#define PLUG_ABOUT			101
#define PLUG_RSP			102
#define PLUG_GFX			103
#define PLUG_AUDIO			104
#define PLUG_CTRL			105
#define DIR_ROM_DEFAULT			106
#define DIR_AUTO_SAVE			107
#define DIR_INSTANT_SAVE		108
#define DIR_SCREEN_SHOT                 109
#define SCREENSHOT_TAKEN		110
#define DIR_SELECT_ROM			111
#define DIR_SELECT_AUTO			112
#define DIR_SELECT_INSTANT		113
#define DIR_SELECT_SCREEN               114
//Options (general) Tab
#define OPTION_AUTO_SLEEP		115
#define OPTION_AUTO_FULLSCREEN	        116
#define OPTION_BASIC_MODE		117
#define OPTION_REMEMBER_CHEAT	        118

#define RB_MAX_ROMS			119
#define RB_ROMS				120
#define RB_MAX_DIRS			121
#define RB_DIRS				122
#define FORCE_ENABLE_DELAYRDP	123
#define RB_DIR_RECURSION		124
#define RB_AVAILABLE_FIELDS		125
#define RB_SHOW_FIELDS			126
#define RB_ADD				127
#define RB_REMOVE			128
#define RB_UP				129
#define RB_DOWN				130
#define ADVANCE_INFO			131
#define ADVANCE_DEFAULTS		132
#define FORCE_AUTO16			133
#define NUMBER_1			134
#define ROM_CPU_STYLE			135
#define ROM_SMCM			136
#define ROM_MEM_SIZE			137
#define ROM_AUDIO_SIGNAL		138
#define ROM_SAVE_TYPE			139
#define ROM_COUNTER_FACTOR		140
#define ROM_LARGE_BUFFER		141
#define ROM_USE_TLB			142
#define ROM_REG_CACHE			143
#define ROM_DELAY_SI			144
#define ROM_SP_HACK			145
#define ROM_SAVE			146
#define LIMITFPS_OFF			147
#define ROM_DELAY_RDP			148
#define ROM_DELAY_RSP			149
#define MENU_SLOW			150
//Core Styles
#define CORE_INTERPRETER		151
#define CORE_RECOMPILER			152
//RDRAM Size
#define RDRAM_4MB			153
#define RDRAM_8MB			154
//Core Styles
#define SMCM_NONE			155
#define SMCM_CACHE			156
#define SMCM_PROTECTED			157
#define SMCM_CHECK_MEM			158
#define SMCM_CHANGE_MEM			159
#define SMCM_CHECK_ADV			160

#define MENUDES_SLOW			161
#define FORCE_DISABLE_TLB		162
#define SMCM_CHECK_RETURN		163
#define SAVE_4K_eepROM			164
#define SAVE_16K_eepROM			165
#define SAVE_SRAM			166
#define SAVE_FlashRAM			167
//Rom Notes
#define NOTE_STATUS			168
#define NOTE_CORE			169
#define NOTE_PLUGIN			170
/*********************************************************************************
* ROM Information                                                                *
*********************************************************************************/
//Rom Info Title
#define INFO_TITLE			171

#define INFO_ROM_NAME_TEXT		172
#define INFO_FILE_NAME_TEXT		173
#define INFO_LOCATION_TEXT		174
#define INFO_SIZE_TEXT			175
#define INFO_CART_ID_TEXT		176
#define INFO_CRC1_TEXT			177
#define INFO_CRC2_TEXT			178
#define INFO_CIC_CHIP_TEXT		179
#define ROM_ALIGN_DMA			180
/*********************************************************************************
* Messages                                                                       *
*********************************************************************************/
#define MSG_CPU_PAUSED			181
#define MSG_CPU_RESUMED			182
//#define MSG_PERM_LOOP                   183
#define MSG_MEM_ALLOC_ERROR             183
#define MSG_FAIL_INIT_GFX               184
#define MSG_FAIL_INIT_AUDIO             185
#define MSG_FAIL_INIT_RSP               186
#define MSG_FAIL_INIT_CONTROL           187
#define MSG_FAIL_LOAD_PLUGIN            188
#define MENU_ADVANCE_DESC		189
#define MSG_FAIL_OPEN_SAVE              190
#define MSG_FAIL_OPEN_FLASH             191
#define MSG_FAIL_OPEN_ZIP               192
#define MSG_RB_INFO			193
#define MSG_FAIL_ZIP                    194
#define MSG_FAIL_IMAGE                  195
#define MSG_UNKNOWN_FILE_FORMAT         196
//#define MSG_UNKNOWN_MEM_ACTION          198
#define MSG_FAIL_OPEN_MEMPAK            197
//#define MSG_NONMAPPED_SPACE             200
#define MSG_MSGBOX_TITLE                198
#define MSG_PIF2_ERROR                  199
#define MSG_PIF2_TITLE                  200
#define MSG_EMULATION_STARTED           201
#define MSG_UNABLED_LOAD_STATE          202
#define MSG_EMULATION_ENDED             203
#define MSG_PLUGIN_NOT_INIT             204
#define MSG_CONFIRMATION_UNINSTALL      205
#define HOT_SWAP_COMPLETE               206
#define FULLSCREEN_ENTERED              207
#define GS_PRESS                        208
#define FPS_DISPLAY                     209
#define FORCE_ENABLE_DMA		210
#define SLOW_WARN			211
#define MENU_SM64			212
#define MENUDES_SM64			213
#define SM64_WARN			214
#define MENU_STANDARD			215
#define MENUDES_STANDARD		216
#define STANDARD_WARN			217
#define REGISTRY			218
#define NUMBER_2		        219
#define NUMBER_3		        220
#define PLUGINS				221
#define MENUDES_GAME_SETTINGS		222
#define OKPOSTSWAP			223
#define OKPRESWAP			224
#define MSG_EMULATION_RESET		225
#define MSG_OCTOMINO			226
#define SPEEDCAP			227
#define SPEEDCAP_ON			228
#define SPEEDCAP_OFF			229
#define SPEEDCAP_DESC			230
#define AllocateCompile_SD		231
#define AllocateCompile_SD_MENUDES	232
#define Allocate_SD_Info		233
#define Compile_SD_Info			234
#define FORCE_ENABLE_REGISTERCACHING 235