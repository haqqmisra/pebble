#ifndef draw_h
#define draw_h

#include <pd_api.h>
#include "util.h"

#define NUMXTICKS 6
#define NUMYTICKS 4

struct Plot {
	int xdraw;
	int ydraw;
	int xorigin;
	int yorigin;
	int width;
	int height;
	char xlabels[NUMXTICKS+1][144];
	char ylabels[NUMYTICKS+1][144];
};

void printLatLine( PlaydateAPI* pd, float lat, float temp, int x, int y );
void printAllLatLines( PlaydateAPI* pd, float lat[], float temp[], int size, int x, int y );
void printFloat( PlaydateAPI* pd, int x, int y, float flt, int prec );
void printInt( PlaydateAPI* pd, int x, int y, int num, int prec );
void createPlot( struct Plot* plt, int x, int y, int width, int height );
void drawPlot( PlaydateAPI* pd, struct Plot plt );

#endif

