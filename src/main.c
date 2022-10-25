#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "model.h"
#include "util.h"

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

int eventHandler( PlaydateAPI* pd, PDSystemEvent event, uint32_t arg )
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit )
	{
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);

		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->setUpdateCallback( update, pd );
	}

	return 0;
}

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400-TEXT_WIDTH)/2;
int y = (240-TEXT_HEIGHT)/2;
int dx = 1;
int dy = 2;

int i = 0;
char out[144];

float temp = -5;	// degC
float dt = 8.64e4;	// seconds

static int update( void* userdata )
{
	PlaydateAPI* pd = userdata;

	pd->display->setInverted( 1 );
	pd->graphics->clear( kColorWhite );
	pd->graphics->setFont( font );

	temp = temp + nextStep( temp, dt );

	float_to_string( temp, out );
//	ftoa( temp, out, 6 );

	pd->graphics->drawText( out, strlen( out ), kASCIIEncoding, x, y );

	i++;


	pd->system->drawFPS( 0, 0 );

	return 1;
}

