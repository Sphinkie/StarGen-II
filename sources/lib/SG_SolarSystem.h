/* ------------------------------------------------------------------------- */
// File       : SG_SolarSystem.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#ifndef _SG_SOLAR_SYSTEM_H_
#define _SG_SOLAR_SYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SG_Star.h"
#include "SG_Planet.h"
#include "SG_Stardust.h"

#define RANDOM -1

/* ------------------------------------------------------------------------- */
/// A Solar System (contains a primary star, several planets, and a dust cloud).
/**
	Librairie de génération d'un système planétaire réaliste à partir d'informations
	sur l'étoile primaire (cad centrale) du système.
*/
/* ------------------------------------------------------------------------- */
class SG_SolarSystem
{
	public:
		SG_SolarSystem(long seed);
		~SG_SolarSystem();
		void setSeed(long seed);
        void generateSystem(std::string  filename);
        void generateSolarSystem(std::string  filename);
        void setStarMass(double mass = 1.0);
        void setStarAge(long double age);
        void setStarLuminosity(long double luminosity =1);
        void setStarBoloMagnitude(long double magnitude);
        void setStarName(std::string name);
        SG_Star* getStar();

    private:
		void calculatePlanets();
		void writePlanets();

		long         mSeed;             ///< Seed pour les éléments aléatoires
		SG_Star*     mSun;              ///< The central star of eh planetary system
		SG_Stardust* mStardust;         ///< The stardust cloud sourrounding the star
        std::string  mFilename;         ///< The name of the file to generate
};

#endif
