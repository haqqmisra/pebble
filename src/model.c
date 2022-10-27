#include "model.h"

#define TFREEZE 0.0	// degC

void init( float *temp, float *lat, float *xlat, float *dxlat, int size, float tempinit )
{
	int i;

	lat[0]       = -M_PI / 2;
	lat[size+1]  = M_PI / 2;
	xlat[0]      = sinf( lat[0] );
	xlat[size+1] = sinf( lat[size-1] );

	for ( i = 1; i < size+1; ++i ) {
		//lat[i]  = lat[0] + i * M_PI / ( size - 2 );
		lat[i]  = lat[0] + ( 1 + 2 * ( i - 1 ) ) * M_PI / ( 2 * size );
		xlat[i] = sinf( lat[i] );
	}
	for ( i = 0; i < size+1; ++i ) {
		dxlat[i] = fabsf( xlat[i+1] - xlat[i] );
	}
	for ( i = 0; i < size+2; ++i ) {
		temp[i] = tempinit;
	}
	return;
}

void diff( float temp[], float xlat[], float dxlat[], float *t2prime, int size )
{
	float tprime[size+1], tprimeave[size];
	int k;

	for ( k = 0; k < size+1; k++ ) {
		tprime[k] = ( temp[k+1] - temp[k] ) / dxlat[k];
	}
	for ( k = 0; k < size; k++ ) {
		tprimeave[k] = ( tprime[k] * dxlat[k] + tprime[k-1] * dxlat[k-1] ) / ( dxlat[k] + dxlat[k-1] );
		//t2prime[k]   = ( ( ( powf( dxlat[k] / 2, 2 ) ) - ( powf( dxlat[k-1] / 2, 2 ) ) ) * ( 1 - powf( xlat[k], 2 ) ) * tprimeave[k] + ( powf( dxlat[k-1] / 2, 2 ) ) * ( 1 - powf( xlat[k] + ( dxlat[k] / 2 ), 2 ) ) * tprime[k] - ( ( powf( dxlat[k] / 2, 2 ) ) * ( 1 - powf( xlat[k] - ( dxlat[k-1] / 2 ), 2 ) ) ) * tprime[k-1] ) / ( ( dxlat[k] / 2 ) * ( dxlat[k-1] / 2 ) * ( dxlat[k] / 2 + dxlat[k-1] / 2 ) );
		t2prime[k] = 1.0;
	}
	return;
}

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

float getSolcon( float lat, float dt, int niter )
{
	float obliquity = 23.5;
	float solcon = 1360;	// W m^-2

	float halfday, mumean, solar;

	float nsec   = dt * niter;
	float bigG   = 6.67e-11;
	float msun   = 2.e30;
	float Rorbit = 1.496e11;
	float wvel   = sqrtf( ( bigG * msun ) / powf( Rorbit, 3 ) );
	float wt     = wvel * nsec;
	wt     = fmodf( wt, 2 * M_PI );
	float dec    = asinf( -sinf( deg2rad( obliquity ) ) * cosf( wt + M_PI / 2 ) );
	float harg   = tanf( dec ) * tanf( lat );

	if ( harg >= 1 ) {
		halfday = M_PI;
	}
	else if ( harg <= -1 ) {
		halfday = 0.0;
	}
	else {
		halfday = acosf( -tanf( lat ) * tanf( dec ) );
	}

	mumean = halfday * sinf( lat ) * sinf( dec ) + cosf( lat ) * cosf( dec ) * sinf( halfday );
	solar  = ( solcon / M_PI ) * mumean;

	return solar;
}


float nextStep( float temp, float dt, int niter, float lat, float t2p )
{
	//float solcon = 340;	// W m^-2
	float cp     = 5.25e6;	// J m^-2 degC^-1
	float diff   = 0.58;
	float ir, alb, solcon, dtemp, dum, dum2;

	ir     = getInfrared( temp );
	alb    = getAlbedo( temp );
	solcon = getSolcon( lat, dt, niter );

	dum = dt / cp;
	//dum2 = diff * t2p;

	dtemp = ( ( 1 - alb ) * solcon - ir ) * dum;

	return dtemp;
}

void updateAllLat( float *temp, float dt, int niter, int size, float *t2p, float lat[], float xlat[], float dxlat[] )
{
	int i;

	//diff( temp, xlat, dxlat, t2p, size );

	for ( i = 0; i < size; i++ ) {
		temp[i] = temp[i] + nextStep( temp[i], dt, niter, lat[i], t2p[i] );
	}

	return;
}





