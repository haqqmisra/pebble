#ifndef constants_h
#define constants_h

// model settings
#define NYEARS 200
#define TINIT 288

// model parameters
#define TFREEZE 273.16
#define S0 1360		// W m^-2
#define OBLIQUITY 23.5
#define ARAD 199.0	// W m^-
//#define ARAD 203.0	// W m^-
#define BRAD 2.0	// W m^-2 degC^-1
#define ALAND 0.3
#define AICE 0.6
#define D0 0.17		// W m^-2 K^-1
#define CP 2.1e8	// J m^-2 K^-1
//#define CP 3.0e8	// J m^-2 K^-1

// physical constants
#define TFREEZE 273.16	// K
#define BIG_G 6.67e-11	// m^3 kg^-1 s^-2
#define MSTAR 2.e30	// kg (1 solar mass = 2.e30 kg)
#define RORBIT 1.496e11 // m (1 AU = 1.496e11 m)

// model constants
#define STEPS_PER_UPDATE 30
#define NBELTS 18
#define NPTS NBELTS+2

// PlayDate constants
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

#endif
