#ifndef main_h
#define main_h

typedef enum { initializing, ready, running, paused, crash, done } Status;
typedef enum { annual, daily, configure } Display;

static int update( void* userdata );
void reset( void* userdata );
void changeState( Status newstate );

#endif
