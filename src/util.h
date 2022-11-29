#ifndef util_h
#define util_h

#include <math.h>

float deg2rad( float nrad );
float newPrecision( float n, float i );
int n_tu( int number, int count );


void reverse(char* str, int len);
int intToStr(int x, char* str, int d);
void ftoa(float n, char* res, int afterpoint);



void float_to_string( float f, char r[] );
void strrev( char *arr, int start, int end );
char *int_to_string( int number, char *arr, int base );

#endif

