#include "model.h"

#define TFREEZE 0.0	// degC

void init( float *temp, float *lat, float *xlat, float *dxlat, float *diff, int npts, int nbelts, float tempinit )
{
	int i;
	float d0 = 0.58;

	lat[0]          = -M_PI / 2;
	lat[npts-1]     = M_PI / 2;
	xlat[0]         = sinf( lat[0] );
	xlat[npts-1]    = sinf( lat[npts-1] );

	for ( i = 1; i < npts - 1; ++i ) {
		//lat[i]  = lat[0] + i * M_PI / ( size - 2 );
		lat[i]     = lat[0] + ( 1 + 2 * ( i - 1 ) ) * M_PI / ( 2 * nbelts );
		xlat[i]    = sinf( lat[i] );
	}
	for ( i = 0; i < npts - 1; ++i ) {
		dxlat[i] = fabsf( xlat[i+1] - xlat[i] );
	}
	for ( i = 0; i < npts; ++i ) {
		temp[i] = tempinit;
		diff[i] = d0;
	}
	return;
}

void updateDiffusion( float temp[], float xlat[], float dxlat[], float diff[], float *thermal, int npts )
{
	float tprime, t2prime, dprime, xpow2, dum1, dum2, dum3, dum4;
	int i;

	for ( i = 1; i < npts - 1; i++ ) {
		tprime = ( temp[i+1] - temp[i-1] ) / ( dxlat[i] + dxlat[i-1] );

		dum1    = temp[i+1] - 2 * temp[i] + temp[i-1];
		dum2    = powf( dxlat[i], 2 ) / 2 + powf( dxlat[i-1], 2 ) / 2;
		t2prime = dum1 / dum2;

		dprime = ( diff[i+1] - diff[i-1] ) / ( dxlat[i] + dxlat[i-1] );

		xpow2      = ( 1 - powf( xlat[i], 2 ) );
		dum3       = xpow2 * t2prime * diff[i];
		dum4       = xpow2 * dprime - 2 * xlat[i] * diff[i];
		thermal[i] = dum3 + tprime * dum4;
	}
	return;
}

float getInfrared( float temp )
{
	float irad;
	float Arad = 203.0;	// W m^-2
	float Brad = 2.0;	// W m^-2 degC^-1
	float KtoC = 273.16;

	irad = Arad + Brad * ( temp - KtoC );
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
	float obliquity = 0.0;
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


float nextStep( float temp, float dt, int niter, float lat, float thermal )
{
	float cp     = 5.25e6;	// J m^-2 degC^-1
	float ir, alb, solcon, dtemp, dum, dum2;

	ir     = getInfrared( temp );
	alb    = getAlbedo( temp );
	solcon = getSolcon( lat, dt, niter );

	dum = dt / cp;

	dtemp = ( ( 1 - alb ) * solcon - ir + thermal ) * dum;
	//dtemp = ( ( 1 - alb ) * solcon - ir ) * dum;

	return dtemp;
}

void updateAllLat( float *temp, float dt, int niter, int npts, float diff[], float *thermal, float lat[], float xlat[], float dxlat[] )
{
	int i;

	updateDiffusion( temp, xlat, dxlat, diff, thermal, npts );

	for ( i = 1; i < npts - 1; i++ ) {
		temp[i] = temp[i] + nextStep( temp[i], dt, niter, lat[i], thermal[i] );
	}
	temp[0]      = temp[1];
	temp[npts-1] = temp[npts-2];

	return;
}
