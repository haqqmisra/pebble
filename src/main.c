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

#define STEPS_PER_UPDATE 30
#define NBELTS 18
#define NPTS NBELTS+2

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

struct Plot plot1, plot2;

enum Status { initializing, running, paused, crashed };
enum Status state = initializing;

enum Display { annual, daily, configure };
enum Display screen = configure;

float temp[NPTS], lat[NPTS], xlat[NPTS], dxlat[NPTS-1], thermal[NPTS], diff[NPTS], area[NPTS];
float tmeanglobal[NPTS];
float tmean;
float pstart, pend, pausedtime, runtime;

float dt = 8.64e4;	// seconds
int niter = 0;
int year = 1;
int i, m;

static int update( void* userdata )
{
	PlaydateAPI* pd = userdata;

	//pd->display->setInverted( 1 );
	pd->graphics->clear( kColorWhite );
	pd->graphics->setFont( font );

        PDButtons pushed;
        pd->system->getButtonState( NULL, &pushed, NULL );

        if ( state == initializing ) {
		pd->graphics->drawText( "Initializing...", strlen( "Initializing..." ), kASCIIEncoding, 80, 50 );

		pd->system->resetElapsedTime();
		pausedtime = 0;

		init( temp, lat, xlat, dxlat, diff, area, NPTS, NBELTS, 273.0 );
		tmean = 0;
		for ( i = 0; i < NPTS; i++ ) {
			tmeanglobal[i]    = temp[i];
		}

		createPlot( &plot1, 120, 20, 270, 90 );
		createPlot( &plot2, 120, 20 + 90 + 20, 270, 90 );

		strcpy( plot1.xlabels[0], "NP" );
		strcpy( plot1.xlabels[1], "60N" );
		strcpy( plot1.xlabels[2], "30N" );
		strcpy( plot1.xlabels[3], "EQ" );
		strcpy( plot1.xlabels[4], "30S" );
		strcpy( plot1.xlabels[5], "60S" );
		strcpy( plot1.xlabels[6], "SP" );

		pd->graphics->drawText( "Ready!", strlen( "Ready!" ), kASCIIEncoding, 80, 50+20 );

	        if ( pushed & kButtonA ) {
			state  = running;
			screen = annual;
		}
	}
	else if ( state == running ) {

		for ( m = 0; m < STEPS_PER_UPDATE; m++ ) {
			updateAllLat( temp, dt, niter, NPTS, diff, thermal, lat, xlat, dxlat );
			tmean = updateMeanTemp( temp, area, niter, NPTS, tmean, tmeanglobal );
			niter++;
		}

		if ( year < callYear( 0 ) ) {
			year++;
		}

		runtime = pd->system->getElapsedTime() - pausedtime;

	        if ( pushed & kButtonA ) {
			state = paused;
			pstart = pd->system->getElapsedTime() - pausedtime;
		}
	}
	else if ( state == paused ) {

		runtime = pstart;

	        if ( pushed & kButtonA ) {
			state = running;
			pend  = pd->system->getElapsedTime() - pausedtime;
			pausedtime = pausedtime + ( pend - pstart );
		}
	}
	else if ( state == crashed ) {
		pd->system->error( "Error: model crashed" );
	}
	else {
		pd->system->error( "Error: non-existent state reached" );
	}


	if ( screen == annual ) {

		drawPlot( pd, plot1 );
		drawPlot( pd, plot2 );

		pd->graphics->drawText( "lat", strlen( "lat" ), kASCIIEncoding, 5, 3 );
		pd->graphics->drawText( "temp", strlen( "temp" ), kASCIIEncoding, 37, 3 );
		printAllLatLines( pd, lat, tmeanglobal, NBELTS, 2, SCREEN_HEIGHT );

		pd->graphics->drawText( "avg =", strlen( "avg =" ), kASCIIEncoding, 75, 3 );
		printFloat( pd, 120, 3, tmean, 1 );
		pd->graphics->drawText( "K", strlen( "K" ), kASCIIEncoding, 162, 3 );

		pd->graphics->drawText( "year =", strlen( "year =" ), kASCIIEncoding, 195, 3 );
		printInt( pd, 250, 3, callYear(0), 2 );

		pd->graphics->drawText( "time:", strlen( "time:" ), kASCIIEncoding, 295, 3 );
		printFloat( pd, 345, 3, runtime, 1 );

	}
	else if ( screen == daily ) {

	}
	else if ( screen == configure ) {

	}
	else {
		pd->system->error( "Error: non-existent screen reached" );
	}



	//pd->system->drawFPS( SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10 );

	return 1;
}

