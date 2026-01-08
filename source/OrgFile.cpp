#include <string.h>
#include "Setting.h"
#include "OrgData.h"
#include <stdio.h>
#include "Sound.h"
#include "resource.h"
#include "Scroll.h"
#include "rxoFunction.h"

//ORGMAKER Checks these in files, don't mess with them.
char music_file[MAX_PATH]; //NewData.org
char pass[7] = "Org-01"; //Base Format
char pass2[7] = "Org-02";//Pipi
char pass3[7] = "Org-03";//蒐ew Drams

char pass4[7] = "Org-16";//16 Tracks

//Important Structures for how ORGMAKER works.
typedef struct {
	long x;//What step the note is on.
	unsigned char y;//What key is the note on.
	unsigned char length;//how long is the note.
	unsigned char volume;//volume of note, max is 255.
	unsigned char pan;//The pan, 12 for right, 6 for center and 0 for left.
}ORGANYANOTE; //Structure of Notes
typedef struct {
	unsigned short freq;//Chours of note(1000 = Default)
	unsigned char wave_no;//The Waveform the TRACK is using
	unsigned char pipi;//》data[i].pipi = 1; pipi is being used.
	unsigned short note_num;//Number of notes.
}ORGANYATRACK; //Structure of Tracks and Drams
typedef struct {
	signed short wait; //Speed of ORG
	unsigned char line; //Beat (For time signature)
	unsigned char dot; //Step (For time signature)
	long repeat_x;//Beginning of repeat segment.
	long end_x;//End of repeat segment.
	ORGANYATRACK tdata[MAXTRACK]; //TRACKS
}ORGANYADATA; //Structure of an ORG File
ORGANYADATA orgdat;
//Gets the amount of Notes and can copy them?
unsigned short OrgData::GetNoteNumber(char track,NOTECOPY *nc)
{
	NOTELIST *np;
	unsigned short num = 0;
	np = info.tdata[track].note_list;//Gets amount of notes in one track
	if(nc == NULL){
		while(np != NULL){
			num++;
			np = np->to;
		}
		return num;
	}else{
		//Copies the notes to whatever beat they need to go to
		while(np != NULL && np->x < nc->x1_1){
			np = np->to;
		}
		if (np == NULL) //No notes in the track
		{
			return 0;
		}
		//Gets the amount of notes?
		while(np != NULL && np->x <= nc->x1_2){
			num++;
			np = np->to;
		}
		return num;
	}
}



//just saves the music data.
BOOL OrgData::SaveMusicData(void)
{
	NOTELIST *np;
	int i,j; // i is TRACK
	bool pf;
	//ORG Structure to save
	orgdat.wait = info.wait;
	orgdat.line = info.line;
	orgdat.dot = info.dot;
	orgdat.repeat_x = info.repeat_x;
	orgdat.end_x = info.end_x;
	
	//Loop that writes the needed TRACK data
	for(i = 0; i < MAXTRACK; i++){
		orgdat.tdata[i].freq = info.tdata[i].freq;
		orgdat.tdata[i].wave_no = info.tdata[i].wave_no;
		orgdat.tdata[i].pipi = info.tdata[i].pipi;
		orgdat.tdata[i].note_num = GetNoteNumber(i,NULL);
	} //Saves Notes
	//Can't write to file error
	FILE *fp;
	if((fp=fopen(music_file,"wb"))==NULL){
		MessageBox(hWnd,"Can't write to ORG file.", "Saving Error", MB_OK);
		return(FALSE);
	}
	j = 1;//j is the file version in this instance.
	pf = false;//Pipi Flag
	for (i = 0; i < MAXMELODY; i++) //pipi file checker
	{
		if (orgdat.tdata[i].pipi == 0)
		{
			j = 1;
		}
		else
		{
			pf = true;
		}
		if (pf == true)
		{
			j = 2;
		}
	}
	for(i=MAXTRACK / 2;i<MAXTRACK;i++) //Checks for new Drams.
	{
		if (orgdat.tdata[i].wave_no >= 13)
		{
			j = 3;
		}
	}

	TrackFlag(); //Checks if new channels are used.
	if (TrackFlag())
	{
		j = 4;
	}

	if(j==1)fwrite(&pass[0], sizeof(char), 6, fp);
	else if(j==2)fwrite(&pass2[0], sizeof(char), 6, fp);//version 2, write version 2.
	else if(j==3)fwrite(&pass3[0], sizeof(char), 6, fp);//version 3, write version 3.
	else fwrite(&pass4[0], sizeof(char), 6, fp);

	//Writes the Struct of an ORG's data.
		fwrite(&orgdat, sizeof(ORGANYADATA), 1, fp);

	//Writes the TRACK's data, j is the TRACK in this instance.
	if (j == 4)
	{
		for (j = 0; j < MAXTRACK; j++) {
			if (info.tdata[j].note_list == NULL)continue;//If there's no notes, continue to the next TRACK.
			np = info.tdata[j].note_list;//Steps/Beats notes are on.
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->x, sizeof(long), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;//Keys notes are on.
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->y, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;//Length of notes
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->length, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;//Volume of notes
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->volume, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;//Panning of notes
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->pan, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
		}
	}
	else
	{ //Yes, I did just copy and paste the code from above.
		for (j = 0; j < 8; j++) { //Vanilla ORG Melody save.
			if (info.tdata[j].note_list == NULL)continue;
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->x, sizeof(long), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->y, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->length, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->volume, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->pan, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
		}
		for (j = 16; j < 24; j++) { //Vanilla ORG Dram save.
			if (info.tdata[j].note_list == NULL)continue;
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->x, sizeof(long), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->y, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->length, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->volume, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
			np = info.tdata[j].note_list;
			for (i = 0; i < orgdat.tdata[j].note_num; i++) {
				fwrite(&np->pan, sizeof(unsigned char), 1, fp);
				np = np->to;
			}
		}
	}
	fclose(fp);
	PutRecentFile(music_file); //Puts it the most recent on the recent list.
//	MessageBox(hWnd,"保存しました","Message (Save)",MB_OK);
	if(SaveWithInitVolFile != 0){
		AutoSavePVIFile();
	}

	return TRUE;
}


extern char *dram_name[];
extern HWND hDlgPlayer;

//checks if the file exists?
int OrgData::FileCheckBeforeLoad(char *checkfile)
{
	FILE *fp;
	char pass_check[6];
	char ver = 0;
	if((fp=fopen(checkfile,"rb"))==NULL){
		MessageBox(hWnd,"File Doesn't exist.","Loading Error",MB_OK);
		return 1; //Sorry Strultz, I had to make it where ur little pop up windows don't pop up.
	}

	fread(&pass_check[0], sizeof(char), 6, fp);
	if( !memcmp( pass_check, pass, 6 ) )ver = 1;
	if( !memcmp( pass_check, pass2, 6 ) )ver = 2;
	if( !memcmp( pass_check, pass3, 6 ) )ver = 3;
	if (!memcmp(pass_check, pass4, 6))ver = 4;
	if( !ver ){
		fclose(fp);
		MessageBox(hWnd,"Not a proper ORG file.","Loading Error",MB_OK);
		return 1; //1
	}
	fclose(fp);

	return 0;
}

BOOL OrgData::LoadMusicData(void)
{
	ORGANYADATA orgdat;
	NOTELIST* np;
	NOTECOPY nc;
	MUSICINFO mi;
	int i, j;
	char pass_check[6];
	char ver = 0;
	bool cflag = false;

	//｣｣｣｣｣｣｣｣｣｣｣｣｣｣｣ロード
	FILE* fp;
	if ((fp = fopen(music_file, "rb")) == NULL) {
		MessageBox(hWnd, "Couldn't access file.", "Error (Load)", MB_OK);
		return(FALSE);
	}
	//パスワードチェック
	fread(&pass_check[0], sizeof(char), 6, fp);
	if (!memcmp(pass_check, pass, 6))ver = 1;
	if (!memcmp(pass_check, pass2, 6))ver = 2;
	if (!memcmp(pass_check, pass3, 6))ver = 3;
	if (!memcmp(pass_check, pass4, 6))ver = 4;
	if (!ver) {
		fclose(fp);
		MessageBox(hWnd, "Not a proper Version.", "Error(Load)", MB_OK);
		return FALSE;
	}

	//Open the ORG file.
	fread(&orgdat, sizeof(ORGANYADATA), 1, fp);

	//Reads the struct of an ORG file
	info.wait = orgdat.wait;
	info.line = orgdat.line;
	info.dot = orgdat.dot;
	info.repeat_x = orgdat.repeat_x;
	info.end_x = orgdat.end_x;
	for (i = 0; i < MAXTRACK; i++) {
		info.tdata[i].freq = orgdat.tdata[i].freq;
		if (ver == 1)info.tdata[i].pipi = 0;
		else info.tdata[i].pipi = orgdat.tdata[i].pipi;
		info.tdata[i].wave_no = orgdat.tdata[i].wave_no;
	}

	//Loads in TRACKS
	for (j = 0; j < MAXTRACK; j++) {
		//If no notes, continue.
		if (orgdat.tdata[j].note_num == 0) {
			info.tdata[j].note_list = NULL;
			continue;
		}
		if (orgdat.tdata[j].note_num > info.alloc_note) {
			if (!cflag) {
				MessageBox(NULL, "Some tracks were shortened due to having too many notes.", "Notice", MB_OK | MB_ICONASTERISK);
				cflag = true;
			}
		}
		//リストを作る
		np = info.tdata[j].note_p;
		info.tdata[j].note_list = info.tdata[j].note_p;
		np->from = NULL;
		np->to = (np + 1);
		np++;
		for (i = 1; i < orgdat.tdata[j].note_num && i < info.alloc_note; i++) {
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}
		//最後の音符のtoはNULL
		np--;
		np->to = NULL;

		//Placement of notes.
		np = info.tdata[j].note_p;//Ｘ座標
		for (i = 0; i < orgdat.tdata[j].note_num; i++) {
			if (i >= info.alloc_note) { fseek(fp, sizeof(long), SEEK_CUR); continue; }
			fread(&np->x, sizeof(long), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//Ｙ座標
		for (i = 0; i < orgdat.tdata[j].note_num; i++) {
			if (i >= info.alloc_note) { fseek(fp, sizeof(unsigned char), SEEK_CUR); continue; }
			fread(&np->y, sizeof(unsigned char), 1, fp);
			if (np->y >= 96) np->y = KEYDUMMY; //Places the notes on their Y/Key
			np++;
		}
		np = info.tdata[j].note_p;//長さ
		for (i = 0; i < orgdat.tdata[j].note_num; i++) {
			if (i >= info.alloc_note) { fseek(fp, sizeof(unsigned char), SEEK_CUR); continue; }
			fread(&np->length, sizeof(unsigned char), 1, fp);
			if (np->length == 0) np->length = 1; // Length of note.
			np++;
		}
		np = info.tdata[j].note_p;//Volume
		for (i = 0; i < orgdat.tdata[j].note_num; i++) {
			if (i >= info.alloc_note) { fseek(fp, sizeof(unsigned char), SEEK_CUR); continue; }
			fread(&np->volume, sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//Panning
		for (i = 0; i < orgdat.tdata[j].note_num; i++) {
			if (i >= info.alloc_note) { fseek(fp, sizeof(unsigned char), SEEK_CUR); continue; }
			fread(&np->pan, sizeof(unsigned char), 1, fp);
			np++;
		}
	}
	fclose(fp);
	//データを有効に
	for (j = 0; j < MAXMELODY; j++)
	{
		MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);
	}
	for (j = MAXMELODY; j < MAXTRACK; j++) {
		i = info.tdata[j].wave_no;
		InitDramObject(i, j - MAXMELODY);
	}

	//プレイヤーに表示
	char str[32];
	SetPlayPointer(0);//頭出し
	scr_data.SetHorzScroll(0);
	itoa(info.wait,str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWWAIT,str);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,"0");
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,"0");

	scr_data.ChangeHorizontalRange(info.dot * info.line * MAXHORZMEAS);
	//MakeMusicParts(info.line,info.dot);//パーツを生成
	//MakePanParts(info.line,info.dot);
	PutRecentFile(music_file);
	//↓2014.05.06 A
	if(SaveWithInitVolFile != 0){
		AutoLoadPVIFile();
	}
	return TRUE;
}

void OrgData::SortNotes()
{
	NOTELIST *pntl,*pNtls,*np;
	ORGANYADATA orgdat;
	int i,j;

	orgdat.wait = info.wait;
	orgdat.line = info.line;
	orgdat.dot = info.dot;
	orgdat.repeat_x = info.repeat_x;
	orgdat.end_x = info.end_x;

	for(i = 0; i < MAXTRACK; i++){
		orgdat.tdata[i].freq = info.tdata[i].freq;
		orgdat.tdata[i].wave_no = info.tdata[i].wave_no;
		orgdat.tdata[i].pipi = info.tdata[i].pipi;
		orgdat.tdata[i].note_num = GetNoteNumber(i,NULL);
	}

	pNtls = new NOTELIST[ALLOCNOTE]; //Memory Allocation for each TRACK.
	
	for(j = 0; j < MAXTRACK; j++){
		if(info.tdata[j].note_list == NULL)continue;
		pntl = pNtls;
		np = info.tdata[j].note_list;//音符の先頭
		for(i = 0; i < orgdat.tdata[j].note_num; i++){
			pNtls[i].x = np->x;
			pNtls[i].y = np->y;
			pNtls[i].pan = np->pan;
			pNtls[i].volume = np->volume;
			pNtls[i].length = np->length;
			np = np->to;
		}
		//リストを作る
		if(true){
			np = info.tdata[j].note_p; //領域の先頭
			info.tdata[j].note_list = info.tdata[j].note_p;
			np->from = NULL;
			np->to = (np + 1);
			np++;
			for(i = 1; i < orgdat.tdata[j].note_num; i++){
				np->from = (np - 1);
				np->to = (np + 1);
				np++;
			}
			//最後の音符のtoはNULL
			np--;
			np->to = NULL;
			np = info.tdata[j].note_p;//Ｘ座標
			for(i = 0; i < orgdat.tdata[j].note_num; i++){
				np->x = pNtls[i].x;
				np->y = pNtls[i].y;
				np->length = pNtls[i].length;
				np->pan = pNtls[i].pan;
				np->volume = pNtls[i].volume;
				np++;
			}
		}else{ //実験的にサカシにする
			np = info.tdata[j].note_p+4095; //領域の先頭
			info.tdata[j].note_list = info.tdata[j].note_p;
			np->from = NULL;
			np->to = (np - 1);
			np--;
			for(i = 1; i < orgdat.tdata[j].note_num; i++){
				np->from = (np + 1);
				np->to = (np - 1);
				np--;
			}
			//最後の音符のtoはNULL
			np++;
			np->to = NULL;
			np = info.tdata[j].note_p;//Ｘ座標
			for(i = 0; i < orgdat.tdata[j].note_num; i++){
				np->x = pNtls[i].x;
				np->y = pNtls[i].y;
				np->length = pNtls[i].length;
				np->pan = pNtls[i].pan;
				np->volume = pNtls[i].volume;
				np++;
			}

		}
	}	
	delete [] pNtls;	// 2014.10.18 解放を追加した。
	//プレイヤーに表示
	char str[32];
	SetPlayPointer(0);//頭出し
	scr_data.SetHorzScroll(0);
	itoa(info.wait,str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWWAIT,str);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,"0");
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,"0");

	//MakeMusicParts(info.line,info.dot);//パーツを生成
	//MakePanParts(info.line,info.dot);

}
