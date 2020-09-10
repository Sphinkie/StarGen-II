/* ------------------------------------------------------------------------- */
// File       : SG_const.h
// Project    : StarGen 2
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#ifndef _SG_CONST_H_
#define _SG_CONST_H_

#if 0
 #ifdef LONG_RAND
 #define RAND_MAX		(2147483647.0)
 #else
 #define RAND_MAX		(32767.0)
 #endif
#endif


#ifndef PI
#define PI	(3.1415926536)
#endif

/// Conversion coeffs
#define RADIANS_PER_ROTATION	(6.2831853072)  /* 2.0 * PI                 */
#define SECONDS_PER_HOUR		(3600.0)
#define CM_PER_AU				(1.495978707e13)/* number of cm in an AU	*/
#define CM_PER_KM				(1.0E5)			/* number of cm in a km		*/
#define KM_PER_AU				(CM_PER_AU / CM_PER_KM)
#define CM_PER_METER			(100.0)
#define MILLIBARS_PER_BAR		(1000.00)

#define ECCENTRICITY_COEFF		 (0.077)		/* Dole's was 0.077			*/
#define PROTOPLANET_MASS		 (1.0e-15)		/* Units of solar masses	*/

/// Sun data
#define SOLAR_MASS_IN_GRAMS		   (1.989e33)	/* Units of grams			*/
#define SOLAR_MASS_IN_KILOGRAMS    (1.989e30)	/* Units of kg				*/
#define SOLAR_MASS_IN_EARTH_MASSES (332775.64)  /* Units of Earth Mass      */
#define SOLAR_RADIUS_IN_M          (6.96e8)     /* Units of meters          */
#define SOLAR_RADIUS_IN_KM         (696000)     /* Units of kilometers      */
#define SOLAR_RADIUS_IN_AU         (4.65e-3)    /* Units of AU              */
#define SOLAR_TEMPERATURE          (6000)       /* Units of degrees Kelvin  */
#define TOEM(x)    (x)*SOLAR_MASS_IN_EARTH_MASSES /* Convert from SM to EM  */
#define TOSM(x)    (x)/SOLAR_MASS_IN_EARTH_MASSES /* Convert from EM to SM  */

/// Earth data
#define FREEZING_POINT_OF_WATER (273.15)		/* Units of degrees Kelvin	*/
#define BOILING_POINT_OF_WATER  (373.35)		/* Units of degrees Kelvin	*/
#define CHANGE_IN_EARTH_ANG_VEL (-1.3E-15)		/* Units of radians/sec/year*/
#define EARTH_MASS_IN_GRAMS		(5.977E27)		/* Units of grams			*/
#define EARTH_RADIUS			(6.378E8)		/* Units of cm				*/
#define EARTH_DENSITY			(5.52)			/* Units of g/cc			*/
#define EARTH_RADIUS_IN_KM		(6378.0)		/* Units of km				*/
#define EARTH_ACCELERATION		(980.7)			/* Units of cm/sec2			*/
#define EARTH_AXIAL_TILT		(23.4)			/* Units of degrees			*/
#define EARTH_EXOSPHERE_TEMP	(1273.0)		/* Units of degrees Kelvin	*/
#define EARTH_EFFECTIVE_TEMP	(255.0)			/* Units of degrees Kelvin (was 250) */
#define EARTH_WATER_MASS_PER_AREA	 (3.83e15)	/* grams per square km		*/
#define EARTH_SURF_PRES_IN_MILLIBARS (1013.25)
#define EARTH_SURF_PRES_IN_BARS (1.01325)
#define EARTH_SURF_PRES_IN_MMHG	(760.)			/* Dole p. 15				*/
#define EARTH_SURF_PRES_IN_PSI	(14.696)		/* Pounds per square inch	*/
#define EARTH_CONVECTION_FACTOR (0.43)			/* from Hart, eq.20			*/
#define EARTH_AVERAGE_CELSIUS   (14.0)			/* Average Earth Temperature*/
#define EARTH_AVERAGE_KELVIN    (EARTH_AVERAGE_CELSIUS + FREEZING_POINT_OF_WATER)
#define EARTH_DAYS_IN_A_YEAR	(365.256)		/* Earth days per Earth year*/

// Atmosphere calculation
#define MMHG_TO_MILLIBARS (EARTH_SURF_PRES_IN_MILLIBARS / EARTH_SURF_PRES_IN_MMHG)
#define PSI_TO_MILLIBARS  (EARTH_SURF_PRES_IN_MILLIBARS / EARTH_SURF_PRES_IN_PSI)
#define PPM_PRESSURE      (EARTH_SURF_PRES_IN_MILLIBARS / 1000000.) /* Parts per million */
#define MMHG_TO_PPM       (1000000 / EARTH_SURF_PRES_IN_MMHG)

#define H20_ASSUMED_PRESSURE   (47. * MMHG_TO_MILLIBARS)  /* Dole, p. 15    */
#define MIN_O2_IPP             (72. * MMHG_TO_MILLIBARS)  /* Dole, p. 15	*/
#define MAX_HABITABLE_PRESSURE (118 * PSI_TO_MILLIBARS)   /* Dole, p. 16	*/

#define GAS_RETENTION_THRESHOLD (6.0)			/* ratio of esc vel to RMS vel */
#define ASTEROID_MASS_LIMIT		(0.001)			/* Units of Earth Masses	*/
#define CLOUD_COVERAGE_FACTOR	(1.839e-8)		/* Km2/kg					*/
#define ICE_ALBEDO				(0.7)
#define CLOUD_ALBEDO			(0.52)
#define GAS_GIANT_ALBEDO		(0.5)			/* albedo of a gas giant	*/
#define AIRLESS_ICE_ALBEDO		(0.5)
#define EARTH_ALBEDO			(0.3)			/* was .33 for a while */
#define GREENHOUSE_TRIGGER_ALBEDO (0.20)
#define ROCKY_ALBEDO			(0.15)
#define ROCKY_AIRLESS_ALBEDO	(0.07)
#define WATER_ALBEDO			(0.04)

#define GRAV_CONSTANT			(6.672e-8)		/* units of dyne cm2/gram2	*/
#define MOLAR_GAS_CONST			(8314.41)		/* units: g*m2/(sec2*K*mol) */
#define K						(50.0)			/* K = gas/dust ratio		*/
#define B						(1.2e-5)		/* Used in Crit_mass calc	*/
#define DUST_DENSITY_COEFF		(2.0e-3)		/* A in Dole's paper		*/
#define ALPHA					(5.0)			/* Used in density calcs	*/
#define N						(3.0)			/* Used in density calcs	*/
#define J						(1.46e-19)		/* Used in day-length calcs (cm2/sec2 g) */

#ifdef HUGE_VAL
#define INCREDIBLY_LARGE_NUMBER HUGE_VAL
#else
#define INCREDIBLY_LARGE_NUMBER (9.9999e37)
#endif

/*	Now for a few molecular weights (used for RMS velocity calcs):	   */
/*	This table is from Dole's book "Habitable Planets for Man", p. 38  */

#define ATOMIC_HYDROGEN			(1.0)	/* H   */
#define MOL_HYDROGEN			(2.0)	/* H2  */
#define HELIUM					(4.0)	/* He  */
#define ATOMIC_NITROGEN			(14.0)	/* N   */
#define ATOMIC_OXYGEN			(16.0)	/* O   */
#define METHANE					(16.0)	/* CH4 */
#define AMMONIA					(17.0)	/* NH3 */
#define WATER_VAPOR				(18.0)	/* H2O */
#define NEON					(20.2)	/* Ne  */
#define MOL_NITROGEN			(28.0)	/* N2  */
#define CARBON_MONOXIDE			(28.0)	/* CO  */
#define NITRIC_OXIDE			(30.0)	/* NO  */
#define MOL_OXYGEN				(32.0)	/* O2  */
#define HYDROGEN_SULPHIDE		(34.1)	/* H2S */
#define ARGON					(39.9)	/* Ar  */
#define CARBON_DIOXIDE			(44.0)	/* CO2 */
#define NITROUS_OXIDE			(44.0)	/* N2O */
#define NITROGEN_DIOXIDE		(46.0)	/* NO2 */
#define OZONE					(48.0)	/* O3  */
#define SULPH_DIOXIDE			(64.1)	/* SO2 */
#define SULPH_TRIOXIDE			(80.1)	/* SO3 */
#define KRYPTON					(83.8)	/* Kr  */
#define XENON					(131.3) /* Xe  */

//	Atomic numbers, are used as a gas ID key

#define AN_H	1
#define AN_HE	2
#define AN_C	6
#define AN_N	7
#define AN_O	8
#define AN_F	9
#define AN_NE	10
#define AN_P	15
#define AN_S	16
#define AN_CL	17
#define AN_AR	18
#define AN_FE	26
#define AN_NI	28
#define AN_CU	29
#define AN_BR	35
#define AN_KR	36
#define AN_I	53
#define AN_XE	54
#define AN_HG	80
#define AN_AT	85
#define AN_RN	86
#define AN_FR	87

#define AN_NH3	900
#define AN_H2O	901
#define AN_CO2	902
#define AN_O3	903
#define AN_CH4	904
#define AN_SO2	905
#define AN_CH3CH2OH	906
#define AN_CO	907

/*	The following defines are used in the kothari_radius function 	*/
#define A1_20					(6.485e12)		/* All units are in cgs system.	 */
#define A2_20					(4.0032e-8)		/*	 ie: cm, g, dynes, etc.		 */
#define BETA_20					(5.71e12)

#define JIMS_FUDGE				(1.004)

/*	 The following defines are used in determining the fraction of a planet	 */
/*	covered with clouds in function cloud_fraction							 */
#define Q1_36					(1.258e19)		/* grams	*/
#define Q2_36					(0.0698)		/* 1/Kelvin */

/// Mathematical macros
#define pow2(a)    ((a) * (a))
#define pow3(a)    ((a) * (a) * (a))
#define pow4(a)    ((a) * (a) * (a) * (a))
#define pow1_2(a)  sqrt(a)
#define pow1_3(a)  pow(a,(0.3333333))
#define pow2_3(a)  pow(a,(0.6666666))
#define pow4_3(a)  pow(a,(1.3333333))
#define pow1_4(a)  sqrt(sqrt(a))
#define pow1_8(a)  pow(a,0.125)


#endif   
