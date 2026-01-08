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
#include <Windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include "Main.h"
#include "CPU.h"
#include "Plugin.h"
#include "Resource.h"
#define NoOfSortKeys	3
typedef struct {
	char     szFullFileName[MAX_PATH+1];
	char     Status[60];
	char     FileName[200];
	char     InternalName[22];
	char     GameName[200];
	char     CartID[3];
	char     Developer[30];
	char     ReleaseDate[30];
	char     Genre[15];
	int		 Players;
	int      RomSize;
	BYTE     Manufacturer;
	BYTE     Country;
	DWORD    CRC1;
	DWORD    CRC2;
	int      CicChip;
	char     ForceFeedback[15];
} ROM_INFO;
typedef struct {
	BYTE     Country;
	DWORD    CRC1;
	DWORD    CRC2;
	long     Fpos;
} ROM_LIST_INFO;
typedef struct {
	int    ListCount;
	int    ListAlloc;
	ROM_INFO * List;
} ITEM_LIST;
typedef struct {
	int    ListCount;
	int    ListAlloc;
	ROM_LIST_INFO * List;
} ROM_LIST;
typedef struct {
	int    Key[NoOfSortKeys];
	BOOL   KeyAscend[NoOfSortKeys];
} SORT_FIELDS;
typedef struct {
	char *status_name;
	COLORREF HighLight;
	COLORREF Text;
	COLORREF SelectedText;
} COLOR_ENTRY;
typedef struct {
	COLOR_ENTRY *List;
	int count;
	int max_allocated;
} COLOR_CACHE;
#define RB_FileName			0
#define RB_InternalName		1
#define RB_GameName			2
#define RB_Status			3
#define RB_RomSize			4
#define RB_CartridgeID		5
#define RB_Crc1				6
#define RB_Crc2				7
#define RB_CICChip			8
void LoadRomList             (void);
void RomList_SortList        (void);
void FillRomExtensionInfo    (ROM_INFO * pRomInfo);
BOOL FillRomInfo             (ROM_INFO * pRomInfo);
#define COLOR_TEXT 0
#define COLOR_SELECTED_TEXT 1
#define COLOR_HIGHLIGHTED 2
void AddToColorCache(COLOR_ENTRY color);
COLORREF GetColor(char *status,int selection);
int ColorIndex(char *status);
void SetColors(char *status);
int CALLBACK RomList_CompareItems(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);
int CALLBACK RomList_CompareItems2(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);
char CurrentRBFileName[MAX_PATH+1]={""};
ROMBROWSER_FIELDS RomBrowserFields[]={ "Game Name",0,RB_GameName,260,RB_GAMENAME,"Internal Name",1,RB_InternalName,133,RB_INTERNALNAME,"File Name",2,RB_FileName,101,RB_FILENAME,"1st CRC",3,RB_Crc1,71,RB_CRC1,"Size",4,RB_RomSize,58,RB_ROMSIZE,"Status",-1,RB_Status,93,RB_STATUS,"2nd CRC",-1,RB_Crc2,71,RB_CRC2,"ID",-1,RB_CartridgeID,23,RB_CART_ID,"CIC Chip",-1,RB_CICChip,79,RB_CICCHIP,};
HWND hRomList=NULL;
int NoOfFields=sizeof(RomBrowserFields) / sizeof(RomBrowserFields[0]),
 FieldType[(sizeof(RomBrowserFields) / sizeof(RomBrowserFields[0]))+1];
ITEM_LIST ItemList={0,0,NULL};
COLOR_CACHE ColorCache;
void AddRomToList (char * RomLocation) {
	LV_ITEM  lvItem;
	ROM_INFO * pRomInfo;
	int index;
	if (ItemList.ListAlloc==0) {
		ItemList.List=(ROM_INFO *)malloc(100 * sizeof(ROM_INFO));
		ItemList.ListAlloc=100;
	} else if (ItemList.ListAlloc==ItemList.ListCount) {
		ItemList.ListAlloc +=100;
		ItemList.List=(ROM_INFO *)realloc(ItemList.List,ItemList.ListAlloc * sizeof(ROM_INFO));
	}
	if (ItemList.List==NULL) {
		DisplayError(GS(MSG_MEM_ALLOC_ERROR));
		DisplayThreadExit("AddRomToList-ItemList.List==NULL");
	}
	pRomInfo=&ItemList.List[ItemList.ListCount];
	if (pRomInfo==NULL) return;
	memset(pRomInfo,0,sizeof(ROM_INFO));
	memset(&lvItem,0,sizeof(lvItem));
	strncpy(pRomInfo->szFullFileName,RomLocation,MAX_PATH);
	if (!FillRomInfo(pRomInfo)) { return;  }
	lvItem.mask=LVIF_TEXT|LVIF_PARAM;
	lvItem.iItem=ListView_GetItemCount(hRomList);
	lvItem.lParam=(LPARAM)ItemList.ListCount;
	lvItem.pszText=LPSTR_TEXTCALLBACK;
	ItemList.ListCount +=1;
	index=ListView_InsertItem(hRomList,&lvItem);
	if (_stricmp(pRomInfo->szFullFileName,LastRoms[0])==0) {
		ListView_SetItemState(hRomList,index,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		ListView_EnsureVisible(hRomList,index,FALSE);
	}
}
void CreateRomListControl (HWND hParent) {
	hRomList=CreateWindow(WC_LISTVIEW,NULL,WS_TABSTOP|WS_VISIBLE|WS_CHILD|LVS_OWNERDRAWFIXED|LVS_SINGLESEL|LVS_REPORT,0,0,0,0,hParent,(HMENU)IDC_ROMLIST,hInst,NULL);
	ResetRomBrowserColomuns();
	LoadRomList();
}
void LoadRomList (void) {
	int count,index;
	ROM_INFO * pRomInfo;
	LV_ITEM  lvItem;
	FreeRomBrowser();
	ListView_DeleteAllItems(hRomList);
	for (count=0; count<ItemList.ListCount; count ++) {
		pRomInfo=&ItemList.List[count];
		memset(&lvItem,0,sizeof(lvItem));
		lvItem.mask=LVIF_TEXT|LVIF_PARAM;
		lvItem.iItem=ListView_GetItemCount(hRomList);
		lvItem.lParam=(LPARAM)count;
		lvItem.pszText=LPSTR_TEXTCALLBACK;
		index=ListView_InsertItem(hRomList,&lvItem);
		if (_stricmp(pRomInfo->szFullFileName,LastRoms[0])==0) {
			ListView_SetItemState(hRomList,index,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			ListView_EnsureVisible(hRomList,index,FALSE);
		}
		SetColors(pRomInfo->Status);
	}
	RomList_SortList();
}
void FillRomExtensionInfo(ROM_INFO * pRomInfo) {
	LPSTR IniFileName;
	char Identifier[100];
	IniFileName=GetIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",pRomInfo->CRC1,pRomInfo->CRC2,pRomInfo->Country);
	if (RomBrowserFields[RB_GameName].Pos>=0) {
		_GetPrivateProfileString(Identifier,"Game Name",GS(UNKNOWN),pRomInfo->GameName,sizeof(pRomInfo->GameName),IniFileName);
		_GetPrivateProfileString(Identifier,"Status",GS(UNKNOWN),pRomInfo->Status,sizeof(pRomInfo->Status),IniFileName);
		SetColors(pRomInfo->Status);
	}
}
BOOL FillRomInfo(ROM_INFO * pRomInfo) {
	char drive[_MAX_DRIVE],dir[_MAX_DIR],ext[_MAX_EXT];
	BYTE RomData[0x1000];
	int count;
	if (RomBrowserFields[RB_CICChip].Pos>=0) {
		if (!LoadDataFromRomFile(pRomInfo->szFullFileName,RomData,sizeof(RomData),&pRomInfo->RomSize)) return FALSE;
	} else {
		if (!LoadDataFromRomFile(pRomInfo->szFullFileName,RomData,0x40,&pRomInfo->RomSize)) return FALSE;
	}
	_splitpath(pRomInfo->szFullFileName,drive,dir,pRomInfo->FileName,ext);
	strcat(pRomInfo->FileName,ext);
	if (RomBrowserFields[RB_InternalName].Pos>=0) {
		memcpy(pRomInfo->InternalName,(void *)(RomData+0x20),20);
		for(count=0 ; count<20; count +=4) {
			pRomInfo->InternalName[count] ^=pRomInfo->InternalName[count+3];
			pRomInfo->InternalName[count+3] ^=pRomInfo->InternalName[count];
			pRomInfo->InternalName[count] ^=pRomInfo->InternalName[count+3];
			pRomInfo->InternalName[count+1] ^=pRomInfo->InternalName[count+2];
			pRomInfo->InternalName[count+2] ^=pRomInfo->InternalName[count+1];
			pRomInfo->InternalName[count+1] ^=pRomInfo->InternalName[count+2];
		}
		pRomInfo->InternalName[21]='\0';
	}
	pRomInfo->CartID[0]=*(RomData+0x3F);
	pRomInfo->CartID[1]=*(RomData+0x3E);
	pRomInfo->CartID[2]='\0';
	pRomInfo->Manufacturer=*(RomData+0x38);
	pRomInfo->Country=*(RomData+0x3D);
	pRomInfo->CRC1=*(DWORD *)(RomData+0x10);
	pRomInfo->CRC2=*(DWORD *)(RomData+0x14);
	if (RomBrowserFields[RB_CICChip].Pos>=0) pRomInfo->CicChip=GetCicChipID(RomData);
	FillRomExtensionInfo(pRomInfo);
	return TRUE;
}
void RefreshRomBrowser (void) {
	char RomDir[MAX_PATH+1];
	if (!hRomList) return;
	ListView_DeleteAllItems(hRomList);
	FreeRomBrowser();
	GetRomDirectory(RomDir);
	FillRomList (RomDir);
	RomList_SortList();
}
void ResetRomBrowserColomuns (void) {
	int Coloumn,index;
	LV_COLUMN lvColumn;
	char szString[300];
	memset(&lvColumn,0,sizeof(lvColumn));
	lvColumn.mask=LVCF_FMT;
	while (ListView_GetColumn(hRomList,0,&lvColumn)) {
		ListView_DeleteColumn(hRomList,0);
	}
	//Add Colomuns
	lvColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt=LVCFMT_LEFT;
	lvColumn.pszText=szString;
	for (Coloumn=0; Coloumn<NoOfFields; Coloumn ++) {
		for (index=0; index<NoOfFields; index ++) {
			if (RomBrowserFields[index].Pos==Coloumn) { break; }
		}
		if (index==NoOfFields||RomBrowserFields[index].Pos!=Coloumn) {
			FieldType[Coloumn]=-1;
			break;
		}
		FieldType[Coloumn]=RomBrowserFields[index].ID;
		lvColumn.cx=RomBrowserFields[index].ColWidth;
		strncpy(szString,GS(RomBrowserFields[index].LangID),sizeof(szString));
		ListView_InsertColumn(hRomList,Coloumn,&lvColumn);
	}
}
void ResizeRomListControl (WORD nWidth,WORD nHeight) {
	if (IsWindow(hRomList)) {
		if (IsWindow(hStatusWnd)) {
			RECT rc;
			GetWindowRect(hStatusWnd,&rc);
			nHeight -=(WORD)(rc.bottom-rc.top);
		}
		MoveWindow(hRomList,0,0,nWidth,nHeight,TRUE);
	}
}
void RomList_ColoumnSortList(LPNMLISTVIEW pnmv) {
	int index;
	for (index=0; index<NoOfFields; index++) {
		if (RomBrowserFields[index].Pos==pnmv->iSubItem) { break; }
	}
	if (NoOfFields==index) return;
	RomList_SortList();
}
int CALLBACK RomList_CompareItems2(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort) {
	SORT_FIELDS * SortFields=(SORT_FIELDS *)lParamSort;
	ROM_INFO * pRomInfo1,* pRomInfo2;
	int count,result;
	for (count=0; count<3; count ++) {
		pRomInfo1=&ItemList.List[SortFields->KeyAscend[count]?lParam1:lParam2];
		pRomInfo2=&ItemList.List[SortFields->KeyAscend[count]?lParam2:lParam1];
		switch (SortFields->Key[count]) {
		case RB_FileName: result=(int)lstrcmpi(pRomInfo1->FileName,pRomInfo2->FileName); break;
		case RB_InternalName: result=(int)lstrcmpi(pRomInfo1->InternalName,pRomInfo2->InternalName); break;
		case RB_GameName: result=(int)lstrcmpi(pRomInfo1->GameName,pRomInfo2->GameName); break;
		case RB_Status: result=(int)lstrcmpi(pRomInfo1->Status,pRomInfo2->Status); break;
		case RB_RomSize: result=(int)pRomInfo1->RomSize-(int)pRomInfo2->RomSize; break;
		case RB_CartridgeID: result=(int)lstrcmpi(pRomInfo1->CartID,pRomInfo2->CartID); break;
		case RB_Crc1: result=(int)pRomInfo1->CRC1-(int)pRomInfo2->CRC1; break;
		case RB_Crc2: result=(int)pRomInfo1->CRC2-(int)pRomInfo2->CRC2; break;
		case RB_CICChip: result=(int)pRomInfo1->CicChip-(int)pRomInfo2->CicChip; break;
		default: result=0; break;
		}
		if (result!=0) { return result; }
	}
	return 0;
}
void RomList_GetDispInfo(LPNMHDR pnmh) {
	LV_DISPINFO * lpdi=(LV_DISPINFO *)pnmh;
	ROM_INFO * pRomInfo=&ItemList.List[lpdi->item.lParam];
	switch(FieldType[lpdi->item.iSubItem]) {
	case RB_FileName: strncpy(lpdi->item.pszText,pRomInfo->FileName,lpdi->item.cchTextMax); break;
	case RB_InternalName: strncpy(lpdi->item.pszText,pRomInfo->InternalName,lpdi->item.cchTextMax); break;
	case RB_GameName: strncpy(lpdi->item.pszText,pRomInfo->GameName,lpdi->item.cchTextMax); break;
	case RB_Status: strncpy(lpdi->item.pszText,pRomInfo->Status,lpdi->item.cchTextMax); break;
	case RB_RomSize: sprintf(lpdi->item.pszText,"%.1f MBit",(float)pRomInfo->RomSize/0x20000); break;
	case RB_CartridgeID: strncpy(lpdi->item.pszText,pRomInfo->CartID,lpdi->item.cchTextMax); break;
	case RB_Crc1: sprintf(lpdi->item.pszText,"0x%08X",pRomInfo->CRC1); break;
	case RB_Crc2: sprintf(lpdi->item.pszText,"0x%08X",pRomInfo->CRC2); break;
	case RB_CICChip:
		if (pRomInfo->CicChip<0) {
			sprintf(lpdi->item.pszText,GS(UNKNOWN));
		} else {
			if (GetCicChipID(RomHeader)==4) sprintf(lpdi->item.pszText,"CIC-NUS-XENO");
			else if (GetCicChipID(RomHeader)==7) sprintf(lpdi->item.pszText,"CIC-NUS-5167");
			else if (GetCicChipID(RomHeader)==8) sprintf(lpdi->item.pszText,"CIC-NUS-8303");
			else if (GetCicChipID(RomHeader)==9) sprintf(lpdi->item.pszText,"CIC-NUS-DDUS");
			else sprintf(lpdi->item.pszText,"CIC-NUS-610%d",pRomInfo->CicChip);
		}
		break;
	default: strncpy(lpdi->item.pszText,"",lpdi->item.cchTextMax);
	}
	if (strlen(lpdi->item.pszText)==0) { strcpy(lpdi->item.pszText,""); }
}
void RomList_PopupMenu(LPNMHDR pnmh) {
	LV_DISPINFO * lpdi=(LV_DISPINFO *)pnmh;
	HMENU hMenu=LoadMenu(hInst,MAKEINTRESOURCE(IDR_POPUP));
	HMENU hPopupMenu=GetSubMenu(hMenu,0);
	ROM_INFO * pRomInfo;
	LV_ITEM lvItem;
	POINT Mouse;
	LONG iItem;
	GetCursorPos(&Mouse);
	iItem=ListView_GetNextItem(hRomList,-1,LVNI_SELECTED);
	if (iItem!=-1) {
		memset(&lvItem,0,sizeof(LV_ITEM));
		lvItem.mask=LVIF_PARAM;
		lvItem.iItem=iItem;
		if (!ListView_GetItem(hRomList,&lvItem)) return;
		pRomInfo=&ItemList.List[lvItem.lParam];
		if (!pRomInfo) return;
		strcpy(CurrentRBFileName,pRomInfo->szFullFileName);
	} else {
		strcpy(CurrentRBFileName,"");
	}
	//Fix up menu
	MenuSetText(hPopupMenu,0,GS(MENU_OPEN),NULL);
	MenuSetText(hPopupMenu,1,GS(MENU_ROM_INFO),NULL);
	MenuSetText(hPopupMenu,3,GS(MENU_CHOOSE_ROM),NULL);
	MenuSetText(hPopupMenu,4,GS(MENU_REFRESH),NULL);
	MenuSetText(hPopupMenu,6,GS(MENU_CHEAT),NULL);
	MenuSetText(hPopupMenu,7,GS(TAB_ROMNOTES),NULL);
	if (strlen(CurrentRBFileName)==0) {
		DeleteMenu(hPopupMenu,7,MF_BYPOSITION);
		DeleteMenu(hPopupMenu,6,MF_BYPOSITION);
		DeleteMenu(hPopupMenu,5,MF_BYPOSITION);
		DeleteMenu(hPopupMenu,2,MF_BYPOSITION);
		DeleteMenu(hPopupMenu,1,MF_BYPOSITION);
		DeleteMenu(hPopupMenu,0,MF_BYPOSITION);
	}
	TrackPopupMenu(hPopupMenu,0,Mouse.x,Mouse.y,0,hMainWindow,NULL);
	DestroyMenu(hMenu);
}
void RomList_OpenRom(LPNMHDR pnmh) {
	ROM_INFO * pRomInfo;
	LV_ITEM lvItem;
	DWORD ThreadID;
	LONG iItem;
	iItem=ListView_GetNextItem(hRomList,-1,LVNI_SELECTED);
	if (iItem==-1) return;
	memset(&lvItem,0,sizeof(LV_ITEM));
	lvItem.mask=LVIF_PARAM;
	lvItem.iItem=iItem;
	if (!ListView_GetItem(hRomList,&lvItem)) return;
	pRomInfo=&ItemList.List[lvItem.lParam];
	if (!pRomInfo) return;
	strcpy(CurrentFileName,pRomInfo->szFullFileName);
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)OpenChosenFile,NULL,0,&ThreadID);
}
void RomList_SortList (void) {
	SORT_FIELDS SortFields;
	ListView_SortItems(hRomList,RomList_CompareItems2,&SortFields);
}
void RomListDrawItem (LPDRAWITEMSTRUCT ditem) {
	RECT rcItem,rcDraw;
	ROM_INFO * pRomInfo;
	char String[300];
	LV_ITEM lvItem;
	BOOL bSelected;
	HBRUSH hBrush;
	LV_COLUMN lvc;
	int nColumn;
	lvItem.mask=LVIF_PARAM;
	lvItem.iItem=ditem->itemID;
	if (!ListView_GetItem(hRomList,&lvItem)) return;
	lvItem.state=ListView_GetItemState(hRomList,ditem->itemID,-1);
	bSelected=(lvItem.state&LVIS_SELECTED);
	pRomInfo=&ItemList.List[lvItem.lParam];
	if (bSelected) {
		hBrush=CreateSolidBrush(GetColor(pRomInfo->Status,COLOR_HIGHLIGHTED));
		SetTextColor(ditem->hDC,GetColor(pRomInfo->Status,COLOR_SELECTED_TEXT));
	} else {
		hBrush=(HBRUSH)(COLOR_WINDOW+1);
		SetTextColor(ditem->hDC,GetColor(pRomInfo->Status,COLOR_TEXT));
	}
	FillRect(ditem->hDC,&ditem->rcItem,hBrush);
	SetBkMode(ditem->hDC,TRANSPARENT);
	//Draw
	ListView_GetItemRect(hRomList,ditem->itemID,&rcItem,LVIR_LABEL);
	ListView_GetItemText(hRomList,ditem->itemID,0,String,sizeof(String));
	memcpy(&rcDraw,&rcItem,sizeof(RECT));
	rcDraw.right -=3;
	DrawText(ditem->hDC,String,strlen(String),&rcDraw,DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_VCENTER);
	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH;
	for (nColumn=1; ListView_GetColumn(hRomList,nColumn,&lvc); nColumn +=1) {
		rcItem.left=rcItem.right;
		rcItem.right +=lvc.cx;
		ListView_GetItemText(hRomList,ditem->itemID,nColumn,String,sizeof(String));
		memcpy(&rcDraw,&rcItem,sizeof(RECT));
		rcDraw.right -=3;
		DrawText(ditem->hDC,String,strlen(String),&rcDraw,DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_VCENTER);
	}
	DeleteObject(hBrush);
}
void RomListNotify(LPNMHDR pnmh) {
	switch (pnmh->code) {
	case LVN_GETDISPINFO: RomList_GetDispInfo(pnmh); break;
	case LVN_COLUMNCLICK: RomList_ColoumnSortList((LPNMLISTVIEW)pnmh); break;
	case NM_RETURN:       RomList_OpenRom(pnmh); break;
	case NM_DBLCLK:       RomList_OpenRom(pnmh); break;
	case NM_RCLICK:       RomList_PopupMenu(pnmh); break;
	}
}
void SelectRomDir (void) {
	char Buffer[MAX_PATH],Directory[255];
	char RomDir[MAX_PATH+1];
	LPITEMIDLIST pidl;
	BROWSEINFO bi;
	GetRomDirectory(RomDir);
	bi.hwndOwner=hMainWindow;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=Buffer;
	bi.lpszTitle=GS(DIR_SELECT_ROM);
	bi.ulFlags=BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
	bi.lpfn=(BFFCALLBACK)SelectRomDirCallBack;
	bi.lParam=(DWORD)RomDir;
	if ((pidl=SHBrowseForFolder(&bi))!=NULL) {
		if (SHGetPathFromIDList(pidl,Directory)) {
			int len=strlen(Directory);
			if (Directory[len-1]!='\\') {
				strcat(Directory,"\\");
			}
			SetRomDirectory(Directory);
		}
	}
}
void FillRomList (char * Directory) {
	char FullPath[MAX_PATH+1],FileName[MAX_PATH+1],SearchSpec[MAX_PATH+1];
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	strcpy(SearchSpec,Directory);
	if (SearchSpec[strlen(Directory)-1]!='\\') { strcat(SearchSpec,"\\"); }
	strcat(SearchSpec,"*.*");
	hFind=FindFirstFile(SearchSpec,&fd);
	if (hFind==INVALID_HANDLE_VALUE) return;
	do {
		char drive[_MAX_DRIVE],dir[_MAX_DIR],ext[_MAX_EXT];
		if (strcmp(fd.cFileName,".")==0) continue;
		if (strcmp(fd.cFileName,"..")==0) continue;
		strcpy(FullPath,Directory);
		if (FullPath[strlen(Directory)-1]!='\\') { strcat(FullPath,"\\"); }
		strcat(FullPath,fd.cFileName);
		if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0) {
			if (Recursion) { FillRomList(FullPath); }
			continue;
		}
		_splitpath(FullPath,drive,dir,FileName,ext);
		//if (_stricmp(ext,".zip")==0&&fd.nFileSizeLow <=(30.1 * 1024 * 1024)) { AddRomToList(FullPath); continue; }
		if (_stricmp(ext,".v64")==0) { AddRomToList(FullPath); continue; }
		if (_stricmp(ext,".z64")==0) { AddRomToList(FullPath); continue; }
		if (_stricmp(ext,".n64")==0) { AddRomToList(FullPath); continue; }
	} while (FindNextFile(hFind,&fd));
	FindClose(hFind);
}
void HideRomBrowser(void) {
	if (!inFullScreen) {
		long Style;
		ShowWindow(hMainWindow,SW_HIDE);
		Style=GetWindowLong(hMainWindow,GWL_STYLE);
		if (strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0) ChangeWinSize(hMainWindow,640,480,NULL);
		else {
			Style=Style&~(WS_SIZEBOX|WS_MAXIMIZEBOX);
			SetWindowLong(hMainWindow,GWL_EXSTYLE,GetWindowLong(hMainWindow,GWL_EXSTYLE)&~WS_EX_COMPOSITED);
		}
		SetWindowLong(hMainWindow,GWL_STYLE,Style);
		EnableWindow(hRomList,FALSE);
		ShowWindow(hRomList,SW_HIDE);
		SendMessage(hMainWindow,WM_USER+17,0,0);
		ShowWindow(hMainWindow,SW_SHOW);
		FixupMenubar(hMainWindow);
		UsuallyonTopWindow(hMainWindow);
	}
	if (strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0&&!GLideN64HasBeenSetupFirst&&!inFullScreen) {
		GLideN64NeedsToBeSetupFirst=TRUE;
		strcpy(GfxDLL,"GLideN64.dll");
	}
	SetupPlugins(hMainWindow);
}
void HandleShutdown (HWND hParent) {
	CPURunning=FALSE;
	SetupPlugins(hHiddenWin);
	if (strcmp(GfxDLL,"Icepir8sLegacyLLE.dll")==0) SetWindowLong(hMainWindow,GWL_EXSTYLE,GetWindowLong(hMainWindow,GWL_EXSTYLE)&~WS_EX_COMPOSITED);
	else SetWindowLong(hMainWindow,GWL_STYLE,GetWindowLong(hMainWindow,GWL_STYLE)|WS_SIZEBOX|WS_MAXIMIZEBOX);
	if (hRomList==NULL) CreateRomListControl(hParent);
	else EnableWindow(hRomList,TRUE);
	ChangeWinSize(hMainWindow,640,480,NULL);
	ShowWindow(hRomList,SW_SHOW);
	DrawMenuBar(hMainWindow);
	ShowWindow(hMainWindow,SW_SHOW);
	if (__argc!=0) RefreshRomBrowser();
	if (__argc!=1) __argc=1;
	ListView_SetExtendedListViewStyleEx(hRomList,LVS_EX_DOUBLEBUFFER,LVS_EX_DOUBLEBUFFER);
	UsuallyonTopWindow(hMainWindow);
	SetForegroundWindow(hMainWindow);
}
void FreeRomBrowser (void) {
	if (ItemList.ListAlloc!=0) {
		free(ItemList.List);
		ItemList.ListAlloc=0;
		ItemList.ListCount=0;
		ItemList.List=NULL;
	}
}
void AddToColorCache(COLOR_ENTRY color) {
	// Allocate more memory if there is not enough to store the new entry.
	if (ColorCache.count==ColorCache.max_allocated) {
		COLOR_ENTRY *temp;
		const int increase=ColorCache.max_allocated+5;
		temp=(COLOR_ENTRY *)realloc(ColorCache.List,sizeof(COLOR_ENTRY) * increase);
		if (temp==NULL) return;
		ColorCache.List=temp;
		ColorCache.max_allocated=increase;
	}
	ColorCache.List[ColorCache.count]=color;
	ColorCache.count++;
}
COLORREF GetColor(char *status,int selection) {
	int i=ColorIndex(status);
	switch(selection) {
	case COLOR_SELECTED_TEXT:
		if (i==-1) return RGB(0xFF,0xFF,0xFF);
		return ColorCache.List[i].SelectedText;
	case COLOR_HIGHLIGHTED:
		if (i==-1) return RGB(0,0,0);
		return ColorCache.List[i].HighLight;
	default:
		if (i==-1) return RGB(0,0,0);
		return ColorCache.List[i].Text;
	}
}
int ColorIndex(char *status) {
	int i;
	for (i=0; i<ColorCache.count; i++)
		if (strcmp(status,ColorCache.List[i].status_name)==0) return i;
	return -1;
}
void SetColors(char *status) {
	int count;
	COLOR_ENTRY colors;
	char String[100];
	LPCSTR IniFileName;
	IniFileName=GetIniFileName();
	if (ColorIndex(status)==-1) {
		_GetPrivateProfileString("ROM Status",status,"000000",String,7,IniFileName);
		count=(AsciiToHex(String)&0xFFFFFF);
		colors.Text=(count&0x00FF00)|((count>>0x10)&0xFF)|((count&0xFF)<<0x10);
		sprintf(String,"%s.Sel",status);
		_GetPrivateProfileString("ROM Status",String,"FFFFFFFF",String,9,IniFileName);
		count=AsciiToHex(String);
		if (count<0) {
			colors.HighLight=COLOR_HIGHLIGHT+1;
		} else {
			count=(AsciiToHex(String)&0xFFFFFF);
			count=(count&0x00FF00)|((count>>0x10)&0xFF)|((count&0xFF)<<0x10);
			colors.HighLight=count;
		}
		colors.SelectedText=0xFFFFFF;
		colors.status_name=(char *)malloc(strlen(status)+1);
		strcpy(colors.status_name,status);
		AddToColorCache(colors);
	}
}
void SaveRomBrowserColoumnInfo (void) {
	DWORD Disposition=0;
	HKEY  hKeyResults=0;
	char  String[256];
	long  lResult;
	sprintf(String,"PJ64 V 1.6.2\\Configuration\\ROM Browser");
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult==ERROR_SUCCESS) {
		int Coloumn,index;
		LV_COLUMN lvColumn;
		memset(&lvColumn,0,sizeof(lvColumn));
		lvColumn.mask=LVCF_WIDTH;
		for (Coloumn=0;ListView_GetColumn(hRomList,Coloumn,&lvColumn); Coloumn++) {
			for (index=0; index<NoOfFields; index++) {
				if (RomBrowserFields[index].Pos==Coloumn) { break; }
			}
			RomBrowserFields[index].ColWidth=lvColumn.cx;
			sprintf(String,"%s.Width",RomBrowserFields[index].Name);
			RegSetValueEx(hKeyResults,String,0,REG_DWORD,(BYTE *)&lvColumn.cx,sizeof(DWORD));
		}
		RegCloseKey(hKeyResults);
	}
}
void SaveRomBrowserColoumnPosition (int index,int Position) {
	char  String[256],szPos[10];
	DWORD Disposition=0;
	HKEY  hKeyResults=0;
	long  lResult;
	sprintf(String,"PJ64 V 1.6.2\\Configuration\\ROM Browser");
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER,String,0,"",REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);
	if (lResult==ERROR_SUCCESS) {
		sprintf(szPos,"%d",Position);
		RegSetValueEx(hKeyResults,RomBrowserFields[index].Name,0,REG_SZ,(CONST BYTE *)szPos,strlen(szPos));
		RegCloseKey(hKeyResults);
	}
}
void LoadRomBrowserColoumnInfo (void) {
	char  String[256],szPos[10];
	DWORD Disposition=0;
	HKEY  hKeyResults=0;
	long  lResult;
	NoOfFields ;
	sprintf(String,"PJ64 V 1.6.2\\Configuration\\ROM Browser");
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,&hKeyResults);
	if (lResult==ERROR_SUCCESS) {
		DWORD Type,Value,count,Bytes=4;
		for (count=0; count<(DWORD)NoOfFields; count ++) {
			Bytes=sizeof(szPos);
			// Coloumn Position
			lResult=RegQueryValueEx(hKeyResults,RomBrowserFields[count].Name,0,&Type,(LPBYTE)szPos,&Bytes);
			if (lResult==ERROR_SUCCESS) { RomBrowserFields[count].Pos=atoi(szPos); }
			//Coloumn Width
			Bytes=sizeof(Value);
			sprintf(String,"%s.Width",RomBrowserFields[count].Name);
			lResult=RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)(&Value),&Bytes);
			if (lResult==ERROR_SUCCESS) { RomBrowserFields[count].ColWidth=Value; }
		}
		RegCloseKey(hKeyResults);
	}
	ResetRomBrowserColomuns();
}