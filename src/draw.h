#ifndef draw_h
#define draw_h

#include <pd_api.h>
#include "util.h"
#include "constants.h"

struct Plot {
	int xdraw;
	int ydraw;
	int xorigin;
	int yorigin;
	int xrcorner;
	int yrcorner;
	int width;
	int height;
	float xmin;
	float xmax;
	float ymin;
	float ymax;
	float xconv;
	float yconv;
	char xlabels[NUMXTICKS+1][STRLEN];
	char ylabels[NUMYTICKS+1][STRLEN];

};

void printLatLine( PlaydateAPI* pd, float lat, float temp, int x, int y );
void printAllLatLines( PlaydateAPI* pd, float lat[], float temp[], int size, int x, int y );
void printFloat( PlaydateAPI* pd, int x, int y, float flt, int prec );
void printInt( PlaydateAPI* pd, int x, int y, int num, int prec );
void createPlot( struct Plot* plt, int x, int y, int width, int height );
void drawPlot( PlaydateAPI* pd, struct Plot plt );
void setXlimits( struct Plot* plt, float xmin, float xmax );
void setYlimits( struct Plot* plt, float ymin, float ymax );
float xCoord( struct Plot plt, float x );
float yCoord( struct Plot plt, float y );
void plotLine( PlaydateAPI* pd, struct Plot plt, float x1, float y1, float x2, float y2 );
void plotArray( PlaydateAPI* pd, struct Plot plt, float xdata[], float ydata[], int size );
void plotMarker( PlaydateAPI* pd, struct Plot plt, float xdata[], float ydata[], int index );
void addXAxisLabels( struct Plot* plt, char label[][STRLEN], int size );
void addYAxisLabels( struct Plot* plt, char label[][STRLEN], int size );
void batteryPercentString( PlaydateAPI* pd, char *out );
void drawVarFloat( PlaydateAPI* pd, char* name, float value, int x, int y );
void drawVarInt( PlaydateAPI* pd, char* name, int value, int x, int y );

#endif

