#include "Setting.h"
#include "OrgData.h"
#include "DefOrg.h"
#include "TrackFlag.h"

char NewMelody = MAXMELODY / 2;
char NewDram = MAXTRACK - MAXDRAM / 2;

bool OrgData::TrackFlag(void) //Used for ORG-16
{
	char i; //Track

	//Checks Melody
	for (i = NewMelody; i < MAXMELODY; i++) //i = Last STD Track, i < maxmium track
	{
		org_dat.tdata[i].note_num = GetNoteNumber(i, NULL);
		if (org_dat.tdata[i].note_num == NULL)
			continue;
		else
		{
			return true;
		}
	}
		//Checks Drams
		for (i = NewDram; i < MAXTRACK; i++)
		{
			org_dat.tdata[i].note_num = GetNoteNumber(i, NULL);
			if (org_dat.tdata[i].note_num == NULL)
				continue;
			else
			{
				return true;
			}
		}
	return false;
}

