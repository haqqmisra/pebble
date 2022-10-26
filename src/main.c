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

#define NBELTS 16

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400-TEXT_WIDTH)/4;
int y = (240-TEXT_HEIGHT)/2;
int dx = 1;
int dy = 2;



char out1[144], out2[144], out3[144], out4[144];


enum Status { initializing, running, paused, crashed };
enum Status state = initializing;

float temp[NBELTS+1], lat[NBELTS+1], xlat[NBELTS+1], dxlat[NBELTS];


//float temp[4] = { 15.0, 15.0, 15.0, 15.0 };
float dt = 8.64e4;	// seconds

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

	        if ( pushed & kButtonA ) {
			state = running;
		}
	}
	else if ( state == running ) {

		temp[0] = temp[0] + nextStep( temp[0], dt );
		temp[1] = temp[1] + nextStep( temp[1], dt );
		temp[2] = temp[2] + nextStep( temp[2], dt );
		temp[3] = temp[3] + nextStep( temp[3], dt );


		printAllLatLines( pd, lat, xlat, temp, NBELTS, x, y );


		//	temp = temp + nextStep( temp, dt );

		//float_to_string( temp[0], out1 );
		//float_to_string( temp[1], out2 );
		//float_to_string( temp[2], out3 );
		//float_to_string( temp[3], out4 );
		//	ftoa( temp, out, 6 );

		//pd->graphics->drawText( out1, strlen( out1 ), kASCIIEncoding, x, y );
		//pd->graphics->drawText( out2, strlen( out2 ), kASCIIEncoding, x, y-20 );
		//pd->graphics->drawText( out3, strlen( out3 ), kASCIIEncoding, x, y-40 );
		//pd->graphics->drawText( out4, strlen( out4 ), kASCIIEncoding, x, y-60 );

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
	}
	else {}

	pd->system->drawFPS( 0, 0 );

	return 1;
}

