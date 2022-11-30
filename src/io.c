#include "io.h"

int stepThroughTime( PDButtons pushed, float crankchange, int step, int stepprint )
{
	int dstep = 0;

	if ( ( crankchange > 0 ) && ( stepprint < step ) ) {
		dstep = (int)( stepprint + crankchange < step ? crankchange : step - stepprint );
	}
	else if ( ( crankchange < 0 ) && ( stepprint > 0 ) ) {
                dstep = (int)( stepprint + crankchange >= 0 ? crankchange : 0 );
        }
	else if ( ( pushed & kButtonLeft ) && ( stepprint > 0 ) ) {
		dstep = -1;
	}
	else if ( ( pushed & kButtonDown ) && ( stepprint > 0 ) ) {
		dstep = -1;
	}
	else if ( ( pushed & kButtonRight ) && ( stepprint < step ) ) {
		dstep = 1;
	}
	else if ( ( pushed & kButtonUp ) && ( stepprint < step ) ) {
		dstep = 1;
	}

	return stepprint + dstep;
}
