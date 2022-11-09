#include "draw.h"

// Drawing/IO Functions for PEBBLE

void printLatLine( PlaydateAPI* pd, float lat, float temp, int x, int y )
{
	char out[144], f1[144], f2[144];
	float latdeg = roundf( deg2rad( lat ) );

	float_to_string( latdeg, f1 );
	float_to_string( newPrecision( temp, 1 ), f2 );

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
	char out[144];
	float tmp;

	tmp = newPrecision( flt, prec );
	float_to_string( tmp, out );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );
	return;
}

void printInt( PlaydateAPI* pd, int x, int y, int num, int prec )
{
	char out[144];

	int_to_string( num, out, 10 );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );
	return;
}

void createPlot( struct Plot* plt, int x, int y, int width, int height )
{
	plt->xdraw   = x;
	plt->ydraw   = y;
	plt->width   = width;
	plt->height  = height;
	plt->xorigin = width + x - 1;
	plt->yorigin = height + y - 1;

	return;
}

void drawPlot( PlaydateAPI* pd, struct Plot plt )
{
	int lenticks = 5;
	int labelxoffset = -7;
	int labelyoffset = 5;
	int i;

	pd->graphics->drawRect( plt.xdraw, plt.ydraw, plt.width, plt.height, kColorBlack );

	for ( i = 1; i < NUMXTICKS; i++ ) {
		pd->graphics->drawLine( plt.xdraw + i * plt.width / NUMXTICKS, plt.ydraw, plt.xdraw + i * plt.width / NUMXTICKS, plt.ydraw + lenticks, 1, kColorBlack );
		pd->graphics->drawLine( plt.xdraw + i * plt.width / NUMXTICKS, plt.yorigin, plt.xdraw + i * plt.width / NUMXTICKS, plt.yorigin - lenticks, 1, kColorBlack );
	}
	for ( i = 1; i < NUMYTICKS; i++ ) {
		pd->graphics->drawLine( plt.xdraw, plt.ydraw + i * plt.height / NUMYTICKS, plt.xdraw + lenticks, plt.ydraw + i * plt.height / NUMYTICKS, 1, kColorBlack );
		pd->graphics->drawLine( plt.xorigin, plt.ydraw + i * plt.height / NUMYTICKS, plt.xorigin - lenticks, plt.ydraw + i * plt.height / NUMYTICKS, 1, kColorBlack );
	}

	for ( i = 0; i < NUMXTICKS + 1; i++ ) {
		pd->graphics->drawText( plt.xlabels[i], strlen( plt.xlabels[i] ), kASCIIEncoding, plt.xdraw + i * plt.width / NUMXTICKS + labelxoffset, plt.yorigin + labelyoffset );
	}

	return;
}

