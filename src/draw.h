#ifndef draw_h
#define draw_h

#include <pd_api.h>
#include "constants.h"

typedef struct {
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

} Plot;

void createPlot( Plot* plt, int x, int y, int width, int height );
void drawPlot( PlaydateAPI* pd, Plot plt );
void setXlimits( Plot* plt, float xmin, float xmax );
void setYlimits( Plot* plt, float ymin, float ymax );
float xCoord( Plot plt, float x );
float yCoord( Plot plt, float y );
void plotLine( PlaydateAPI* pd, Plot plt, float x1, float y1, float x2, float y2 );
void plotArray( PlaydateAPI* pd, Plot plt, float xdata[], float ydata[], int size );
void plotMarker( PlaydateAPI* pd, Plot plt, float xdata[], float ydata[], int index );
void addXAxisLabels( Plot* plt, char label[][STRLEN], int size );
void addYAxisLabels( Plot* plt, char label[][STRLEN], int size );

#endif

