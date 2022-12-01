#ifndef main_h
#define main_h

#define NUMSTATUS 6
#define NUMDISPLAY 3

typedef enum { initializing, ready, running, paused, crash, done } Status;
typedef enum { annual, daily, configure } Display;

static int update( void* userdata );
void reset( void* userdata );
void changeState( Status newstate );

#endif
