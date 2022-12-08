#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "model.h"
#include "draw.h"
#include "io.h"
#include "json.h"
#include "constants.h"

#define NUMSTATUS 6
#define NUMDISPLAY 3

const char* fontpath = "fonts/font-rains-1x.pft";

typedef enum { initializing, ready, running, paused, crash, done } Status;
typedef enum { annual, daily, configure } Display;
typedef enum { Kelvin, Celsius, Fahrenheit } Units;

static int update( void* userdata );
void reset( void* userdata );
void updateUnits( void* userdata );
void changeState( Status newstate );
void changeScreen( Display newscreen );
void freeMemory( PlaydateAPI* pd );

#endif
