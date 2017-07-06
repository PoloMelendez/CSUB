/*===============================================================================================
These functions were derived from the FMOD library.
It is copyrighted material, but free for personal use.
Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.
See other sample programs in the fmod /examples directory
===============================================================================================*/
//The following paths are consistent with the fmod download/example paths.
//They can be changed if you place the header and library files
//in another area.
//For instance, directories you already have established
//for your personal project.
//
//#include "../../api/inc/fmod.h"
//#include "../../api/inc/fmod_errors.h"
#include <FMOD/fmod.h>
#include <FMOD/fmod_errors.h>
#include <stdio.h>

//local global variables are defined here
#define MAX_SOUNDS 32
FMOD_SYSTEM  *xsystem;
FMOD_SOUND   *sound[MAX_SOUNDS];
FMOD_CHANNEL *channel = 0;
static int nsounds=0;

int ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return 1;
	}
	return 0;
}

int fmod_init(void)
{
	FMOD_RESULT result;
    unsigned int version;
	//printf("fmod_init()...\n");
	result = FMOD_System_Create(&xsystem);
	if (ERRCHECK(result)) return 1;
	result = FMOD_System_GetVersion(xsystem, &version);
	if (ERRCHECK(result)) return 1;
	if (version < FMOD_VERSION) {
		printf("Error!  Old version of FMOD %08x.\n", version);
		printf("This program requires %08x\n", FMOD_VERSION);
		return 1;
	}
	result = FMOD_System_Init(xsystem, 32, FMOD_INIT_NORMAL, NULL);
	if (ERRCHECK(result)) return 1;
	return 0;
}

int fmod_createsound(char *fname, int i)
{
	FMOD_RESULT result;
	//printf("fmod_createsound(**%s**, %i)...\n",fname,i);
	if (nsounds >= MAX_SOUNDS)
		return 0;
	result = FMOD_System_CreateSound(xsystem, fname, FMOD_SOFTWARE, 0, &sound[i]);
	if (ERRCHECK(result)) {
		printf("error fmod_createsound()\n");
		return 1;
	}
	++nsounds;
	return 0;
}

int fmod_playsound(int i)
{
	FMOD_RESULT result;
	//printf("fmod_playsound(%i)...\n",i);
	result = FMOD_System_PlaySound(xsystem, FMOD_CHANNEL_FREE, sound[i], 0, &channel);
	if (ERRCHECK(result)) {
		printf("error fmod_playsound()\n");
		return 1;
	}
	return 0;
}

int fmod_setmode(int i, int mode)
{
	FMOD_RESULT result;
	//printf("fmod_setmode()...\n");
	//result = FMOD_Sound_SetMode(sound[i], FMOD_LOOP_OFF);
	result = FMOD_Sound_SetMode(sound[i], mode);
	if (ERRCHECK(result)) {
		printf("error fmod_setmode()\n");
		return 1;
	}
	return 0;
}

int fmod_getlength(int i, unsigned int *lenms)
{
	FMOD_RESULT result;
	result = FMOD_Sound_GetLength(sound[i], lenms, FMOD_TIMEUNIT_MS);
	if (ERRCHECK(result)) {
		printf("error fmod_getlength()\n");
		return 1;
	}
	return 0;
}

int fmod_systemupdate(void)
{
	FMOD_System_Update(xsystem);
	return 0;
}

int fmod_getchannelsplaying(int *channelsplaying)
{
	FMOD_System_GetChannelsPlaying(xsystem, channelsplaying);
	return 0;
}

int fmod_cleanup(void)
{
	int i;
	FMOD_RESULT result;
	for (i=0; i<nsounds; i++) {
		result = FMOD_Sound_Release(sound[i]);
		if (ERRCHECK(result))
			return 1;
	}
	result = FMOD_System_Close(xsystem);
	if (ERRCHECK(result)) return 1;
	result = FMOD_System_Release(xsystem);
	if (ERRCHECK(result)) return 1;
	return 0;
}

