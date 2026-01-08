//#include <wchar.h> char‚ğwchar_t‚É’¼‚·‚Ì‚Íª‹C‚ª—v‚éB
#include "Setting.h"
#include "DefOrg.h"
#include "resource.h"
#include "Gdi.h"
#include "OrgData.h"
#include "Scroll.h"
#include "Mouse.h"
#include "Click.h"
#include <stdio.h>
#include "Filer.h"
#include <string.h>

#include "Sound.h"
#include "Timer.h"
#define GET_MEAS1	1
#define GET_MEAS2	3
#define GET_BEAT1	2
#define GET_BEAT2	4

//#define VIRTUAL_CB_SIZE 640000	// 2014.10.19 D
#define VIRTUAL_CB_SIZE 64	// 2014.10.19 A

#define MESSAGE_STRING_BUFFER_SIZE (1024*1024)
#define MESSAGE_STRING_MAX 1024

/* //«‚±‚Ìˆês‚ğ’Ç‰Á‚·‚éš

#include "rxoFunction.h"

  */
/* //EditNote‚Å‚Ìƒgƒ‰ƒbƒNw’è
	-1 : 0 ~ 8
	-2 : 8 ~ 16
	-3 : 0 ~ 16 
	-4 : CurrentTrack
  */
MEMORYSTATUS rMem ;
//GlobalMemoryStatus( &Mem ) ;

extern void SetModified(bool mod);

extern HWND hDlgTrack;
extern int mute_name[MAXTRACK];
extern char timer_sw;
extern NOTECOPY nc_Select; //‘I‘ğ”ÍˆÍ
extern int tra, ful ,haba; 
extern int sGrid;	//”ÍˆÍ‘I‘ğ‚ÍƒOƒŠƒbƒh’PˆÊ‚Å
extern int sACrnt;	//”ÍˆÍ‘I‘ğ‚Íí‚É¶ÚİÄÄ×¯¸
extern int gDrawDouble;	//—¼•û‚Ìƒgƒ‰ƒbƒNƒOƒ‹[ƒv‚ğ•`‰æ‚·‚é
extern CHAR app_path[];
extern int iDragMode;
extern int SaveWithInitVolFile;
extern int sMetronome;
extern int sSmoothScroll;

TCHAR *MessageStringBuffer = NULL;	// 2014.10.19 A
TCHAR *MessageString[MESSAGE_STRING_MAX];

//int msgbox(HWND hWnd , int MessageID, int TitleID, UINT uType);

int iChangeEnablePlaying; //2010.09.23 A
int iChangeFinish; //2010.09.23 A

int iActivatePAN = 0; //2014.05.01 A
int iActivateVOL = 0; //2014.05.01 A

int iSlideOverlapNotes = 0; //d‚È‚è‡‚¤‰¹•„‚Ì•\¦‚ğáŠ±‚¸‚ç‚· 2014.05.06 A

int Menu_Recent[]={
	IDM_RECENT1, IDM_RECENT2, IDM_RECENT3, IDM_RECENT4, IDM_RECENT5, IDM_RECENT6, IDM_RECENT7, IDM_RECENT8, IDM_RECENT9, IDM_RECENT0
};
char *FileAcc[]={
	"File1", "File2", "File3", "File4", "File5", "File6", "File7", "File8", "File9", "File10"
};

char RecentFileName[10][MAX_PATH];	//Å‹ßg‚Á‚½ƒtƒ@ƒCƒ‹–¼
void SetMenuRecent(int iMenuNumber, char *strText, int iDisable);
void CreateMenuRecent();

void ShowStatusMessage(void);

int iRecentTrackM[MAXMELODY]={ // 2010.09.23 A Å‹ßg‚Á‚½ƒgƒ‰ƒbƒN”Ô†
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};
int iRecentTrackD[MAXDRAM]={ // 2010.09.23 A Å‹ßg‚Á‚½ƒgƒ‰ƒbƒN”Ô†
	16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
};

int NoteWidth; //‰¹•„‚Ì•
int NoteEnlarge_Until_16px; //•\¦‚ÌÛANOTE‚Ì“ª‚ğÅ‘å16ƒsƒNƒZƒ‹‚Ü‚ÅL‚Î‚·B
int iPushStratch = 0; //2014.05.31
int iLastEditNoteLength = 1;

void setRecentTrack(int iNewTrack) { 
	int iRT[MAXMELODY];
	int i, j;
	if (iNewTrack < MAXMELODY) {
		for (i = 0; i < MAXMELODY; i++) {
			iRT[i] = iRecentTrackM[i];
		}
		iRecentTrackM[0] = iNewTrack;
		j = 1;
		for (i = 0; i < MAXMELODY; i++) {
			if (iRT[i] != iNewTrack) {
				iRecentTrackM[j] = iRT[i];
				j++;
			}
		}
	}
	else {
		for (i = 0; i < MAXMELODY; i++) {
			iRT[i] = iRecentTrackD[i];
		}
		iRecentTrackD[0] = iNewTrack;
		j = 1;
		for (i = 0; i < MAXMELODY; i++) {
			if (iRT[i] != iNewTrack) {
				iRecentTrackD[j] = iRT[i];
				j++;
			}
		}
	}
	return;
}


//iOrder:0`7
//isDrumTrack:0 ƒƒƒfƒB  ,   isDrumTrack:1 ƒhƒ‰ƒ€
int getRecentTrack(int iOrder, int isDrumTrack){ //Å‹ßg‚Á‚½ƒgƒ‰ƒbƒN‚ğ•Ô‚·
	int i,j;
	j=0;
 	if(isDrumTrack==0){
		for(i=0;i<MAXMELODY;i++){
			if(7-iOrder==j)return iRecentTrackM[i];
			j++;
		}
	}else{
		for(i=0;i<MAXMELODY;i++){
			if(7-iOrder==j)return iRecentTrackD[i];
			j++;
		}
	}
	return iOrder; //–{—ˆA‚±‚Ì’l‚Å•Ô‚³‚ê‚é‚±‚Æ‚Í‚È‚¢‚Í‚¸...
}

//Å‹ßg‚Á‚½ƒtƒ@ƒCƒ‹ŒQ‚Éƒvƒbƒg
void PutRecentFile(char *FileName)
{
	int i,j;
	j=9;
	for(i=0;i<10;i++){
		if(strcmp(RecentFileName[i],FileName)==0){ //“™‚µ‚¢‚Æ‚«
			j=i; i=999;
		}
	}

	for(i=j;i>=1;i--){
		strcpy(RecentFileName[i],RecentFileName[i-1]);
	}
	strcpy(RecentFileName[0],FileName);
	CreateMenuRecent();
}


void LoadRecentFromIniFile(){
	int i;
	for(i=0;i<10;i++){
		RecentFileName[i][0]='@';
		RecentFileName[i][1]='\0';
		GetPrivateProfileString( "Recent",FileAcc[i],"@",RecentFileName[i],256,app_path);
	}
	CreateMenuRecent();
}

void SetMenuRecent(int iMenuNumber, char *strText, int iDisable)
{

	if(iMenuNumber<0 || iMenuNumber>9)return;
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	char strCc[256];
	strcpy(strCc,"  &&");
	itoa((iMenuNumber+1)%10, &strCc[3], 10);
	strCc[4]='\0';
	strcat(strCc," ");
	//strcat(strCc,strText);
	int y,i;
	y = strlen(strText);
	for(i=y;i>0;i--)if(strText[i]=='\\'){i++;break;}
	strcat(strCc,&strText[i]);
	if(iMenuNumber==0){
		strcat(strCc,"\tCtrl+Shift+Home");
	}
	ModifyMenu(hMenu, Menu_Recent[iMenuNumber], MF_BYCOMMAND|MF_STRING, Menu_Recent[iMenuNumber], strCc);
	if(iDisable){
		EnableMenuItem(hMenu,Menu_Recent[iMenuNumber],MF_BYCOMMAND|MF_GRAYED);
	}else{
		EnableMenuItem(hMenu,Menu_Recent[iMenuNumber],MF_BYCOMMAND|MF_ENABLED);
	}
}

void ClearRecentFile()
{
	int a;
	a = MessageBox(hWnd,"Test1","uÅ‹ßg‚Á‚½ƒtƒ@ƒCƒ‹v‚ÌƒNƒŠƒA", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);	// 2014.10.19 D
	if(a == IDOK){
		int i;
		for(i=0;i<10;i++){
			RecentFileName[i][0]='@';
			RecentFileName[i][1]='\0';
		}
		CreateMenuRecent();
		MessageBox(hWnd,"test","’Ê’m",MB_OK);	// 2014.10.19 D

	}else{
		MessageBox(hWnd,"test2","’Ê’m",MB_OK);	// 2014.10.19 D
	}

	
}

void CreateMenuRecent()
{
	int i;
	for(i=0;i<10;i++){
		if(RecentFileName[i][0]!='@'){
			SetMenuRecent(i,RecentFileName[i],0);
		}else{
			//SetMenuRecent(i,"–¢g—p",1);	// 2014.10.19 D
			SetMenuRecent(i,MessageString[IDS_STRING76],1);	// 2014.10.19 A
		}
	}

}

void SaveRecentFilesToInifile()
{
	int i;
	for (i = 0; i < 10; i++) {
		WritePrivateProfileString("Recent",FileAcc[i],RecentFileName[i],app_path);
	}
}

bool SetLoadRecentFile(int iNum)
{
	if (iNum < 0 || iNum > 9) return false;
	if (org_data.FileCheckBeforeLoad(RecentFileName[iNum])) return false;
	strcpy(music_file, RecentFileName[iNum]);
	return true;
}

int GetSelectMeasBeat(int GetToValue, int addValue)
{
	if (tra < 0) return 0;
	int r,g;	//line ‰½”‘‚©  // dot ‚P”‘‚Ì•ªŠ„”
	unsigned char line,dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot; line = mi.line;
	//r = nc_Select.x1_1;
	g = dot*line; r = 0;
	switch(GetToValue){
	case GET_MEAS1:
		r = nc_Select.x1_1/g;
		break;
	case GET_MEAS2:
		r = (nc_Select.x1_2+addValue)/g;
		break;
	case GET_BEAT1:
		r = nc_Select.x1_1%g;
		break;
	case GET_BEAT2:
		r = (nc_Select.x1_2+addValue)%g;
		break;
	}
	return r;
}
char *TrackCode[]={"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","Q","W","E","R","T","Y","U","I","A","S","D","F","G","H","J","K"};

//Returns the values of each track
int ReverseTrackCode(char *strTrack)
{
	int i;
	i=-1;
	do{
		i++;
		switch(strTrack[i]){
			//Melody
		case '1':
			return 0;
		case '2':
			return 1;
		case '3':
			return 2;
		case '4':
			return 3;
		case '5':
			return 4;
		case '6':
			return 5;
		case '7':
			return 6;
		case '8':
			return 7;
		case '9':
			return 8;
		case '10':
			return 9;
		case '11':
			return 10;
		case '12':
			return 11;
		case '13':
			return 12;
		case '14':
			return 13;
		case '15':
			return 14;
		//Drams
		case '16':
			return 15;
		case 'Q':
		case 'q':
			return 16;
		case 'W':
		case 'w':
			return 17;
		case 'E':
		case 'e':
			return 18;
		case 'R':
		case 'r':
			return 19;
		case 'T':
		case 't':
			return 20;
		case 'Y':
		case 'y':
			return 21;
		case 'U':
		case 'u':
			return 22;
		case 'I':
		case 'i':
			return 23;
		case 'A':
		case 'a':
			return 24;
		case 'S':
		case 's':
			return 25;
		case 'D':
		case 'd':
			return 26;
		case 'F':
		case 'f':
			return 27;
		case 'G':
		case 'g':
			return 28;
		case 'H':
		case 'h':
			return 29;
		case 'J':
		case 'j':
			return 30;
		case 'K':
		case 'k':
			return 31;
		}
	}

	while(strTrack[i]==' '); //If strTrack is blank return 99.
	return 99;
}

void MuteTrack(int Track)
{
	SendDlgItemMessage(hDlgTrack , mute_name[Track] , BM_CLICK , 0, 0);
	
}


void EditNote(int AddNotes , int Track , int Function)
{
	if(timer_sw!=0)return;
	if(AddNotes==0)return;
	int j,jmin,jmax,Trc;
	Trc = Track;
	RECT rect = {64,0,WWidth,WHeight};//XV‚·‚é—Ìˆæ
	PARCHANGE pc;
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	pc.x1 = 0  * mi.dot * mi.line;
	pc.x2 = mi.end_x * mi.dot * mi.line - 1;	
	if(AddNotes<0){
		pc.mode = MODEPARSUB;
		pc.a = -AddNotes;
	}
	else{
		pc.mode = MODEPARADD;
		pc.a = AddNotes;
	}

	if(tra>=0){ //‘I‘ğ”ÍˆÍ‚ª‚ ‚ê‚ÎB
		if(ful == 1 || tra == org_data.track){
			pc.x1 = nc_Select.x1_1;
			pc.x2 = nc_Select.x1_2;
		}
		if(ful == 1 && sACrnt>0){
			if(org_data.track<MAXMELODY){
				Trc=-1;
			}else{
				Trc=-2;
			}
		}
	}

	if(Trc<0){
		jmin = 0;
		jmax = MAXMELODY;
		if(Trc==-2){
			jmin = MAXMELODY;
			jmax = MAXTRACK;
		}else if(Trc==-3){
			jmax = MAXTRACK;
		}else if(Trc==-4){ //ƒJƒŒƒ“ƒgƒgƒ‰ƒbƒN
			jmin = (int)org_data.track;
			jmax = jmin + 1;
		}
	}else{
		jmin = Track;
		jmax = Track+1;
	}
	for(j=jmin;j<jmax;j++){
		pc.track = j;
		if(Function==0)org_data.ChangeTransData(&pc);
		else if(Function==1)org_data.ChangeVolumeData(&pc, 0);
		else if(Function==2)org_data.ChangePanData(&pc);
		else if(Function==10){
			pc.mode = MODEMULTIPLY;
			org_data.ChangeVolumeData(&pc, 0);
		}else if(Function>=MODEDECAY && Function<MODEDECAY+20){
			pc.mode = (unsigned char)Function;
			org_data.ChangeVolumeData(&pc, 0);
		}else if(Function>=MODEDECAY+20){
			org_data.EnsureEmptyArea(&pc, Function - MODEDECAY - 20);
		}
	}
	//org_data.PutMusic();	//•\¦
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//MessageBox(hdwnd,"w’è”ÍˆÍ‚ÌƒL[‚ğ•ÏX‚µ‚Ü‚µ‚½","’Ê’m",MB_OK);
	return;
}

void VolumeDecayEdit(int AddNotes, int Track, int Function)  //2014.05.01 A
{
	EditNote(AddNotes, Track, MODEDECAY + Function);
}

//‰¹‚Ì‚‚³‚ğã‚°‚é Track=-1‚Å‚·‚×‚Ä(ƒhƒ‰ƒ€‚Í‚Ì‚¼‚­)
void TransportNote(int AddNotes , int Track )
{
	EditNote(AddNotes , Track , 0);
}

void VolumeEdit(int AddNotes , int Track )
{
	EditNote(AddNotes , Track , 1);
}
void VolumeWariaiEdit(int AddNotes , int Track )  //2014.04.30 A
{
	EditNote(AddNotes , Track , 10);
}

void PanEdit(int AddNotes , int Track )
{
	EditNote(AddNotes , Track , 2);
}

void ShowMemoryState(){ //ƒfƒoƒbƒO—p
	char cc[32]; int y;
	GlobalMemoryStatus( &rMem ) ;
	y=rMem.dwAvailPhys/1000;
	_itoa(y,cc,10);
	MessageBox(NULL,cc,"Mem",MB_OK);
}

//‰¹•„‚Ì®—
void SortMusicNote(void)
{
	int a;
	a = MessageBox(hWnd,"’·ŠÔ‚Ìg—p‚É‚æ‚èAƒm[ƒgi‰¹•„j‚ªƒƒ‚ƒŠã‚É\nU—‚µ‚Ä‚µ‚Ü‚¢‚Ü‚·Bi•ˆ–Ê‡‚Æƒƒ‚ƒŠ‡‚ÍˆÙ‚È‚Á‚Ä‚¢‚éj\n‚±‚ÌŠÖ”‚Íƒm[ƒg‚ğ•ˆ–Ê‚Ì‡”Ô‚É\n•À‚×Š·‚¦‚Ü‚·B\n®Aƒf[ƒ^‚ğƒ[ƒh‚µ’¼‚µ‚Ä‚à“¯‚¶Œø‰Ê‚ª“¾‚ç‚ê‚Ü‚·B\nÀs‚µ‚Ü‚·‚©H","g‚¢•û‚Æ–Ú“I",MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);	// 2014.10.19 D
	if(a == IDOK){
		org_data.SortNotes();
		MessageBox(hWnd,"•À‚×‘Ö‚¦EÄ","Done",MB_OK);	// 2014.10.19 D

	}else{
		MessageBox(hWnd,"ƒLƒƒƒ“ƒZƒ‹‚µ‚Ü‚µ‚½B","Done",MB_OK);	// 2014.10.19 D
	}
}

//‰¼‘z“I‚ÉƒNƒŠƒbƒvƒ{[ƒh‚ğ—pˆÓ‚µ‚ÄAƒf[ƒ^‚Ì‚â‚èæ‚è‚Í‚±‚¢‚Â‚ğ‰î‚µ‚Äs‚¤B
char VirtualCB[VIRTUAL_CB_SIZE];
char *readVCB;

void ClearVirtualCB(void)
{
	VirtualCB[0]=0;
	readVCB = VirtualCB;
}

void AddIntegerToVirtualCB(int iNum)
{
	char ins[32];
	itoa(iNum,ins,10);
	strcat(VirtualCB,ins);
	strcat(VirtualCB,"|");
}

void AddTrackSeparater(void)
{
	strcat(VirtualCB,"@");
}

//ƒZƒpƒŒ[ƒ^‚ğ”­Œ©‚µ‚½‚çtrue
bool ReadTrackSeparater(void)
{
	if(*readVCB!='@')return false;
	readVCB++;
	return true;
}
void AddStartToVirtualCB(void)
{
	strcpy(VirtualCB,"OrgCBData|");
}

//“Ç‚İo‚µŠJn‚³‚¹‚é‚Æ“¯‚ÉA³‹K‚Ìƒf[ƒ^‚©ƒ`ƒFƒbƒN
bool ReadStartFromVirtualCB(void)
{
	readVCB = &VirtualCB[10]; //æ“ª‚ÌŒ`®H
	if(VirtualCB[0]=='O' && VirtualCB[1]=='r' && VirtualCB[2]=='g' && 
		VirtualCB[3]=='C' && VirtualCB[4]=='B' && VirtualCB[5]=='D' && 
		VirtualCB[6]=='a' && VirtualCB[7]=='t' && VirtualCB[8]=='a')return true;
	VirtualCB[10]='\0'; //ƒf[ƒ^”jŠü
	return false;

}

int ReadIntegerFromVirtualCB(void)
{
	if(*readVCB=='\0')return -9999;
	char ons[32], *cp;
	cp = ons;
	do{
		*cp = *readVCB;
		readVCB++;
		cp++;
	}while(*readVCB!='|');
	readVCB++;
	*cp=0;
	int i;
	i = atoi(ons);
	return i;
}

//–{•¨‚ÌCB‚ÉƒRƒs[
void SetClipBoardFromVCB(void)
{
	//MessageBox(NULL,VirtualCB,"Error(Copy)",MB_OK);
	HGLOBAL hText;
	char *pText;
	hText = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, 640000);
	pText = (char*)GlobalLock(hText);
	lstrcpy(pText, VirtualCB);
	GlobalUnlock(hText);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hText);
	CloseClipboard();

	ClearVirtualCB();

}

//VCB‚Ö‘ã“ü
void GetClipBoardToVCB(void)
{
	HANDLE hText;
	char *pText;

	OpenClipboard(NULL);

	hText = GetClipboardData(CF_TEXT);
	if(hText == NULL) {
		//printf("ƒNƒŠƒbƒvƒ{[ƒh‚ÉƒeƒLƒXƒgƒf[ƒ^‚Í‚È‚¢B\n");
	} else {
		pText = (char*)GlobalLock(hText);
		int i;
		for(i=0;i<640000;i++){
			VirtualCB[i]=pText[i];
			if(pText[i]=='\0')i=640000+1; //‹­ˆø‚Éƒ‹[ƒvI—¹
		}

		GlobalUnlock(hText);
	}

	CloseClipboard();
	ReadStartFromVirtualCB();
}

RECT rect1 = {0,0,WWidth,WHeight};//XV‚·‚é—Ìˆæ

void ReplaseUndo()
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(org_data.ReplaceFromUndoData()>0){ //‚±‚êˆÈãUNDOo—ˆ‚È‚¢
		EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_GRAYED);
	}
	//org_data.PutBackGround();
	//org_data.PutMusic();	//•\¦
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//’Êí‚Ìó‘Ô‚É–ß‚·‚É‚Í
	EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_ENABLED);
	DrawMenuBar(hWnd);//ƒƒjƒ…[‚ğÄ•`‰æ
	if(org_data.MinimumUndoCursor==0 && org_data.CurrentUndoCursor==0){
		SetModified(false);
	}else{
		SetModified(true);
	}
}

void SetUndo()
{
	if(org_data.SetUndoData()>0){ //ƒZƒbƒg‚µA‚à‚µAƒƒjƒ…[‚ªŠDF•\¦‚È‚ç”Z‚­‚·‚é
		HMENU hMenu;
		hMenu=GetMenu(hWnd);
		//’Êí‚Ìó‘Ô‚É–ß‚·‚É‚Í
		EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
		DrawMenuBar(hWnd);//ƒƒjƒ…[‚ğÄ•`‰æ
	}
	SetModified(true);
}

void ResetLastUndo() //æ‚è‚¯‚µ
{
	if(org_data.ResetLastUndo()>0){ //ƒZƒbƒg‚µA‚à‚µAƒƒjƒ…[‚ªŠDF•\¦‚È‚ç”Z‚­‚·‚é
		HMENU hMenu;
		hMenu=GetMenu(hWnd);
		//’Êí‚Ìó‘Ô‚É–ß‚·‚É‚Í
		EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
		DrawMenuBar(hWnd);//ƒƒjƒ…[‚ğÄ•`‰æ
	}

}

void ClearUndo()
{
	org_data.ClearUndoData();
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
	//’Êí‚Ìó‘Ô‚É–ß‚·‚É‚Í
	//EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_ENABLED);
	DrawMenuBar(hWnd);//ƒƒjƒ…[‚ğÄ•`‰æ

}

void ReplaceRedo()
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(org_data.ReplaceFromRedoData()>0){
		//’Êí‚Ìó‘Ô‚É–ß‚·‚É‚Í
		EnableMenuItem(hMenu,IDM_REDO,MF_BYCOMMAND|MF_GRAYED);
	}
	EnableMenuItem(hMenu,IDM_UNDO,MF_BYCOMMAND|MF_ENABLED);
	DrawMenuBar(hWnd);//ƒƒjƒ…[‚ğÄ•`‰æ
	//org_data.PutBackGround();
	//org_data.PutMusic();	//•\¦
	//RedrawWindow(hWnd,&rect1,NULL,RDW_INVALIDATE|RDW_ERASENOW);

}

void ChangeGridMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)sGrid=iValue;
	else{
		sGrid = 1 - sGrid;
	}
	if(sGrid==0)
		CheckMenuItem(hMenu,IDM_GRIDMODE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_GRIDMODE,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}

void ChangeMetronomeMode(int iValue)
{
	HMENU hMenu;
	hMenu = GetMenu(hWnd);
	if (iValue != -1)sMetronome = iValue;
	else {
		sMetronome = 1 - sMetronome;
	}
	if (sMetronome == 0)
		CheckMenuItem(hMenu, IDM_METRONOME, (MF_BYCOMMAND | MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu, IDM_METRONOME, (MF_BYCOMMAND | MFS_CHECKED));
	ShowStatusMessage();
}

void ChangeScrollMode(int iValue)
{
	HMENU hMenu;
	hMenu = GetMenu(hWnd);
	if (iValue != -1)sSmoothScroll = iValue;
	else {
		sSmoothScroll = 1 - sSmoothScroll;
	}
	if (sSmoothScroll == 0)
		CheckMenuItem(hMenu, IDM_SMOOTHSCROLL, (MF_BYCOMMAND | MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu, IDM_SMOOTHSCROLL, (MF_BYCOMMAND | MFS_CHECKED));
	ShowStatusMessage();
}

//2010.09.23 A
void ChangeEnablePlaying(int iValue){
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iChangeEnablePlaying=iValue;
	else{
		iChangeEnablePlaying = 1 - iChangeEnablePlaying;
	}
	if(iChangeEnablePlaying==0)
		CheckMenuItem(hMenu,IDM_ENABLEPLAYING,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_ENABLEPLAYING,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}
//2010.09.23 A
void ChangeFinish(int iValue){
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iChangeFinish=iValue;
	else{
		iChangeFinish = 1 - iChangeFinish;
	}
	if(iChangeFinish==0)
		CheckMenuItem(hMenu,IDM_CHANGEFINISH,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_CHANGEFINISH,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}

//2010.09.23 A
void ChangeNoteEnlarge(int iValue){
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)NoteEnlarge_Until_16px = iValue;
	else{
		NoteEnlarge_Until_16px = 1 - NoteEnlarge_Until_16px;
	}
	if(NoteEnlarge_Until_16px == 0)
		CheckMenuItem(hMenu,IDM_NOTE_ENLARGE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_NOTE_ENLARGE,(MF_BYCOMMAND|MFS_CHECKED));
	//ShowStatusMessage();
}

int MinimumGrid(int x)
{
	int r;	//line ‰½”‘‚©  // dot ‚P”‘‚Ì•ªŠ„”
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	r = x - (x % dot);
	return r;

}
int MaximumGrid(int x)
{
	int r;	//line ‰½”‘‚©  // dot ‚P”‘‚Ì•ªŠ„”
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot;
	r = x - (x % dot) + dot-1;
	return r;
}

int MinimumGridLine(int x)
{
	int r;	//line ‰½”‘‚©  // dot ‚P”‘‚Ì•ªŠ„”
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot * mi.line;
	r = x - (x % dot);
	return r;

}
int MaximumGridLine(int x)
{
	int r;	//line ‰½”‘‚©  // dot ‚P”‘‚Ì•ªŠ„”
	unsigned char dot;
	MUSICINFO mi;	org_data.GetMusicInfo(&mi);
	dot = mi.dot * mi.line;
	r = x - (x % dot) + dot-1;
	return r;
}

void ChangeSelAlwaysCurrent(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)sACrnt=iValue;
	else{
		sACrnt = 1 - sACrnt;
	}
	if(sACrnt==0)
		CheckMenuItem(hMenu,IDM_ALWAYS_CURRENT,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_ALWAYS_CURRENT,(MF_BYCOMMAND|MFS_CHECKED));

	ShowStatusMessage();

}

void ChangeDrawDouble(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)gDrawDouble=iValue;
	else{
		gDrawDouble = 1 - gDrawDouble;
	}
	if(gDrawDouble==0)
		CheckMenuItem(hMenu,IDM_DRAWDOUBLE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_DRAWDOUBLE,(MF_BYCOMMAND|MFS_CHECKED));
		//ModifyMenu(hMenu, IDM_DRAWDOUBLE, MF_BYCOMMAND|MF_STRING, IDM_DRAWDOUBLE, "‚¨‚¦");
	//org_data.PutMusic();

	ShowStatusMessage();
}

void ChangeDragMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iDragMode=iValue;
	else{
		iDragMode = 1 - iDragMode;
	}
	if(iDragMode==0)
		CheckMenuItem(hMenu,IDM_DRAGMODE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_DRAGMODE,(MF_BYCOMMAND|MFS_CHECKED));
	//org_data.PutMusic();
	ShowStatusMessage();

}

void ChangeSlideOverlapNoteMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iSlideOverlapNotes=iValue;
	else{
		iSlideOverlapNotes = 1 - iSlideOverlapNotes;
	}
	if(iSlideOverlapNotes==0)
		CheckMenuItem(hMenu,IDM_SLIDEOVERLAPNOTES,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_SLIDEOVERLAPNOTES,(MF_BYCOMMAND|MFS_CHECKED));
	//org_data.PutMusic();
	ShowStatusMessage();

}

void ChangePushStratchNOTE(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)iPushStratch=iValue;
	else{
		iPushStratch = 1 - iPushStratch;
	}
	if(iPushStratch==0)
		CheckMenuItem(hMenu,IDM_PRESSNOTE,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_PRESSNOTE,(MF_BYCOMMAND|MFS_CHECKED));
	//org_data.PutMusic();
	ShowStatusMessage();

}

bool AutoLoadPVIFile()
{
	//2014.05.06 A
	//2014.05.30 M
	char *PVIFile;
	PVIFile = new char[MAX_PATH];
	strcpy(PVIFile, music_file);
	int ml = strlen(PVIFile);
	if(ml>4){
		if(PVIFile[ml-4] == '.'){
			strcpy(PVIFile+ml-4, ".pvi");
		}
	}else{
		delete [] PVIFile;
		return false;
	}
	FILE *fp;
	fp = fopen(PVIFile, "rt");
	if(fp==NULL){
		delete [] PVIFile;
		return false;
	}
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		fscanf(fp,"%d",&r);
		org_data.def_pan[t] = (unsigned char)r;
		fscanf(fp,"%d",&r);
		org_data.def_volume[t] = (unsigned char)r;
	}
	fclose(fp);
	delete [] PVIFile;
	return true;
}

bool AutoSavePVIFile()
{
	//2014.05.06 A
	char *PVIFile;
	PVIFile = new char[MAX_PATH];
	strcpy(PVIFile, music_file);
	int ml = strlen(PVIFile);
	if(ml>4){
		if(PVIFile[ml-4] == '.'){
			strcpy(PVIFile+ml-4, ".pvi");
		}
	}else{
		delete [] PVIFile;
		return false;
	}
	FILE *fp;
	fp = fopen(PVIFile, "wt");
	if(fp==NULL){
		delete [] PVIFile;
		return false;
	}
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		r = (int)org_data.def_pan[t];
		fprintf(fp,"%d\n",r);
		r = (int)org_data.def_volume[t];
		fprintf(fp,"%d\n",r);
	}
	fclose(fp);
	delete [] PVIFile;
	return true;
}

void ChangeAutoLoadMode(int iValue)
{
	HMENU hMenu;
	hMenu=GetMenu(hWnd);
	if(iValue!=-1)SaveWithInitVolFile=iValue;
	else{
		SaveWithInitVolFile = 1 - SaveWithInitVolFile;
	}
	if(SaveWithInitVolFile==0)
		CheckMenuItem(hMenu,IDM_AUTOLOADPVI,(MF_BYCOMMAND|MFS_UNCHECKED));
	else
		CheckMenuItem(hMenu,IDM_AUTOLOADPVI,(MF_BYCOMMAND|MFS_CHECKED));
	ShowStatusMessage();
}

// StringTable‚ğQÆ‚·‚éƒƒbƒZ[ƒWƒ{ƒbƒNƒX //2014.10.18 
int msgbox(HWND hWnd , int MessageID, int TitleID, UINT uType)
{
	TCHAR strMesssage[2048];
	TCHAR strTitle[1024];
	LoadString(GetModuleHandle(NULL), MessageID, strMesssage, 2048);
	LoadString(GetModuleHandle(NULL), TitleID  , strTitle   , 1024);
	return MessageBox(hWnd, strMesssage, strTitle, uType);
}

void FreeMessageStringBuffer(void)
{
	free(MessageStringBuffer);
}

int AllocMessageStringBuffer(void)
{
	int i, r, flg;
	TCHAR *ptr, *p;
	if(MessageStringBuffer == NULL){
		MessageStringBuffer = (TCHAR *)calloc(MESSAGE_STRING_BUFFER_SIZE, sizeof(TCHAR));
	}
	for(i = 0; i < MESSAGE_STRING_MAX; i++){
		MessageString[i] = NULL;
	}
	ptr = MessageStringBuffer;
	for(i = 1; i < MESSAGE_STRING_MAX; i++){
		MessageString[i] = ptr;
		r = LoadString(GetModuleHandle(NULL), i, ptr, 1024); //1024‚Í“K“–‚Å‚·B³Šm‚É‚ÍMESSAGE_STRING_BUFFER_SIZE‚©‚çŒvZ‚·‚é•K—v‚ª‚ ‚è‚Ü‚·B
		if(r > 0){
			//––”ö‚Ì!!‚ğŒŸo‚·‚é
			for(p = ptr + r - 3, flg = 0; *p != 0 ; p++){
				if(*p == '!')flg++; else flg = 0;
				if(flg >= 2)break;
			}
			//!!‚Ì‚Æ‚«A“ª‚©‚ç!‚ğ\0‚É’uŠ·
			if(flg == 2){
				for(p = ptr ; *p != 0 ; p++)if(*p == '!')*p = 0;
			}
			//ƒ|ƒCƒ“ƒ^ˆÚ“®
			ptr += (r + 1 + 1); //+ 1‚ÍNULL‚Ì•ªA‚à‚¤+1‚Í—\”õB
		}else{ //´×°‚Ìê‡(‘¶İ‚µ‚È‚¢‚Æ‚©)
			break; //”²‚¯‚é
		}
	}

	//lpstrFilter‚É‚Â‚¢‚Ä‚Í"!"‚ğ\0‚É•ÏŠ·‚·‚é•K—vƒAƒŠB
	//for(ptr = MessageString[108]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	//for(ptr = MessageString[109]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	//for(ptr = MessageString[110]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	//for(ptr = MessageString[111]; *ptr != 0; ptr++)if(*ptr == '!')*ptr = 0;
	return 0;
}

