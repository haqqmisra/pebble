#ifndef model_h
#define model_h

#include <math.h>
#include "util.h"

void init( float *temp, float *lat, float *x, float *dxlat, int size, float tempinit );
void diff( float temp[], float xlat[], float dxlat[], float *t2prime, int size );
float getInfrared( float temp );
float getAlbedo( float temp );
float getSolcon( float lat, float dt, int niter );
float nextStep( float temp, float dt, int niter, float lat, float t2p );
void updateAllLat( float *temp, float dt, int niter, int size, float *t2p, float lat[], float xlat[], float dxlat[] );

#endif
