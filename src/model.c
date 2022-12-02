#include "model.h"
#include "constants.h"

void init( float *temp, float *lat, float *xlat, float *dxlat, float *diff, float *area, int npts, int nbelts, float tempinit )
{
	int i;

	lat[0]       = -M_PI / 2;
	lat[npts-1]  = M_PI / 2;
	xlat[0]      = sinf( lat[0] );
	xlat[npts-1] = sinf( lat[npts-1] );

	for ( i = 1; i < npts - 1; ++i ) {
		//lat[i]  = lat[0] + i * M_PI / ( size - 2 );
		lat[i]  = lat[0] + ( 1 + 2 * ( i - 1 ) ) * M_PI / ( 2 * nbelts );
		xlat[i] = sinf( lat[i] );
		area[i] = fabsf( sinf( lat[i] + M_PI / ( 2 * nbelts ) ) - sinf( lat[i] - M_PI / ( 2 * nbelts ) ) ) / 2;
	}
	for ( i = 0; i < npts - 1; ++i ) {
		dxlat[i] = fabsf( xlat[i+1] - xlat[i] );
	}
	for ( i = 0; i < npts; ++i ) {
		temp[i] = tempinit;
		diff[i] = D0;
	}

	//wvel = sqrtf( ( BIG_G * MSTAR ) / powf( RORBIT, 3 ) );
	// need to cast in log form to maintain float precision
	wvel = expf( 0.5 * ( logf( BIG_G ) + logf( MSTAR ) - 3 * logf( RORBIT ) ) );

	callYear( -1 );

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

	irad = ARAD + BRAD * ( temp - TFREEZE );
	return irad;
}

float getAlbedo( float temp )
{
	float albedo;

	if ( temp >= TFREEZE ) {
		albedo = ALAND;
	}
	else {
		albedo = AICE;
	}
	return albedo;
}

float updateDeclination( float dt, int niter )
{
	static float oldwt;
	float dec;

	float nsec = dt * niter;
	float wt   = wvel * nsec;
	wt         = fmodf( wt, 2 * M_PI );

	if ( niter == 0 ) {
		oldwt = wt;
	}
	if ( wt <= oldwt ) {
		callYear( 1 );
	}
	oldwt = wt;

	dec = asinf( -sinf( deg2rad( OBLIQUITY ) ) * cosf( wt + M_PI / 2 ) );
	return dec;
}

float getSolcon( float lat, float dec )
{
	float halfday, mumean, solar;
	float harg = tanf( dec ) * tanf( lat );

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
	solar  = ( S0 / M_PI ) * mumean;

	return solar;
}


float nextStep( float temp, float lat, float dec, float thermal, float dt )
{
	float ir, alb, solcon, dtemp, dum, dum2;

	ir     = getInfrared( temp );
	alb    = getAlbedo( temp );
	solcon = getSolcon( lat, dec );

	dum = dt / CP;

	dtemp = ( ( 1 - alb ) * solcon - ir + thermal ) * dum;

	return dtemp;
}

void updateAllLat( float *temp, float dt, int niter, int npts, float diff[], float *thermal, float lat[], float xlat[], float dxlat[] )
{
	int i, dec;

	dec = updateDeclination( dt, niter );

	updateDiffusion( temp, xlat, dxlat, diff, thermal, npts );

	for ( i = 1; i < npts - 1; i++ ) {
		temp[i] = temp[i] + nextStep( temp[i], lat[i], dec, thermal[i], dt );
	}
	temp[0]      = temp[1];
	temp[npts-1] = temp[npts-2];

	return;
}

float getMeanTemp( float temp[], float area[], int npts )
 {
	int i;
	int tempsum = 0;

	for ( i = 1; i < npts - 1; i++ ) {
		tempsum += temp[i] * area[i];
	}
	return tempsum;
}

int callYear( int query )
{
	static int yearcount;

	if ( query == -1 ) {
		yearcount = 0;
	}
	else if ( query == 1 ) {
		yearcount++;
	}
	return yearcount;
}

float* callTempLatSum( int query, float newtemp[] )
{
	static float templatsum[NPTS];
	int i;

        if ( query == -1 ) {
		for ( i = 0; i < NPTS; i++ ) {
			templatsum[i] = 0;
		}
        }
	else if ( query == 1 ) {
		for ( i = 0; i < NPTS; i++ ) {
			templatsum[i] += newtemp[i];
		}
	}
	return templatsum;
}

void annualMeanTempLat( float temp[], int niter, int npts, float *tmeanlat )
{
	float *tlatsum;
	int i;

	tlatsum = callTempLatSum( 1, temp );

	for ( i = 0; i < npts; i++ ) {
		tmeanlat[i] = tlatsum[i] / niter;
	}
	return;
}



