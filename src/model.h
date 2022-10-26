#ifndef model_h
#define model_h

#include <math.h>

void init( float *temp, float *lat, float *x, float *dxlat, int size, float tempinit );
float getInfrared( float temp );
float getAlbedo( float temp );
float nextStep( float temp, float dt );

#endif

