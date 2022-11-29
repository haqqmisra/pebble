#include "draw.h"

// Drawing/IO Functions for PEBBLE

void printLatLine( PlaydateAPI* pd, float lat, float temp, int x, int y )
{
	char out[STRLEN], f1[STRLEN], f2[STRLEN];
	float latdeg = roundf( deg2rad( lat ) );

	ftoa( temp, f2, 1 );
	float_to_string( latdeg, f1 );

	f1[ strlen( f1 ) - 2 ] = '\0';

	if ( latdeg > 0 ) {
		if ( fabsf( latdeg ) < 10 ) {
			strcpy( out, "  " );
		}
		else {
			strcpy( out, " " );
		}
	}
	else {
		if ( fabsf( latdeg ) < 10 ) {
			strcpy( out, " " );
		}
		else {
			strcpy( out, "" );
		}
	}

	strcat( out, f1 );
	strcat( out, " " );
	strcat( out, f2 );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );
	return;
}

void printAllLatLines( PlaydateAPI* pd, float lat[], float temp[], int size, int x, int y )
{
	int i;
	float dy = 12.5;
	for ( i = size; i > 0; i-- ) {
		printLatLine( pd, lat[i], temp[i], x, y - i * dy );
	}
	return;
}

void printFloat( PlaydateAPI* pd, int x, int y, float flt, int prec )
{
	char out[STRLEN];
	float tmp;

	tmp = newPrecision( flt, prec );
	float_to_string( tmp, out );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );
	return;
}

void printInt( PlaydateAPI* pd, int x, int y, int num, int prec )
{
	char out[STRLEN];

	int_to_string( num, out, 10 );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );
	return;
}

void createPlot( struct Plot* plt, int x, int y, int width, int height )
{
	plt->xdraw    = x;
	plt->ydraw    = y;
	plt->width    = width;
	plt->height   = height;
	plt->xorigin  = x;
	plt->yorigin  = height + y - 1;
	plt->xrcorner = width + x - 1;
	plt->yrcorner = y;

	return;
}

void drawPlot( PlaydateAPI* pd, struct Plot plt )
{
	int lenticks = 5;
	int xaxisxoffset = -7;
	int xaxisyoffset = 5;
	int yaxisxoffset = -27;
	int yaxisyoffset = -3;
	int i;

	pd->graphics->drawRect( plt.xdraw, plt.ydraw, plt.width, plt.height, kColorBlack );

	for ( i = 1; i < NUMXTICKS; i++ ) {
		pd->graphics->drawLine( plt.xdraw + i * plt.width / NUMXTICKS, plt.ydraw, plt.xdraw + i * plt.width / NUMXTICKS, plt.ydraw + lenticks, 1, kColorBlack );
		pd->graphics->drawLine( plt.xdraw + i * plt.width / NUMXTICKS, plt.yorigin, plt.xdraw + i * plt.width / NUMXTICKS, plt.yorigin - lenticks, 1, kColorBlack );
	}
	for ( i = 1; i < NUMYTICKS; i++ ) {
		pd->graphics->drawLine( plt.xdraw, plt.ydraw + i * plt.height / NUMYTICKS, plt.xdraw + lenticks, plt.ydraw + i * plt.height / NUMYTICKS, 1, kColorBlack );
		pd->graphics->drawLine( plt.xrcorner, plt.ydraw + i * plt.height / NUMYTICKS, plt.xrcorner - lenticks, plt.ydraw + i * plt.height / NUMYTICKS, 1, kColorBlack );
	}

	for ( i = 0; i < NUMXTICKS + 1; i++ ) {
		pd->graphics->drawText( plt.xlabels[i], strlen( plt.xlabels[i] ), kASCIIEncoding, plt.xdraw + i * plt.width / NUMXTICKS + xaxisxoffset, plt.yorigin + xaxisyoffset );
	}
	for ( i = 0; i < NUMYTICKS + 1; i++ ) {
		pd->graphics->drawText( plt.ylabels[i], strlen( plt.ylabels[i] ), kASCIIEncoding, plt.xdraw + yaxisxoffset, plt.ydraw + i * plt.height / NUMYTICKS + yaxisyoffset );
	}

	return;
}

void setXlimits( struct Plot* plt, float xmin, float xmax )
{
	plt->xmin  = xmin;
	plt->xmax  = xmax;
	plt->xconv = plt->width / fabsf( xmax - xmin );
	return;
}

void setYlimits( struct Plot* plt, float ymin, float ymax )
{
	plt->ymin = ymin;
	plt->ymax = ymax;
	plt->yconv = plt->height / fabsf( ymax - ymin );
	return;
}

float xCoord( struct Plot plt, float x )
{
	return plt.xorigin + ( x - plt.xmin ) * plt.xconv;
}

float yCoord( struct Plot plt, float y )
{
	return plt.yorigin - ( y - plt.ymin ) * plt.yconv + 1;
}

void plotLine( PlaydateAPI* pd, struct Plot plt, float x1, float y1, float x2, float y2 )
{
	float xx1, xx2, yy1, yy2;

	xx1 = xCoord( plt, x1 );
	xx2 = xCoord( plt, x2 );
	yy1 = yCoord( plt, y1 );
	yy2 = yCoord( plt, y2 );

	if ( xx1 > plt.xrcorner ) xx1 = plt.xrcorner;
	if ( xx2 > plt.xrcorner ) xx2 = plt.xrcorner;
	if ( xx1 < plt.xorigin ) xx1 = plt.xorigin;
	if ( xx2 < plt.xorigin ) xx2 = plt.xorigin;
	if ( yy1 < plt.yrcorner ) yy1 = plt.yrcorner;
	if ( yy2 < plt.yrcorner ) yy2 = plt.yrcorner;
	if ( yy1 > plt.yorigin ) yy1 = plt.yorigin;
	if ( yy2 > plt.yorigin ) yy2 = plt.yorigin;


	if ( ( x1 != IGNORE ) && ( x2 != IGNORE ) && ( y1 != IGNORE ) && ( y2 != IGNORE ) ) {
		pd->graphics->drawLine( xx1, yy1, xx2, yy2, 1, kColorBlack );
	}
	return;
}

void plotArray( PlaydateAPI* pd, struct Plot plt, float xdata[], float ydata[], int size )
{
	int i;

	for ( i = 1; i < size; i++ ) {
		plotLine( pd, plt, xdata[i-1], ydata[i-1], xdata[i], ydata[i] );
	}
	return;
}

void addXAxisLabels( struct Plot* plt, char label[][STRLEN], int size )
{
	int i;

	for ( i = 0; i < size; i++ ) {
		strcpy( plt->xlabels[i], label[i] );
	}
	return;
}

void addYAxisLabels( struct Plot* plt, char label[][STRLEN], int size )
{
	int i;

	for ( i = 0; i < size; i++ ) {
		strcpy( plt->ylabels[i], label[i] );
	}
	return;
}

void batteryPercentString( PlaydateAPI* pd, char *out )
{
	int_to_string( pd->system->getBatteryPercentage(), out, 10 );
        strcat( out, "%" );
        return;
}
