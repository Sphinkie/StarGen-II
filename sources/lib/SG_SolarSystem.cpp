/* ------------------------------------------------------------------------- */
// File       : SG_SolarSystem.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#include <stdlib.h>            // for rand() and srand()
#include <math.h>              // for sqrt()
#include "SG_SolarSystem.h"
#include "SG_FileWriter.h"
#include "SG_Utils.h"
#include "SG_Const.h"


/* ------------------------------------------------------------------------- */
/// Constructor. 
/* ------------------------------------------------------------------------- */
SG_SolarSystem::SG_SolarSystem(long seed)
{
	SG_Utils::writeLog(">> STARGEN starting ",false);
    SG_Utils::writeLog("Size of float      :"+std::to_string(sizeof(float))      +" bytes: "+std::to_string(sizeof(float)*8)      +" bits");
    SG_Utils::writeLog("Size of long double:"+std::to_string(sizeof(long double))+" bytes: "+std::to_string(sizeof(long double)*8)+" bits");
    SG_Utils::writeLog("Size of short      :"+std::to_string(sizeof(short))      +" bytes: "+std::to_string(sizeof(short)*8)      +" bits");
    SG_Utils::writeLog("Size of int        :"+std::to_string(sizeof(int))        +" bytes: "+std::to_string(sizeof(int)*8)        +" bits");
    SG_Utils::writeLog("Size of long       :"+std::to_string(sizeof(long))       +" bytes: "+std::to_string(sizeof(long)*8)       +" bits");
    SG_Utils::writeLog("Size of long int   :"+std::to_string(sizeof(long int))   +" bytes: "+std::to_string(sizeof(long int)*8)   +" bits");
    SG_Utils::writeLog("Size of double     :"+std::to_string(sizeof(double))     +" bytes: "+std::to_string(sizeof(double)*8)     +" bits");

	mStardust = NULL;
	mSun = new SG_Star();
	this->setSeed(seed);
}


/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
SG_SolarSystem::~SG_SolarSystem()
{
	delete (mSun);
	if (mStardust) delete (mStardust);
}


/* ------------------------------------------------------------------------- */
/// Specify the Seed for the random numbers used for this generation.
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::setSeed(long seed)
{
	mSeed = seed;
	srand(mSeed);
    SG_Utils::writeLog("Seed: " +std::to_string(mSeed));
}


/* ------------------------------------------------------------------------- */
/// Set some data about the primary star: Mass
/** 
@param mass the mass of the star (unit = Solar Mass). The value RANDOM can also be used.
If the data is not defined, the defalut value is the Sun mass (=1)
*/
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::setStarMass(long mass)
{
	if (mass==RANDOM) mass=SG_Utils::random_number(0.7, 7);

	mSun->setMass(mass);
}


/* ------------------------------------------------------------------------- */
/// Set some data about the primary star: Luminosity
/** 
@param luminosity The luminosity of the star (unit = Solar Luminosity). The value RANDOM can also be used.
If the data is not defined, the default value is the Sun luminosity (=1)
*/
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::setStarLuminosity(long luminosity)
{
	if (luminosity==RANDOM) luminosity=SG_Utils::random_number(0.7, 7);
	mSun->setLuminosity(luminosity);
}


/* ------------------------------------------------------------------------- */
/// This function return a pointer on the primary star of the solar system.
/* ------------------------------------------------------------------------- */
SG_Star* SG_SolarSystem::getStar()
{
	return mSun;
}


/* ------------------------------------------------------------------------- */
/// This function generate a planetary system around the primary star.
/** 
@param filename The name of the file to generate (".xml" or ".txt")
*/
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::generateSystem(std::string filename)
{
	mFilename = filename;

	// For information, we log here the temperature that would be on Earth with such a sun...
	long double temp = mSun->getBodyTemperature(1, EARTH_ALBEDO) - FREEZING_POINT_OF_WATER;
    SG_Utils::writeLog("Earth estimate temperature with this sun: "+std::to_string(temp)+"°C");

	/*************************************************/
	// Gestion des étoiles doubles
	// The following is Holman & Wiegert's equation 1 from
	// Long-Term Stability of Planets in Binary Systems
	// The Astronomical Journal, 117:621-628, Jan 1999 
	/*************************************************/
	SG_Planet* SecondaryStar = NULL;	// TODO A gérer 

	if (SecondaryStar) 
	{
		if(SecondaryStar->getMass() > 0.001)
		{
			long double m1 = mSun->mMass;
			long double m2 = SecondaryStar->getMass();
			long double mu = m2 / (m1 + m2);
			long double e = SecondaryStar->getEccentricity();
			long double a = SecondaryStar->getOrbit();
			long double outer_planet_limit = 
				(0.464 + (-0.380 * mu) + (-0.631 * e) +
				(0.586 * mu * e) + (0.150 * pow2(e)) +
				(-0.198 * mu * pow2(e))) * a;
            SG_Utils::writeLog("outer_planet_limit: " +std::to_string(outer_planet_limit));
		}
	}

	/*************************************************/

	// If the mass of the star is too heavy, there is no planets
	// if ((mSun->mMass > 0.2) && (mSun->mMass < 1.5))  // initial value
	if ((mSun->mMass > 0.8) && (mSun->mMass < 5.0))     // DDL values
	{
		// We create a cloud of dust around the star.
		// and we set a limit at 8 planets maximum
		mStardust = new SG_Stardust(mSun, 8);

		mStardust->setDustDensityRatio(1); //TODO: makke some tests with differents ratio

		// The dust is accreted into planets
		mStardust->generatePlanets();
		// We determine the climatic conditions for every planet
		this->calculatePlanets();
	}
	// We write the data in a file
	this->writePlanets();
}


/* ------------------------------------------------------------------------- */
/// Generation of all planets of the system.
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::calculatePlanets()
{
	int index = 0; 
	SG_Planet* planet = NULL;
	if (!mStardust) return;
	planet = mStardust->getPlanet(index++);

	while (planet)
	{
		planet->calculatePlanet();
		// go to next planet
		planet = mStardust->getPlanet(index++);
	}
	// store the number of planets generated.
	mSun->mPlanetNumber = index-1;
}


/* ------------------------------------------------------------------------- */
/// Write all planets of the system in a file.
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::writePlanets()
{
	int format;
	// Analyse the file extension to determine the desired format
    if      (mFilename.find(".xml",1)!=std::string::npos) format=SG_XML;
    else if (mFilename.find(".txt",1)!=std::string::npos) format=SG_TXT;

	SG_FileWriter DescFile(mFilename, mSeed, format);
	// Write the description of the star in the file
	DescFile.writeStarDescription(mSun);
	// Write the planets description in the file.
	int index = 0; 
	SG_Planet* planet = NULL;
	if (mStardust) planet = mStardust->getPlanet(index++);
	while (planet)
	{
		DescFile.writePlanetDescription(planet);
		// go to next planet
		planet = mStardust->getPlanet(index++);
	}

}

/* ------------------------------------------------------------------------- */
/// This function generate the planets of our solar System around the given star.
/* ------------------------------------------------------------------------- */
void SG_SolarSystem::generateSolarSystem(std::string filename)
{
	mFilename = filename;
	SG_Utils::writeLog("Recreating our Solar System :");
	/// Creation of the 10 planets of the Sol-System
	//                                 sun, no, orbit, exen,  mass
	SG_Planet* Mercure = new SG_Planet(mSun, 1, 0.387, 0.206, TOSM(0.0550));
	SG_Planet* Venus   = new SG_Planet(mSun, 2, 0.723, 0.007, TOSM(0.8150));   
	SG_Planet* Earth   = new SG_Planet(mSun, 3, 1.000, 0.017, TOSM(1.0000));    
	SG_Planet* Mars    = new SG_Planet(mSun, 4, 1.524, 0.093, TOSM(0.1074));  
	SG_Planet* Ceres   = new SG_Planet(mSun, 5, 2.767, 0.079, TOSM(0.000134));
	SG_Planet* Jupiter = new SG_Planet(mSun, 6, 5.203, 0.048, TOSM(317.90));
	SG_Planet* Saturn  = new SG_Planet(mSun, 7, 9.539, 0.056, TOSM(95.180));
	SG_Planet* Uranus  = new SG_Planet(mSun, 8, 19.191,0.046, TOSM(14.530));
	SG_Planet* Neptune = new SG_Planet(mSun, 9, 30.061,0.010, TOSM(17.140));
	SG_Planet* Pluto   = new SG_Planet(mSun,10, 39.529,0.248, TOSM(0.0020));
	SG_Utils::writeLog("planets created");
	SG_Utils::writeLog("===============================================");
	mSun->mPlanetNumber = 10;  // 0..9

	// Set if GasGiant or not.
	Mercure->setGasGiant(false);
	Venus  ->setGasGiant(false);
	Earth  ->setGasGiant(false);
	Mars   ->setGasGiant(false);
	Ceres  ->setGasGiant(false);
	Jupiter->setGasGiant(true);
	Saturn ->setGasGiant(true);
	Uranus ->setGasGiant(true);
	Neptune->setGasGiant(true);
	Pluto  ->setGasGiant(false);

	// Set the axial inclination
	Mercure->setAxialTilt(2.00);
	Venus  ->setAxialTilt(177.3);
	Earth  ->setAxialTilt(23.5);
	Mars   ->setAxialTilt(25.2);
	Ceres  ->setAxialTilt(0.10);
	Jupiter->setAxialTilt(3.10);
	Saturn ->setAxialTilt(26.7);
	Uranus ->setAxialTilt(97.9);
	Neptune->setAxialTilt(29.6);
	Pluto  ->setAxialTilt(122.5);

	// Set the dust and gas mass
	Mercure->addDustMass(TOSM(0.055));
	Venus  ->addDustMass(TOSM(0.815));
	Earth  ->addDustMass(TOSM(1.000));
	Mars   ->addDustMass(TOSM(0.1074));
	Ceres  ->addDustMass(TOSM(0.000134));
	Jupiter->addGasMass (TOSM(317.90));
	Saturn ->addGasMass (TOSM(95.180));
	Uranus ->addGasMass (TOSM(13.530));
	Neptune->addGasMass (TOSM(17.140));
	Pluto  ->addDustMass(TOSM(0.0020));

	// Calculate all the planet parameters
	Mercure->calculatePlanet();
	Venus  ->calculatePlanet();
	Earth  ->calculatePlanet();
	Mars   ->calculatePlanet();
	Ceres  ->calculatePlanet();
	Jupiter->calculatePlanet();
	Saturn ->calculatePlanet();
	Uranus ->calculatePlanet();
	Neptune->calculatePlanet();
	Pluto  ->calculatePlanet();
	SG_Utils::writeLog("Planet climate calculated");

	// Write the data in a file.
	int format;
	// Analyse the file extension to determine the desired format
    if      (mFilename.find(".xml",1)!=std::string::npos) format=SG_XML;
    else if (mFilename.find(".txt",1)!=std::string::npos) format=SG_TXT;

	SG_FileWriter DescFile(mFilename, mSeed, format);
	DescFile.writeStarDescription(mSun);
	DescFile.writePlanetDescription(Mercure);
	DescFile.writePlanetDescription(Venus);
	DescFile.writePlanetDescription(Earth);
	DescFile.writePlanetDescription(Mars);
	DescFile.writePlanetDescription(Ceres);
	DescFile.writePlanetDescription(Jupiter);
	DescFile.writePlanetDescription(Saturn);
	DescFile.writePlanetDescription(Uranus);
	DescFile.writePlanetDescription(Neptune);
	DescFile.writePlanetDescription(Pluto);
	SG_Utils::writeLog("Output file created " + mFilename);

	// delete the planets
	delete(Mercure);
	delete(Venus);
	delete(Earth);
	delete(Mars);
	delete(Ceres);
	delete(Jupiter);
	delete(Saturn);
	delete(Uranus);
	delete(Neptune);
	delete(Pluto);
}
