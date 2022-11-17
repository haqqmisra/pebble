#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "model.h"
#include "util.h"
#include "draw.h"
#include "constants.h"

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

struct Plot plot1, plot2;

enum Status { initializing, running, paused, crashed, completed };
enum Status state = initializing;

enum Display { annual, daily, configure };
enum Display screen = configure;

float temp[NPTS], lat[NPTS], xlat[NPTS], dxlat[NPTS-1], thermal[NPTS], diff[NPTS], area[NPTS];
float latdeg[NPTS];

float tmeanlast;
float tmeanlat[NPTS], tmeanlatlast[NPTS];
float pstart, pend, pausedtime, runtime;

float dt   = 8.64e4;	// seconds
int niter  = 0;
int year;
int yriter = 0;
int i, m;

static int update( void* userdata )
{
	PlaydateAPI* pd = userdata;

	pd->graphics->clear( kColorWhite );
	pd->graphics->setFont( font );

        PDButtons pushed;
        pd->system->getButtonState( NULL, &pushed, NULL );

        if ( state == initializing ) {
		pd->graphics->drawText( "Initializing...", strlen( "Initializing..." ), kASCIIEncoding, 80, 50 );

		pd->system->resetElapsedTime();
		pausedtime = 0;

		year = callYear( -1 );
		callTempLatSum( -1, 0 );

		init( temp, lat, xlat, dxlat, diff, area, NPTS, NBELTS, TINIT );
		tmeanlast = TINIT;
		for ( i = 0; i < NPTS; i++ ) {
			latdeg[i]       = deg2rad( lat[i] );
			tmeanlatlast[i] = TINIT;
		}

		createPlot( &plot1, 120, 20, 270, 90 );
		setXlimits( &plot1, -90, 90 );
		//setYlimits( &plot1, -50, 50 );
		setYlimits( &plot1, TFREEZE - 40, TFREEZE + 40 );
		strcpy( plot1.xlabels[0], "SP" );
		strcpy( plot1.xlabels[1], "60S" );
		strcpy( plot1.xlabels[2], "30S" );
		strcpy( plot1.xlabels[3], "EQ" );
		strcpy( plot1.xlabels[4], "30N" );
		strcpy( plot1.xlabels[5], "60N" );
		strcpy( plot1.xlabels[6], "NP" );
		strcpy( plot1.ylabels[0], "" );
		strcpy( plot1.ylabels[1], "+20" );
		strcpy( plot1.ylabels[2], "FRZ" );
		strcpy( plot1.ylabels[3], "-20" );
		strcpy( plot1.ylabels[4], "" );

		createPlot( &plot2, 120, 20 + 90 + 20, 270, 90 );

		pd->graphics->drawText( "Ready!", strlen( "Ready!" ), kASCIIEncoding, 80, 50+20 );

	        if ( pushed & kButtonA ) {
			state  = running;
			screen = annual;
		}
	}
	else if ( state == running ) {

		// model driver loop
		for ( m = 0; m < STEPS_PER_UPDATE; m++ ) {
			niter++;
			yriter++;

			updateAllLat( temp, dt, niter, NPTS, diff, thermal, lat, xlat, dxlat );

			annualMeanTempLat( temp, yriter, NPTS, tmeanlat );

			if ( callYear( 0 ) == NYEARS ) {
				state = completed;
				break;
			}
			else if ( year < callYear( 0 ) ) {
				year   = callYear( 0 );
				yriter = 0;
				for ( i = 0; i < NPTS; i++ ) {
					tmeanlatlast[i] = tmeanlat[i];
				}
				callTempLatSum( -1, 0 );
				tmeanlast = getMeanTemp( tmeanlatlast, area, NPTS );
			}
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
	else if ( state == completed ) {
		pstart = pd->system->getElapsedTime() - pausedtime;
	}
	else if ( state == crashed ) {
		pd->system->error( "Error: model crashed" );
	}
	else {
		pd->system->error( "Error: non-existent state reached" );
	}


	if ( screen == annual ) {

		drawPlot( pd, plot1 );
		plotArray( pd, plot1, latdeg, tmeanlatlast, NPTS );

		drawPlot( pd, plot2 );


		pd->graphics->drawText( "lat", strlen( "lat" ), kASCIIEncoding, 5, 3 );
		pd->graphics->drawText( "temp", strlen( "temp" ), kASCIIEncoding, 37, 3 );
		printAllLatLines( pd, lat, tmeanlatlast, NBELTS, 2, SCREEN_HEIGHT );

		pd->graphics->drawText( "avg =", strlen( "avg =" ), kASCIIEncoding, 75, 3 );
		printFloat( pd, 120, 3, tmeanlast, 1 );
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

