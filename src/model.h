#ifndef model_h
#define model_h

#include <math.h>
#include "util.h"

void init( float *temp, float *lat, float *x, float *dxlat, float *diff, int npts, int nbelts, float tempinit );
void updateDiffusion( float temp[], float xlat[], float dxlat[], float diff[], float *thermal, int npts );
float getInfrared( float temp );
float getAlbedo( float temp );
float getSolcon( float lat, float dt, int niter );
float nextStep( float temp, float dt, int niter, float lat, float thermal );
void updateAllLat( float *temp, float dt, int niter, int npts, float diff[], float *thermal, float lat[], float xlat[], float dxlat[] );

#endif
