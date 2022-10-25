#include "model.h"

#define TFREEZE 0.0	// degC

float getInfrared( float temp )
{
	float irad;
	float Arad = 203.0;	// W m^-2
	float Brad = 2.0;	// W m^-2 degC^-1

	irad = Arad + Brad * temp;
	return irad;
}

float getAlbedo( float temp )
{
	float albedo;
	float aland = 0.3;
	float aice = 0.7;

	if ( temp > TFREEZE ) {
		albedo = aland;
	} else {
		albedo = aice;
	}
	return albedo;
}

float nextStep( float temp, float dt )
{
	float solcon = 340;	// W m^-2
	float cp = 2.1e8;	// J m^-2 degC^-1
	float ir, alb, dtemp, dum;

	ir = getInfrared( temp );
	alb = getAlbedo( temp );
	//dtemp = ( ( 1 - alb ) * solcon - ir ) / cp;
	//dtemp = log( ( 1 - alb ) * solcon - ir ) - log( cp );

	dum = dt / cp;
	dtemp = ( ( 1 - alb ) * solcon - ir ) * dum;

	return dtemp;
}
