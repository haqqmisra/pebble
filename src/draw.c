#include "draw.h"

// Drawing/IO Functions for PEBBLE

void printLatLine( PlaydateAPI* pd, float lat, float xlat, float temp, int x, int y )
{
	char out[144], f1[144], f2[144], f3[144];
	float latdeg = roundf( deg2rad( lat ) );

	float_to_string( latdeg, f1 );
	float_to_string( xlat, f2 );
	float_to_string( temp, f3 );

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
	strcat( out, "   " );
	//strcat( out, f2 );
	strcat( out, "   " );
	strcat( out, f3 );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );
	return;
}

void printAllLatLines( PlaydateAPI* pd, float lat[], float xlat[], float temp[], int size, int x, int y )
{
	int i;
	int xtop = 50;
	int ytop = 200;
	int dy = 12;
	for ( i = size - 1; i >= 0; i-- ) {
		printLatLine( pd, lat[i], xlat[i], temp[0], xtop, ytop - i*dy );
	}
	return;
}
