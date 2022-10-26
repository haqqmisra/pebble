#ifndef util_h
#define util_h

#include <math.h>

float deg2rad( float nrad );


void strrev(char *arr, int start, int end);
char *itoa(int number, char *arr, int base);

void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char* res, int afterpoint);

int n_tu(int number, int count);
void float_to_string(float f, char r[]);


#endif

