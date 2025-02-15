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
#define MENUDES_GAME_SETTINGS	        79
#define MENUDES_GAME_CHEATS		80
#define MENUDES_UNINSTALLAPP            81
#define FULLSCREEN_EXITED		82
/*********************************************************************************
* Rom Browser                                                                    *
*********************************************************************************/
//Rom Browser Fields
#define RB_FILENAME			83
#define RB_INTERNALNAME			84
#define RB_GAMENAME			85
#define RB_STATUS			86
#define RB_ROMSIZE			87
#define RB_NOTES_CORE			88
#define RB_NOTES_PLUGIN			89
#define RB_CART_ID			90
#define RB_CRC1				91
#define RB_CRC2				92
#define RB_CICCHIP			93
#define SELECT_ROM_DIR			94
//Messages
#define RB_NOT_IN_RDB		        95
#define RB_HACK                         96
/*********************************************************************************
* Options                                                                        *
*********************************************************************************/

#define TAB_PLUGIN			97
#define TAB_DIRECTORY			98
#define TAB_ROMSELECTION		99
#define TAB_ADVANCED			100
#define TAB_ROMSETTINGS			101
#define ROMSETTWARN			102
#define TAB_ROMNOTES			103
//Plugin Dialog
#define PLUG_ABOUT			104
#define PLUG_RSP			105
#define PLUG_GFX			106
#define PLUG_AUDIO			107
#define PLUG_CTRL			108
#define DIR_ROM_DEFAULT			109
#define DIR_AUTO_SAVE			110
#define DIR_INSTANT_SAVE		111
#define DIR_SCREEN_SHOT                 112
#define SCREENSHOT_TAKEN		113
#define DIR_SELECT_ROM			114
#define DIR_SELECT_AUTO			115
#define DIR_SELECT_INSTANT		116
#define DIR_SELECT_SCREEN               117
//Options (general) Tab
#define OPTION_AUTO_SLEEP		118
#define OPTION_AUTO_FULLSCREEN	        119
#define OPTION_BASIC_MODE		120
#define OPTION_REMEMBER_CHEAT	        121
//Rom Browser Tab
#define RB_MAX_ROMS			122
#define RB_ROMS				123
#define RB_MAX_DIRS			124
#define RB_DIRS				125
#define RB_USE				126
#define RB_DIR_RECURSION		127
#define RB_AVAILABLE_FIELDS		128
#define RB_SHOW_FIELDS			129
#define RB_ADD				130
#define RB_REMOVE			131
#define RB_UP				132
#define RB_DOWN				133
#define ADVANCE_INFO			134
#define ADVANCE_DEFAULTS		135
#define USERDB				136
#define NUMBER_1			137
#define ROM_CPU_STYLE			138
#define ROM_SMCM			139
#define ROM_MEM_SIZE			140
#define ROM_ABL				141
#define ROM_SAVE_TYPE			142
#define ROM_COUNTER_FACTOR		143
#define ROM_LARGE_BUFFER		144
#define ROM_USE_TLB			145
#define ROM_REG_CACHE			146
#define ROM_DELAY_SI			147
#define ROM_SP_HACK			148
#define ROM_SAVE			149
#define LIMITFPS_OFF			150
#define ROM_DELAY_RDP			151
#define ROM_DELAY_RSP			152
#define MENU_SLOW			153
//Core Styles
#define CORE_INTERPRETER		154
#define CORE_RECOMPILER			155
//RDRAM Size
#define RDRAM_4MB			156
#define RDRAM_8MB			157
//Core Styles
#define SMCM_NONE			158
#define SMCM_CACHE			159
#define SMCM_PROTECTED			160
#define SMCM_CHECK_MEM			161
#define SMCM_CHANGE_MEM			162
#define SMCM_CHECK_ADV			163

#define MENUDES_SLOW			164
#define REGEDIT_MENUDES			165
#define SMCM_CHECK_RETURN		166
#define SAVE_4K_eepROM			167
#define SAVE_16K_eepROM			168
#define SAVE_SRAM			169
#define SAVE_FlashRAM			170
//Shell Intergration Tab
#define SHELL_TEXT			171
//Rom Notes
#define NOTE_STATUS			172
#define NOTE_CORE			173
#define NOTE_PLUGIN			174
/*********************************************************************************
* ROM Information                                                                *
*********************************************************************************/
//Rom Info Title
#define INFO_TITLE			175
//Rom Info Text
#define INFO_ROM_NAME_TEXT		176
#define INFO_FILE_NAME_TEXT		177
#define INFO_LOCATION_TEXT		178
#define INFO_SIZE_TEXT			179
#define INFO_CART_ID_TEXT		180
#define INFO_CRC1_TEXT			181
#define INFO_CRC2_TEXT			182
#define INFO_CIC_CHIP_TEXT		183
/*********************************************************************************
* Cheats                                                                         *
*********************************************************************************/
//Cheat List
#define CHEAT_LIST_FRAME		184
#define CHEAT_NOTES_FRAME		185
#define CHEAT_MARK_NONE			186
//Add Cheat
#define CHEAT_ADDCHEAT_NAME		187
#define CHEAT_ADDCHEAT_CODE		188
#define CHEAT_ADDCHEAT_NOTES	        189
#define CHEAT_ADDCHEAT_ADD 		190
#define CHEAT_ADDCHEAT_NEW 		191
#define CHEAT_ADDCHEAT_CODEDES 	        192
#define CHEAT_ADDCHEAT_OPT 		193
#define CHEAT_ADDCHEAT_OPTDES 	        194
//Code extension
#define CHEAT_CODE_EXT_TITLE	        195
#define CHEAT_CODE_EXT_TXT		196
//Edit Cheat
#define CHEAT_EDITCHEAT_WINDOW	        197
#define CHEAT_EDITCHEAT_UPDATE	        198
//Cheat Popup Menu
#define CHEAT_ADDNEW			199
#define CHEAT_EDIT			200
#define CHEAT_DELETE			201
/*********************************************************************************
* Messages                                                                       *
*********************************************************************************/
#define MSG_CPU_PAUSED			202
#define MSG_CPU_RESUMED			203
#define MSG_PERM_LOOP                   204
#define MSG_MEM_ALLOC_ERROR             205
#define MSG_FAIL_INIT_GFX               206
#define MSG_FAIL_INIT_AUDIO             207
#define MSG_FAIL_INIT_RSP               208
#define MSG_FAIL_INIT_CONTROL           209
#define MSG_FAIL_LOAD_PLUGIN            210
#define MENU_ADVANCE_DESC		211
#define MSG_FAIL_OPEN_SAVE              212
#define MSG_FAIL_OPEN_FLASH             213
#define MSG_FAIL_OPEN_ZIP               214
#define MSG_RB_INFO			215
#define MSG_FAIL_ZIP                    216
#define MSG_FAIL_IMAGE                  217
#define MSG_UNKNOWN_FILE_FORMAT         218
#define MSG_UNKNOWN_MEM_ACTION          219
#define MSG_UNHANDLED_OP                220
#define MSG_FAIL_OPEN_MEMPAK            221
#define MSG_NONMAPPED_SPACE             222
#define MSG_MSGBOX_TITLE                223
#define MSG_PIF2_ERROR                  224
#define MSG_PIF2_TITLE                  225
#define MSG_EMULATION_STARTED           226
#define MSG_UNABLED_LOAD_STATE          227
#define MSG_EMULATION_ENDED             228
#define MSG_PLUGIN_NOT_INIT             229
#define MSG_CHEAT_NAME_IN_USE           230
#define MSG_MAX_CHEATS                  231
#define GZ_ROMS				232
#define MSG_CONFIRMATION_UNINSTALL      233
#define MSG_PLUGIN_HOT_SWAP             234
#define HOT_SWAP_COMPLETE               235
#define FULLSCREEN_ENTERED              236
#define GS_PRESS                        237
#define FPS_DISPLAY                     238
#define OPEN_REGEDIT			239
#define SLOW_WARN			240
#define MENU_SM64			241
#define MENUDES_SM64			242
#define SM64_WARN			243
#define MENU_STANDARD			244
#define MENUDES_STANDARD		245
#define STANDARD_WARN			246
#define REGISTRY			247
#define NUMBER_2		        248
#define NUMBER_3		        249
#define PLUGINS				250
#define ROM_ALIGN_DMA			251