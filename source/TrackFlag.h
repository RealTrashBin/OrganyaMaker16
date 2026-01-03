#include "Gdi.h"
#include "DefOrg.h"

typedef struct {
	unsigned short note_num;//Number of notes.
}ORGANYATRACKS; //Structure of Tracks and Drams

typedef struct {
	ORGANYATRACKS tdata[MAXTRACK]; //TRACKS
}ORGANYADAT; //Structure of an ORG File
ORGANYADAT org_dat;
