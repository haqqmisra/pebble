#include "main.h"

LCDFont* font = NULL;
PDMenuItem *resetButton = NULL;
PDMenuItem *unitsButton = NULL;

SDFile* outfile;

json_encoder* encoder;

Status state   = initializing;
Display screen = configure;
Units units    = Kelvin;

struct Plot plot1, plot2;

float temp[NPTS], lat[NPTS], xlat[NPTS], dxlat[NPTS-1], thermal[NPTS], diff[NPTS], area[NPTS];
float latdeg[NPTS];

float tmeanlat[NPTS];
float tmeantime[NYEARS+1];
float tmeanlattime[NYEARS+1][NPTS];
float *tprint = NULL;
float *tlatprint = NULL;
float pstart, pend, pausedtime, runtime;

float tlatconvert[NPTS];
float tconvert;
const char* unitlabels[NUMUNITS] = { "K", "C" };

float *tmeandaily;
int *itercount;
int *daycount;
float **tmeandailylattime;

int niter, yriter, year, yearprint, iterprint, daymax;
int iterold;
int tind, steps;
int i, m;

float yearaxis[NYEARS];
char xaxislabel1[NUMXTICKS+1][STRLEN] = { "SP", "60S", "30S", "EQ", "30N", "60N", "NP" };
char xaxislabel2[NUMXTICKS+1][STRLEN];
char yaxislabel1[NUMYTICKS+1][STRLEN] = { "", "+20", "FRZ", "-20", "" };
char yaxislabel2[NUMYTICKS+1][STRLEN] = { "", "+20", "FRZ", "-20", "" };

char msg1[NUMSTATUS][STRLEN];
char msg2[NUMDISPLAY][STRLEN];
char status1[STRLEN], status2[STRLEN];
char* string1;
char batterylife[STRLEN];


int eventHandler( PlaydateAPI* pd, PDSystemEvent event, uint32_t arg )
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {
		const char* err;
		font = pd->graphics->loadFont( fontpath, &err );

		resetButton = pd->system->addMenuItem( "Reset", reset, pd );
		unitsButton = pd->system->addOptionsMenuItem( "Units", unitlabels, NUMUNITS, updateUnits, pd );
		pd->system->setMenuItemValue( unitsButton, units );

		if ( font == NULL ) {
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);
		}

		pd->system->setAutoLockDisabled( 1 );
		pd->system->setUpdateCallback( update, pd );

		changeState( initializing );
		changeScreen( configure );
	}
	else if ( event == kEventResume ) {
		//
	}
	else if ( event == kEventTerminate ) {
		freeMemory( pd );
	}

	return 0;
}



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
		init( temp, lat, xlat, dxlat, diff, area, NPTS, NBELTS, TINIT );


		outfile = pd->file->open( "out.json", kFileWrite );

		pd->json->initEncoder( encoder, writefile, pd, 1 );
		//outfile = pd->file->open( "tmean.txt", kFileWrite );

		year  = callYear( -1 );
		for ( i = 0; i < NITERMAX; i++ ) {
			updateDeclination( DT, i );
			if ( callYear( 0 ) == NYEARS + 1 ) {
				break;
			}
		}
		daymax = i + 1;
		year  = callYear( -1 );
		tmeandailylattime = (float**) pd->system->realloc( NULL, daymax * sizeof( float* ) );
		tmeandaily        = (float*) pd->system->realloc( NULL, daymax * sizeof( float ) );
		itercount         = (int*)   pd->system->realloc( NULL, daymax * sizeof( int ) );
		daycount          = (int*)   pd->system->realloc( NULL, daymax * sizeof( int ) );
		if ( ( tmeandailylattime == NULL ) || ( tmeandaily == NULL ) || ( itercount == NULL ) || ( daycount == NULL ) ) {
			changeState( crash );
			pd->system->error( "Out of memory" );
		}
		for ( i = 0 ; i < daymax; i++ ) {
			tmeandailylattime[i] = (float*) pd->system->realloc( NULL, ( NPTS + 1 ) * sizeof( float ) );
			if ( tmeandailylattime[i] == NULL ) {
				changeState( crash );
				pd->system->error( "Out of memory" );
			}
			itercount[i]  = i;
			daycount[i]   = 0;
			tmeandaily[i] = IGNORE;
		}

		strcpy( msg1[initializing], "Initializing" );
		strcpy( msg1[ready], "Ready (A to Start)" );
		strcpy( msg1[running], "Running (A to Pause)" );
		strcpy( msg1[paused], "Paused (A to Resume)" );
		strcpy( msg1[done], "Done (use crank/arrows)" );
		strcpy( msg1[crash], "Crashed" );

		strcpy( msg2[configure], "" );
		strcpy( msg2[annual], "Annual View (B for Daily)" );
		strcpy( msg2[daily], "Daily View (B for Annual)" );

		niter  = 0;
		yriter = 0;
		tind   = 0;

		pd->system->resetElapsedTime();
		pausedtime = 0;

		steps = STEPS_PER_UPDATE;
		callTempLatSum( -1, 0 );

		for ( i = 0; i < NPTS; i++ ) {
			latdeg[i]               = deg2rad( lat[i] );
			tmeanlattime[0][i]      = TINIT;
			tmeandailylattime[0][i] = TINIT;
		}
		for ( i = 0; i < NYEARS + 1; i++ ) {
			yearaxis[i]  = i;
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
				pd->system->formatString( &string1, "%0.1f", i * (float)NYEARS / NUMXTICKS, xaxislabel2[i] );
				strcpy( xaxislabel2[i], string1 );
			}
			else {
				pd->system->formatString( &string1, "%d", (int)floorf( i * NYEARS / NUMXTICKS ) );
				strcpy( xaxislabel2[i], string1 );
			}
		}
		addXAxisLabels( &plot2, xaxislabel2, NUMXTICKS + 1 );
		addYAxisLabels( &plot2, yaxislabel2, NUMYTICKS + 1 );

		changeState( ready );
	}
	else if ( state == ready ) {
	        if ( pushed & kButtonA ) {
			changeState( running );
			changeScreen( annual );
		}
	}
	else if ( state == running ) {

		// model driver loop
		for ( m = 0; m < steps; m++ ) {
			niter++;
			yriter++;
			iterprint = niter;
			daycount[niter] = yriter;

			updateAllLat( temp, DT, niter, NPTS, diff, thermal, lat, xlat, dxlat );

			tmeandaily[niter] = getMeanTemp( temp, area, NPTS );
			for ( i = 0; i < NPTS; i++ ) {
				tmeandailylattime[niter][i] = temp[i];
			}

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
				changeState( done );
				//pd->file->write( outfile, "1001", strlen( "1001" ) * sizeof( char ) );

				break;
			}

			if ( niter >= NITERMAX ) {
				changeState( crash );
				pd->system->error( "NITERMAX exceeded" );
				break;
			}
		}

		runtime = pd->system->getElapsedTime() - pausedtime;
	        if ( pushed & kButtonA ) {
			changeState( paused );
			pstart = pd->system->getElapsedTime() - pausedtime;
		}
	}
	else if ( state == paused ) {
		runtime = pstart;

		if ( screen == annual ) {
			yearprint = stepThroughTime( pushed, crankchange, year, yearprint );
		}
		else if (screen == daily ) {
			iterold   = iterprint;
			iterprint = stepThroughTime( pushed, crankchange, niter, iterprint );
			if ( ( iterold < iterprint ) && ( daycount[iterold] > daycount[iterprint] ) ) {
				yearprint++;
			}
			else if ( ( iterold > iterprint ) && ( daycount[iterold] < daycount[iterprint] ) ) {
				yearprint--;
			}
		}

	        if ( pushed & kButtonA ) {
			changeState( running );

			yearprint  = year;
			iterprint  = niter;
			pend       = pd->system->getElapsedTime() - pausedtime;
			pausedtime = pausedtime + ( pend - pstart );
		}
	}
	else if ( state == done ) {
		if ( screen == annual ) {
			yearprint = stepThroughTime( pushed, crankchange, year, yearprint );
		}
		else if (screen == daily ) {
			iterold   = iterprint;
			iterprint = stepThroughTime( pushed, crankchange, niter, iterprint );
			if ( ( iterold < iterprint ) && ( daycount[iterold] > daycount[iterprint] ) ) {
				yearprint++;
			}
			else if ( ( iterold > iterprint ) && ( daycount[iterold] < daycount[iterprint] ) ) {
				yearprint--;
			}
		}
	}
	else if ( state == crash ) {
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
				changeScreen( daily );
			}
		}
		else if ( screen == daily ) {
			tind      = iterprint;
			tprint    = tmeandaily;
			tlatprint = tmeandailylattime[iterprint];

		        if ( pushed & kButtonB ) {
				changeScreen( annual );
			}
		}
		drawPlot( pd, plot1 );
		plotArray( pd, plot1, latdeg, tlatprint, NPTS );

		drawPlot( pd, plot2 );
		plotArray( pd, plot2, yearaxis, tmeantime, NYEARS + 1 );
		plotMarker( pd, plot2, yearaxis, tmeantime, yearprint );

		pd->graphics->drawText( "lat", strlen( "lat" ), kASCIIEncoding, 5, 3 );
		pd->graphics->drawText( "temp", strlen( "temp" ), kASCIIEncoding, 37, 3 );

		if ( units == Kelvin ) {
			for ( i = NBELTS; i > 0; i-- ) {
				pd->system->formatString( &string1, "%3d %5.1f", (int)roundf( deg2rad( lat[i] ) ), tlatprint[i] );
				pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 2, SCREEN_HEIGHT - i * 12.5 );
			}
			pd->system->formatString( &string1, "avg=%5.1fK", tprint[tind] );
			pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 75, 3 );
		}
		else if ( units == Celsius ) {
			tconvert = tprint[tind] - TFREEZE;
			for ( i = 0; i < NPTS; i++ ) {
				tlatconvert[i] = tlatprint[i] - TFREEZE;
			}
			for ( i = NBELTS; i > 0; i-- ) {
				pd->system->formatString( &string1, "%3d %5.1f", (int)roundf( deg2rad( lat[i] ) ), tlatconvert[i] );
				pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 2, SCREEN_HEIGHT - i * 12.5 );
			}
			pd->system->formatString( &string1, "avg=%5.1fC", tconvert );
			pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 75, 3 );
		}

	        pd->system->formatString( &string1, "sol=%4.2f", 1360. / S0 );
		pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 168, 3 );

	        pd->system->formatString( &string1, "obl=%4.1f", OBLIQUITY );
		pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 246, 3 );

	        pd->system->formatString( &string1, "co2=%4.1f", 0 );
		pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 323, 3 );

	        pd->system->formatString( &string1, "year=%d", yearprint );
		pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 315, SCREEN_HEIGHT - 20 );

	        pd->system->formatString( &string1, "day=%d", daycount[iterprint] );
		pd->graphics->drawText( string1, strlen( string1 ), kASCIIEncoding, 323, SCREEN_HEIGHT - 9 );

		//pd->graphics->drawText( "time:", strlen( "time:" ), kASCIIEncoding, 315, 3 );
		//printFloat( pd, 350, 3, runtime, 1 );
	}

	pd->graphics->drawText( status1, strlen( status1 ), kASCIIEncoding, 85, SCREEN_HEIGHT - 20 );
	pd->graphics->drawText( status2, strlen( status2 ), kASCIIEncoding, 85, SCREEN_HEIGHT - 9 );

	//pd->system->drawFPS( SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10 );

	return 1;
}

void reset( void* userdata )
{
	PlaydateAPI* pd = userdata;

	freeMemory( pd );
	state  = initializing;
	screen = configure;
	return;
}

void updateUnits( void* userdata )
{
	PlaydateAPI* pd = userdata;

	units = pd->system->getMenuItemValue( unitsButton );
	return;
}

void freeMemory( PlaydateAPI* pd )
{
	int i;

	string1    = pd->system->realloc( string1, 0 );
	tmeandaily = pd->system->realloc( tmeandaily, 0 );
	itercount  = pd->system->realloc( itercount, 0 );
	daycount   = pd->system->realloc( daycount, 0 );
	for ( i = 0 ; i < daymax; i++ ) {
		tmeandailylattime[i] = pd->system->realloc( NULL, 0 );
	}
	tmeandailylattime = pd->system->realloc( NULL, 0 );

	pd->file->close( outfile );

	return;
}

void changeState( Status newstate )
{
	state = newstate;
	strcpy( status1, msg1[newstate] );
	return;
}

void changeScreen( Display newscreen )
{
	screen = newscreen;
	strcpy( status2, msg2[newscreen] );

	if ( screen == daily ) {
		steps = 1;
	}
	else if ( screen == annual ) {
		steps = STEPS_PER_UPDATE;
	}
	return;
}
