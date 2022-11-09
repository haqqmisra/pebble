#include "model.h"

#define TFREEZE 0.0	// degC

enum yearquery { initialize, print, add };


void init( float *temp, float *lat, float *xlat, float *dxlat, float *diff, float *area, int npts, int nbelts, float tempinit )
{
	int i;
	float d0 = 0.38;

	lat[0]          = -M_PI / 2;
	lat[npts-1]     = M_PI / 2;
	xlat[0]         = sinf( lat[0] );
	xlat[npts-1]    = sinf( lat[npts-1] );

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
		diff[i] = d0;
	}

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
	float Arad = 203.0;	// W m^-2
	float Brad = 2.0;	// W m^-2 degC^-1
	float KtoC = 273.16;

	irad = Arad + Brad * ( temp - KtoC );
	return irad;
}

float getAlbedo( float temp )
{
	float albedo;
	float aland = 0.28;
	float aice = 0.7;

	if ( temp > TFREEZE ) {
		albedo = aland;
	} else {
		albedo = aice;
	}
	return albedo;
}

float updateDeclination( float dt, int niter )
{

	static float oldwt;

	float obliquity = 23.5;
	float dec;

	float nsec   = dt * niter;
	float bigG   = 6.67e-11;
	float msun   = 2.e30;
	float Rorbit = 1.496e11;
	float wvel   = sqrtf( ( bigG * msun ) / powf( Rorbit, 3 ) );
	float wt     = wvel * nsec;
	wt           = fmodf( wt, 2 * M_PI );

	if ( niter == 0 ) {
		oldwt = wt;
	}
	if ( wt <= oldwt ) {
		callYear( 1 );
	}
	oldwt = wt;

	dec    = asinf( -sinf( deg2rad( obliquity ) ) * cosf( wt + M_PI / 2 ) );

	return dec;
}

float getSolcon( float lat, float dec )
{
	float solcon = 1360;	// W m^-2

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
	solar  = ( solcon / M_PI ) * mumean;

	return solar;
}


float nextStep( float temp, float lat, float dec, float thermal, float dt )
{
	//float cp     = 5.25e6;	// J m^-2 degC^-1
	float cp     = 2.1e8;	// J m^-2 degC^-1
	float ir, alb, solcon, dtemp, dum, dum2;

	ir     = getInfrared( temp );
	alb    = getAlbedo( temp );
	solcon = getSolcon( lat, dec );

	dum = dt / cp;

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

float updateMeanTemp( float temp[], float area[], int niter, int npts, float tmean, float *tmeanglobal )
{
	int i;
	float told, tnew;

	told  = tmean * niter;
	tnew  = getMeanTemp( temp, area, npts );
	tmean = ( told + tnew ) / ( niter + 1 );
	tmean = newPrecision( tmean, 3 );

	for ( i = 1; i < npts - 1; i++ ) {
		told = tmeanglobal[i] * niter;
		tnew = temp[i];
		tmeanglobal[i] = ( told + tnew ) / ( niter + 1 );
		tmeanglobal[i] = newPrecision( tmeanglobal[i], 3 );
	}
	return tmean;
}

int callYear( int query )
{
	static int yearcount;

	if ( query == -1  ) {
		yearcount = 0;
	}
	else if ( query == 1 ) {
		yearcount++;
	}
	return yearcount;
}





