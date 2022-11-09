#ifndef model_h
#define model_h

#include <math.h>
#include "util.h"

void init( float *temp, float *lat, float *x, float *dxlat, float *diff, float *area, int npts, int nbelts, float tempinit );
void updateDiffusion( float temp[], float xlat[], float dxlat[], float diff[], float *thermal, int npts );
float getInfrared( float temp );
float getAlbedo( float temp );
float updateDeclination( float dt, int niter );
float getSolcon( float lat, float dec );
float nextStep( float temp, float lat, float dec, float thermal, float dt );
void updateAllLat( float *temp, float dt, int niter, int npts, float diff[], float *thermal, float lat[], float xlat[], float dxlat[] );
float getMeanTemp( float temp[], float area[], int npts );
float updateMeanTemp( float temp[], float area[], int niter, int npts, float tmean, float *tmeanglobal );
int callYear( int query );

#endif
