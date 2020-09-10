/* ------------------------------------------------------------------------- */
// File       : SG_Star.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#ifndef _SG_STAR_H_
#define _SG_STAR_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <string>

/* ------------------------------------------------------------------------- */
/// The primary star of the planetary system.
/** 
- The star is initialized with the Sun characteristics.
- The star can then be defined with setMass and setAge. Then the other parameters are 
automatically calculated.
- A better manual control of the star characteristics is possible with setLuminosity, 
setEcosphere and setLife.
- The star can also be defined randomly with the setRandomStar function.
*/
/* ------------------------------------------------------------------------- */
class SG_Star
{
	public:
		SG_Star();
		~SG_Star();

		void setRandomStar();
        void setName(std::string name);
        void setMass(long double mass);
        void setLuminosity(long double luminosity);
		void setEcosphere(long double ecosphere);
		void setLife(long double life);
		void setAge(long double age);
		void setMagnitude(long double magnitude);

		long double getBodyTemperature(long double orbit_radius, long double albedo);
		long double getEffectiveTemperature(long double orbit_radius, long double albedo);
		long double getNearestPlanetOrbit();
		long double getBodePlanetOrbit(int index);
		long double getFarthestPlanetOrbit();
		long double getStellarDustLimit();
		int         getPlanetNumber();
        std::string getName();

        std::string mName;            ///< The name of the star
        long double mLum;             ///< The luminosity of the star (unit=solar lum)
        long double mMass;            ///< The mass of the star (unit= solar mass)
		long double mLife;            ///< The total lifetime estimlated for the star (unit=year)
		long double mAge;             ///< The elapsed lifetile of the star (unit=year)
		long double mR_ecosphere;     ///< The radius of the ecosphere (unit=UA)
		int         mPlanetNumber;    ///< The number of planets orbiting around the star.

    private:

		long double calculateLuminosity(long double mass);
		long double calculateEcosphere(long double luminosity);
		long double calculateLife();
};

#endif
