#include <stdio.h>
#include <stdlib.h>

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



int m;


enum Status { initializing, running, paused, crashed };
enum Status state = initializing;

float temp[NPTS], lat[NPTS], xlat[NPTS], dxlat[NPTS-1], thermal[NPTS], diff[NPTS], area[NPTS];
float tmean;
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

		pd->system->resetElapsedTime();

		init( temp, lat, xlat, dxlat, diff, area, NPTS, NBELTS, 273.0 );

		pd->graphics->drawText( "Ready!", strlen( "Ready!" ), kASCIIEncoding, x, y+20 );

	        if ( pushed & kButtonA ) {
			state = running;
		}
	}
	else if ( state == running ) {

		for ( m = 0; m < 30; m++ ) {
			updateAllLat( temp, dt, niter, NPTS, diff, thermal, lat, xlat, dxlat );
			tmean = updateMeanTemp( temp, area, niter, NPTS, tmean );
			niter++;
		}

		printAllLatLines( pd, lat, thermal, temp, NBELTS, x, y );
		//printAllLatLines( pd, lat, thermal, temp, NPTS, x, y );

		printFloat( pd, x+150, y, pd->system->getElapsedTime() );

		printFloat( pd, x+150, y-40, tmean );
		printFloat( pd, x+150, y-20, niter );

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

