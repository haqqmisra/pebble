#include "model.h"

#define TFREEZE 0.0	// degC

void init( float *temp, float *lat, float *xlat, float *dxlat, int size, float tempinit )
{
	int i;

	lat[0]       = -M_PI / 2;
	lat[size-1]  = M_PI / 2;
	xlat[0]      = sinf( lat[0] );
	xlat[size-1] = sinf( lat[size-1] );

	for ( i = 1; i < size-1; ++i ) {
		//lat[i]  = lat[0] + i * M_PI / ( size - 2 );
		lat[i]  = lat[0] + ( 1 + 2 * ( i - 1 ) ) * M_PI / ( 2 * ( size - 2 ) );
		xlat[i] = sinf( lat[i] );
	}
	for ( i = 0; i < size-1; ++i ) {
		dxlat[i] = fabsf( xlat[i+1] - xlat[i] );
	}
	for ( i = 0; i < size; ++i ) {
		temp[i] = tempinit;
	}
	return;
}

//void diff( float temp[], float xlat[], float dxlat[], *float diffusion, int size )
//{
//	float[size-1]
//}



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
	float cp = 5.25e6;	// J m^-2 degC^-1
	float ir, alb, dtemp, dum;

	ir = getInfrared( temp );
	alb = getAlbedo( temp );
	//dtemp = ( ( 1 - alb ) * solcon - ir ) / cp;
	//dtemp = log( ( 1 - alb ) * solcon - ir ) - log( cp );

	dum = dt / cp;
	dtemp = ( ( 1 - alb ) * solcon - ir ) * dum;

	return dtemp;
}
