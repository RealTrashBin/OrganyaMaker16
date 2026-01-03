#include "Setting.h"
#include "OrgData.h"
#include "DefOrg.h"
#include "TrackFlag.h"
#include <math.h>

float NewMelody = MAXMELODY / 2;
float NewDram = MAXDRAM / 2;


bool OrgData::TrackFlag(void) //Used for ORG-16
{
	int i; //Track
	bool tf; //Track Flag
	tf = false;

	round(NewMelody);
	round(NewDram);

	//Checks Melody
	for (i = NewMelody; i < MAXMELODY; i++) //i = Last STD Track, i < maxmium track
	{
		org_dat.tdata[i].note_num = GetNoteNumber(i, NULL);
		if (org_dat.tdata[i].note_num == NULL)
			continue;
		else
		{
			tf = true;
			break;
		}
	}

	if (tf == false) 
		//Checks Drams
		for (i = NewDram; i < MAXTRACK; i++)
		{
			org_dat.tdata[i].note_num = GetNoteNumber(i, NULL);
			if (org_dat.tdata[i].note_num == NULL)
				continue;
			else
			{
				tf = true;
				break;
			}
		}
	return tf;
}