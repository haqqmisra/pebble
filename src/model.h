#ifndef model_h
#define model_h

#include <math.h>

float getInfrared( float temp );
float getAlbedo( float temp );
float nextStep( float temp, float dt );

#endif

