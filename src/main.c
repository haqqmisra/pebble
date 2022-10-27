#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "pd_api.h"
#include "model.h"
#include "util.h"
#include "draw.h"

static int update(void* userdata);
const char* fontpath = "fonts/font-rains-1x.pft";
LCDFont* font = NULL;

int eventHandler( PlaydateAPI* pd, PDSystemEvent event, uint32_t arg )
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit )
	{
		const char* err;
		font = pd->graphics->loadFont( fontpath, &err );

		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->setUpdateCallback( update, pd );
	}

	return 0;
}

#define NBELTS 18
#define NPTS NBELTS+2

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400-TEXT_WIDTH)/4;
int y = (240-TEXT_HEIGHT)/2;
int dx = 1;
int dy = 2;



char out[144], out1[144];
int flag = 0;


enum Status { initializing, running, paused, crashed };
enum Status state = initializing;

float temp[NBELTS+2], lat[NBELTS+2], xlat[NBELTS+2], dxlat[NBELTS+1], t2prime[NBELTS];


//float temp[4] = { 15.0, 15.0, 15.0, 15.0 };
float dt = 8.64e4;	// seconds
int niter = 0;

static int update( void* userdata )
{
	PlaydateAPI* pd = userdata;

	pd->display->setInverted( 1 );
	pd->graphics->clear( kColorWhite );
	pd->graphics->setFont( font );

        PDButtons pushed;
        pd->system->getButtonState( NULL, &pushed, NULL );

        if ( state == initializing ) {
		pd->graphics->drawText( "Initializing...", strlen( "Initializing..." ), kASCIIEncoding, x, y );

		init( temp, lat, xlat, dxlat, NBELTS, 15.0 );

		pd->graphics->drawText( "Ready!", strlen( "Ready!" ), kASCIIEncoding, x, y+20 );

		//if ( flag == 0 ) {
		//	float_to_string( FLT_MAX, out1 );
		//	strcpy( out, "Float max: " );
		//	strcat( out, out1 );
		//	pd->system->logToConsole( out );
		//	flag = 1;
		//}

	        if ( pushed & kButtonA ) {
			state = running;
		}
	}
	else if ( state == running ) {



		//updateAllLat( temp, dt, NPTS, t2prime, xlat, dxlat );
		updateAllLat( temp, dt, niter, NPTS, t2prime, lat, xlat, dxlat );
		niter++;


		//float_to_string( t2prime[0] * 0.58, out );
		//pd->system->logToConsole( out );


		printAllLatLines( pd, lat, xlat, temp, NBELTS, x, y );

	        if ( pushed & kButtonA ) {
			state = paused;
		}
	}
	else if ( state == paused ) {
	        if ( pushed & kButtonA ) {
			state = running;
		}
	}
	else if ( state == crashed ) {
		pd->system->error( "Halting: model crashed" );
	}
	else {
		pd->system->error( "Halting: non-existent state reached" );
	}

	pd->system->drawFPS( 0, 0 );

	return 1;
}

