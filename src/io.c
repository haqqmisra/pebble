#include "io.h"

int stepThroughYears( PDButtons pushed, float crankchange, int yr, int yrprint )
{
	int dyr = 0;

	if ( ( crankchange > 0 ) && ( yrprint < yr ) ) {
		dyr = (int)( yrprint + crankchange < yr ? crankchange : yr - yrprint );
	}
	else if ( ( crankchange < 0 ) && ( yrprint > 0 ) ) {
                dyr = (int)( yrprint + crankchange >= 0 ? crankchange : 0 );
        }
	else if ( ( pushed & kButtonLeft ) && ( yrprint > 0 ) ) {
		dyr = -1;
	}
	else if ( ( pushed & kButtonDown ) && ( yrprint > 0 ) ) {
		dyr = -1;
	}
	else if ( ( pushed & kButtonRight ) && ( yrprint < yr ) ) {
		dyr = 1;
	}
	else if ( ( pushed & kButtonUp ) && ( yrprint < yr ) ) {
		dyr = 1;
	}

	return yrprint + dyr;
}
