#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "model.h"
#include "util.h"
#include "draw.h"
#include "io.h"
#include "constants.h"

#define NUMSTATUS 6
#define NUMDISPLAY 3

typedef enum { initializing, ready, running, paused, crash, done } Status;
typedef enum { annual, daily, configure } Display;

static int update( void* userdata );
void reset( void* userdata );
void changeState( Status newstate );
void changeScreen( Display newscreen );
void freeMemory( PlaydateAPI* pd );

#endif
