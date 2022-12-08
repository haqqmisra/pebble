#ifndef json_h
#define json_h

#include "pd_api.h"

void writefile( void* userdata, const char* str, int len );

/*
json_encoder encoder =
{
	.startArray = startArray,
	.addArrayMember = addArrayMember,
	.endArray = endArray,
	.startTable = startTable,
	.addTableMember = addTableMember,
	.endTable = endTable,
	.writeNull = writeNull,
	.writeFalse = writeFalse,
	.writeTrue = writeTrue,
	.writeInt = writeInt,
	.writeDouble = writeDouble,
	.writeString = writeString,
} json_encoder;
*/

#endif

