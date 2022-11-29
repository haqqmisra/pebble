#include "util.h"
#include <inttypes.h>

// Utility Functions for PEBBLE

float deg2rad( float nrad )
{
	return nrad * 180.0 / M_PI;
}


float newPrecision(float n, float i)
{
    return floorf( powf( 10, i ) * n ) / powf( 10, i );
}




// Reverses a string 'str' of length 'len'
void reverse( char* str, int len )
{
	int i = 0, j = len - 1, temp;
	while ( i < j ) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
	return;
}


// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char* str, int d)
{
	int i = 0;
	while ( x ) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d) {
		str[i++] = '0';
	}

	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = intToStr(ipart, res, 0);

	// check for display option after point
	if ( afterpoint != 0 ) {
		res[i] = '.'; // add dot
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		fpart = fpart * powf( 10, afterpoint );
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}


/*** Convert float to string ***/

int n_tu( int number, int count )
{
	int result = 1;

	while( count-- > 0 ) {
		result *= number;
	}
	return result;
}

void float_to_string( float f, char r[] )
{
	int length, length2, i, number, position, sign, trailing;
	float number2;

	sign = -1;   // -1 == positive number
	if ( f < 0 ) {
		sign = '-';
		f *= -1;
	}

	number2 = f;
	number = f;
	length = 0;  // Size of decimal part
	length2 = 0; // Size of tenth

	/* Calculate length2 tenth part */
	while( ( number2 - (float)number ) != 0.0 && !( ( number2 - (float)number ) < 0.0 ) ) {
		number2 = f * ( n_tu( 10.0, length2 + 1 ) );
		number = number2;
		length2++;
	}

	/* Calculate length decimal part */
	for ( length = (f > 1) ? 0 : 1; f > 1; length++ ) {
		f /= 10;
	}

	position = length;
	length = length + 1 + length2;
	number = number2;
	if ( sign == '-' ) {
		length++;
		position++;
	}

	/* trailing zeros for integers */
	if ( position == length - 1 ) {
		length++;
		trailing = 1;
	}

	for ( i = length; i >= 0 ; i-- ) {
		if ( i == (length) ) {
			r[i] = '\0';
		}
		else if ( trailing == 1 ) {
			r[i] = '0';
			trailing = 0;
		}
		else if ( i == (position) ) {
			r[i] = '.';
		}
		else if ( sign == '-' && i == 0 )
			r[i] = '-';
		else {
			r[i] = (number % 10) + '0';
			number /=10;
		}
	}
}


/*** Convert integer to string ***/

void strrev(char *arr, int start, int end)
{
    char temp;

    if (start >= end)
        return;

    temp = *(arr + start);
    *(arr + start) = *(arr + end);
    *(arr + end) = temp;

    start++;
    end--;
    strrev(arr, start, end);
}

char *int_to_string( int number, char *arr, int base )
{
    int i = 0, r, negative = 0;

    if (number == 0)
    {
        arr[i] = '0';
        arr[i + 1] = '\0';
        return arr;
    }

    if (number < 0 && base == 10)
    {
        number *= -1;
        negative = 1;
    }

    while (number != 0)
    {
        r = number % base;
        arr[i] = (r > 9) ? (r - 10) + 'a' : r + '0';
        i++;
        number /= base;
    }

    if (negative)
    {
        arr[i] = '-';
        i++;
    }

    strrev(arr, 0, i - 1);

    arr[i] = '\0';

    return arr;
}
