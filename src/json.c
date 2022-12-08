#include "json.h"

void writefile( void* userdata, const char* str, int len )
{
        PlaydateAPI* pd = userdata;
	pd->file->write( (SDFile*)userdata, str, len );
}
