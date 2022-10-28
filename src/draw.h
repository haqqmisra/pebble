#ifndef draw_h
#define draw_h

#include <pd_api.h>
#include "util.h"

void printLatLine( PlaydateAPI* pd, float lat, float xlat, float temp, int x, int y );
void printAllLatLines( PlaydateAPI* pd, float lat[], float xlat[], float temp[], int size, int x, int y );
void printRuntime( PlaydateAPI* pd, int x, int y );

#endif

