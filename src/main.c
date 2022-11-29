#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "model.h"
#include "util.h"
#include "draw.h"
#include "io.h"
#include "constants.h"

static int update( void* userdata );
void reset( void* userdata );
const char* fontpath = "fonts/font-rains-1x.pft";
LCDFont* font = NULL;
PDMenuItem *resetButton = NULL;

int eventHandler( PlaydateAPI* pd, PDSystemEvent event, uint32_t arg )
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {
		const char* err;
		font = pd->graphics->loadFont( fontpath, &err );

		resetButton = pd->system->addMenuItem( "Reset", reset, NULL );

		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->setAutoLockDisabled( 1 );
		pd->system->setUpdateCallback( update, pd );
	}
	else if ( event == kEventResume ) {

	}

	return 0;
}

struct Plot plot1, plot2;

enum Status { initializing, ready, running, paused, crashed, completed };
enum Status state = initializing;

enum Display { annual, daily, configure };
enum Display screen = configure;

float temp[NPTS], lat[NPTS], xlat[NPTS], dxlat[NPTS-1], thermal[NPTS], diff[NPTS], area[NPTS];
float latdeg[NPTS];

float tmeanlat[NPTS];
float tmeantime[NYEARS+1];
float tmeanlattime[NYEARS+1][NPTS];
float tmeandaily[NITERMAX];
float *tprint = NULL;
float *tlatprint = NULL;
float pstart, pend, pausedtime, runtime;

int niter, yriter, year, yearprint;
int tind, steps;
int i, m;

float yearaxis[NYEARS];
char xaxislabel1[NUMXTICKS+1][STRLEN] = { "SP", "60S", "30S", "EQ", "30N", "60N", "NP" };
char xaxislabel2[NUMXTICKS+1][STRLEN];
char yaxislabel1[NUMYTICKS+1][STRLEN] = { "", "+20", "FRZ", "-20", "" };
char yaxislabel2[NUMYTICKS+1][STRLEN] = { "", "+20", "FRZ", "-20", "" };

char status1[STRLEN], status2[STRLEN];
char batterylife[STRLEN];

static int update( void* userdata )
{
	PlaydateAPI* pd = userdata;

	pd->graphics->clear( kColorWhite );
	pd->graphics->setFont( font );

        PDButtons pushed;
        pd->system->getButtonState( NULL, &pushed, NULL );

	float crankchange = 0;
        if ( pd->system->isCrankDocked() == 0 ) {
		crankchange = pd->system->getCrankChange();
	}

        if ( state == initializing ) {
		strcpy( status1, "Initializing" );

		niter  = 0;
		yriter = 0;
		tind   = 0;

		pd->system->resetElapsedTime();
		pausedtime = 0;

		steps = STEPS_PER_UPDATE;
		year  = callYear( -1 );
		callTempLatSum( -1, 0 );

		init( temp, lat, xlat, dxlat, diff, area, NPTS, NBELTS, TINIT );
		for ( i = 0; i < NPTS; i++ ) {
			latdeg[i]       = deg2rad( lat[i] );
			tmeanlattime[0][i] = TINIT;
		}
		for ( i = 0; i < NYEARS + 1; i++ ) {
			yearaxis[i] = i;
			tmeantime[i] = IGNORE;
		}
		tmeantime[0]  = TINIT;
		tmeandaily[0] = TINIT;

		createPlot( &plot1, PLOTX, PLOTY, PLOTWIDTH, PLOTHEIGHT );
		setXlimits( &plot1, -90, 90 );
		setYlimits( &plot1, TFREEZE - 40, TFREEZE + 40 );
		addXAxisLabels( &plot1, xaxislabel1, NUMXTICKS + 1 );
		addYAxisLabels( &plot1, yaxislabel1, NUMYTICKS + 1 );

		createPlot( &plot2, PLOTX, PLOTY + PLOTDY, PLOTWIDTH, PLOTHEIGHT );
		setXlimits( &plot2, 0, NYEARS );
		setYlimits( &plot2, TFREEZE - 40, TFREEZE + 40 );
		for ( i = 0; i < NUMXTICKS + 1; i++ ) {
			if ( NYEARS < NUMXTICKS ) {
				float_to_string( i * (float)NYEARS / NUMXTICKS, xaxislabel2[i] );
			}
			else {
				int_to_string( (int)floorf( i * NYEARS / NUMXTICKS ), xaxislabel2[i], 10 );
			}
		}
		addXAxisLabels( &plot2, xaxislabel2, NUMXTICKS + 1 );
		addYAxisLabels( &plot2, yaxislabel2, NUMYTICKS + 1 );

		state = ready;
	}
	else if ( state == ready ) {
		strcpy( status1, "Ready (A to Start)" );

	        if ( pushed & kButtonA ) {
			state  = running;
			screen = annual;
			strcpy( status2, "Annual" );
		}
	}
	else if ( state == running ) {
		strcpy( status1, "Running (A to Pause)" );

		// model driver loop
		for ( m = 0; m < steps; m++ ) {
			niter++;
			yriter++;

			updateAllLat( temp, DT, niter, NPTS, diff, thermal, lat, xlat, dxlat );

			tmeandaily[niter] = getMeanTemp( temp, area, NPTS );

			annualMeanTempLat( temp, yriter, NPTS, tmeanlat );

			if ( year < callYear( 0 ) ) {
				year      = callYear( 0 );
				yearprint = year;
				yriter = 0;
				for ( i = 0; i < NPTS; i++ ) {
					tmeanlattime[year][i] = tmeanlat[i];
				}
				callTempLatSum( -1, 0 );
				tmeantime[year] = getMeanTemp( tmeanlattime[year], area, NPTS );
			}

			if ( callYear( 0 ) == NYEARS ) {
				state = completed;
				break;
			}
		}

		runtime = pd->system->getElapsedTime() - pausedtime;
	        if ( pushed & kButtonA ) {
			state = paused;
			pstart = pd->system->getElapsedTime() - pausedtime;
		}
	}
	else if ( state == paused ) {
		strcpy( status1, "Paused (A to Resume)" );

		runtime = pstart;

		if ( screen == annual ) {
			yearprint = stepThroughYears( pushed, crankchange, year, yearprint );
		}
		else if (screen == daily ) {
			//dayprint = stepThroughDays( pushed, crankchange, 
		}

	        if ( pushed & kButtonA ) {
			state      = running;
			yearprint  = year;
			pend       = pd->system->getElapsedTime() - pausedtime;
			pausedtime = pausedtime + ( pend - pstart );
		}
	}
	else if ( state == completed ) {
		strcpy( status1, "Done (use crank/arrows)" );

		if ( screen == annual ) {
			yearprint = stepThroughYears( pushed, crankchange, year, yearprint );
		}
		else if (screen == daily ) {

		}
	}
	else if ( state == crashed ) {
		strcpy( status1, "Crash" );
		pd->system->error( "Error: model crashed" );
	}
	else {
		pd->system->error( "Error: non-existent state reached" );
	}


	if ( screen == configure ) {
		pd->graphics->drawText( "PEBBLE", strlen( "PEBBLE" ), kASCIIEncoding, 80, 50+20 );
	}
	else {

		if ( screen == annual ) {
			tind      = yearprint;
			tprint    = tmeantime;
			tlatprint = tmeanlattime[yearprint];

		        if ( pushed & kButtonB ) {
				screen = daily;
				strcpy( status2, "Daily" );
				steps = 1;
			}
		}
		else if ( screen == daily ) {
			tprint    = tmeandaily;
			tlatprint = temp;
			tind      = niter;

		        if ( pushed & kButtonB ) {
				screen = annual;
				strcpy( status2, "Annual" );
				steps = STEPS_PER_UPDATE;
			}
		}


		drawPlot( pd, plot1 );
		plotArray( pd, plot1, latdeg, tlatprint, NPTS );

		drawPlot( pd, plot2 );
		plotArray( pd, plot2, yearaxis, tmeantime, NYEARS + 1 );

		pd->graphics->drawText( "lat", strlen( "lat" ), kASCIIEncoding, 5, 3 );
		pd->graphics->drawText( "temp", strlen( "temp" ), kASCIIEncoding, 37, 3 );
		printAllLatLines( pd, lat, tlatprint, NBELTS, 2, SCREEN_HEIGHT );

		pd->graphics->drawText( "avg =", strlen( "avg =" ), kASCIIEncoding, 75, 3 );
		printFloat( pd, 120, 3, tprint[tind], 1 );
		pd->graphics->drawText( "K", strlen( "K" ), kASCIIEncoding, 162, 3 );

		pd->graphics->drawText( "year =", strlen( "year =" ), kASCIIEncoding, 195, 3 );
		printInt( pd, 250, 3, yearprint, 2 );

		pd->graphics->drawText( "time:", strlen( "time:" ), kASCIIEncoding, 295, 3 );
		printFloat( pd, 345, 3, runtime, 1 );
	}

	pd->graphics->drawText( status1, strlen( status1 ), kASCIIEncoding, 100, SCREEN_HEIGHT - 10 );
	pd->graphics->drawText( status2, strlen( status2 ), kASCIIEncoding, 290, SCREEN_HEIGHT - 10 );

	batteryPercentString( pd, batterylife );
	pd->graphics->drawText( batterylife, strlen( batterylife ), kASCIIEncoding, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 10 );

	//pd->system->drawFPS( SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10 );

	return 1;
}

void reset( void* userdata ) {
	state    = initializing;
	screen   = configure;
}
