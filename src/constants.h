#ifndef constants_h
#define constants_h

// model settings
#define NYEARS 10
#define TINIT 288
#define NITERMAX 90000
#define DT 8.64e4	// seconds (1 day = 8.64e4 seconds)

// model parameters
#define TFREEZE 273.16
#define S0 1360		// W m^-2
#define OBLIQUITY 23.5
//#define ARAD 199.0	// W m^-
#define ARAD 203.3	// W m^-
#define BRAD 2.09	// W m^-2 degC^-1
#define ALAND 0.291
#define AICE 0.663
//#define ALAND 0.3
//#define AICE 0.6
#define D0 0.38		// W m^-2 K^-1
#define CP 2.1e8	// J m^-2 K^-1
//#define CP 3.0e8	// J m^-2 K^-1

// physical constants
#define TFREEZE 273.16	// K
#define BIG_G 6.6732e-11	// m^3 kg^-1 s^-2
//#define MSTAR 2.e30	// kg (1 solar mass = 2.e30 kg)
#define MSTAR 1.9891e30	// kg (1 solar mass = 2.e30 kg)
//#define RORBIT 1.496e11 // m (1 AU = 1.496e11 m)
#define RORBIT 1.495978707e11 // m (1 AU = 1.496e11 m)

// model constants
#define STEPS_PER_UPDATE 30
#define NBELTS 18
#define NPTS NBELTS+2
#define NUMUNITS 2
#define NPTSMAX 75

// graphics constants
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define PLOTX 112
#define PLOTY 20
#define PLOTDY 100
#define PLOTWIDTH 270
#define PLOTHEIGHT 80
#define IGNORE -9999
#define STRLEN 32
#define NUMXTICKS 6
#define NUMYTICKS 4
#define VARSPACE 9

#endif
