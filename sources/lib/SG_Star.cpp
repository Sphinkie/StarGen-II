/* ------------------------------------------------------------------------- */
// File       : SG_Star.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */

#include <math.h>	// pour sqrt()
#include "SG_Star.h"
#include "SG_Utils.h"
#include "SG_Const.h"


/* ------------------------------------------------------------------------- */
/// Constructor. 
/** 
The star is initialized with the Sun characteristics.
*/
/* ------------------------------------------------------------------------- */
SG_Star::SG_Star()
{
	mLum  = 1;
	mMass = 1;
	mR_ecosphere = 1;
	mAge  = 5e9;
	mLife = 10e9;
	mPlanetNumber = 0;
}


/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
SG_Star::~SG_Star()
{}


/* ------------------------------------------------------------------------- */
/// This function set the star parameters randomly.
/* ------------------------------------------------------------------------- */
void SG_Star::setRandomStar()
{
	mMass        = SG_Utils::random_number(0.7, 7);           
	mLum         = this->calculateLuminosity(mMass);          
	mR_ecosphere = this->calculateEcosphere(mLum);     
	mLife        = this->calculateLife();
	mAge         = SG_Utils::random_number(1e9, 6e9); // age of the star: 1 to 6 billions years
	if (mAge>mLife)   mAge = mLife;
}


/* ------------------------------------------------------------------------- */
/// This function set the star mass, and calculate its luminosity, ecosphere, and life.
/**
@param mass The star mass (unit = solar mass)
*/
/* ------------------------------------------------------------------------- */
void SG_Star::setMass(long double mass)
{
	mMass        = mass;
	mLum         = this->calculateLuminosity(mMass);
	mR_ecosphere = this->calculateEcosphere(mLum);
	mLife        = this->calculateLife();
}


/* ------------------------------------------------------------------------- */
/// This function set the star luminosity, and recalculate the ecosphere radius, and life.
/**
Use this function if you want to override the calculated luminosity by a real luminosity.

Luminosity is the total amount of energy that a star radiates into space every second. 
Luminosity depends on both the surface area and the surface temperature of the star,
so that, for example, two stars with the same surface temperature but different 
luminosity must differ in size.

The luminosity of a blackbody (which most stars closely approximate) of temperature T 
and radius R is given by the Stefan-Boltzmann equation: 
 L = 4 pi R² sigma T4 

where sigma is the Stefan-Boltzmann constant (5.67 x 10-8 W/m²/K4).

@param luminosity The luminosity of the star (unit = solar luminosity)
@sa calculateLuminosity setMagnitude
*/
/* ------------------------------------------------------------------------- */
void SG_Star::setLuminosity(long double luminosity)
{
	mLum= luminosity;
	mR_ecosphere = this->calculateEcosphere(mLum);     
	mLife        = this->calculateLife();
}


/* ------------------------------------------------------------------------- */
/// This function set the star ecosphere radius. 
/**
Use this function if you want to override the calculated ecosphere by a real ecosphere radius.
The ecosphere is the region of space around a star that is considered able to support life.
@param ecosphere The ecosphere of the star (unit = AU)
@sa calculateEcosphere
*/
/* ------------------------------------------------------------------------- */
void SG_Star::setEcosphere(long double ecosphere)
{
	mR_ecosphere= ecosphere;
}


/* ------------------------------------------------------------------------- */
/// This function set the age of the star.
/**
The age cannot be higher than teh life of the star.
@param age The age of the star (unit = years)
*/
/* ------------------------------------------------------------------------- */
void SG_Star::setAge(long double age)
{
	mAge = age;
	if (mAge>mLife)   mAge = mLife;
}


/* ------------------------------------------------------------------------- */
/// This function set the star life time.
/**
This is the life duration expected for the star, from its "birth", to its "death".
Use this function if you want to override the calculated life by a real life time.
@param life The life duration of the star (unit = years)
@sa calculateLife
*/
/* ------------------------------------------------------------------------- */
void SG_Star::setLife(long double life)
{
	mLife = life;
}


/* ------------------------------------------------------------------------- */
/// Calculate the luminosity of the star, based on its mass.
/** 
@param mass The mass of the star (unit = solar mass)
@return The luminosity of the star (unit = solar luminosity)
*/
/* ------------------------------------------------------------------------- */
long double SG_Star::calculateLuminosity(long double mass)
{
	long double n;

	if (mass < 1.0)
		n = 1.75 * (mass - 0.1) + 3.325;
	else
		n = 0.5 * (2.0 - mass) + 4.4;
	return(pow(mass, n));
}


/* ------------------------------------------------------------------------- */
/// Calculate the luminosity of the star, based on its bolometric magnitude.
/** 
This function recalculate also the ecosphere radius and life time of the star.

Luminosity (L) is related to bolometric magnitude (Mbol) by the formula: 
	Mbol = 2.5 log (L/Lsun) + 4.72 
	L    = exp((Mbol -4.72)/2.5) * Lsun
	Mbolsun = 4.72

The bolometric magnitude is the magnitude of a star measured across all wavelengths, 
so that it takes into account the total amount of energy radiated. 
If a star is a strong infrared or ultraviolet emitter, its bolometric magnitude will 
differ greatly from its visual magnitude.

@param magnitude The bolometric magnitude of the star 
@sa calculateLuminosity setLuminosity
*/
/* ------------------------------------------------------------------------- */
void SG_Star::setMagnitude(long double magnitude)
{
    mLum = exp((magnitude - 4.72) / 2.5);
	mR_ecosphere = this->calculateEcosphere(mLum);     
	mLife        = this->calculateLife();
}

/* ------------------------------------------------------------------------- */
/// Calculate the ecosphere radius of the star, based on its luminosity.
/** 
The ecosphere is the region of space around a star that is considered able to support life.
@param luminosity The luminosity of the star (unit = solar luminosity).
@return The ecosphere radius, ie: the maximum orbit for a planet that can support life. (unit=AU).
*/
/* ------------------------------------------------------------------------- */
long double SG_Star::calculateEcosphere(long double luminosity)
{
	return(sqrt(luminosity));
}


/* ------------------------------------------------------------------------- */
/// This function calculate the star life time.
/**
The Lifetime is based on the mass/luminosity ratio of the star.
@return The life duration of the star (unit = years)
@sa setLife
*/
/* ------------------------------------------------------------------------- */
long double SG_Star::calculateLife()
{
	return ((mMass / mLum) * 10e9);
}


/*--------------------------------------------------------------------------*/
/// This function estimates the surface temperature for a body (ie: a planet), at a certain distance from the star.
/** 
This is Fogg's eq.19. 
@param  orbit_radius The distance from the star to the body (unit = AU).
@param  albedo       The albedo of the body.
@return The temperature at the surface of this body (Unit = Kelvin)
/*--------------------------------------------------------------------------*/
long double SG_Star::getBodyTemperature(long double orbit_radius, long double albedo)
{
	return(sqrt(mR_ecosphere / orbit_radius)
		  * pow1_4((1.0 - albedo) / (1.0 - EARTH_ALBEDO))
		  * EARTH_AVERAGE_KELVIN);
}


/*--------------------------------------------------------------------------*/
/// This function return the 'Effective Temperature'  of a body.
/** The effective temperature of a planet is the temperature it would have 
if it acted as a black body, absorbing all the incoming radiation, and reradiating
all back to the space. Usually, the effective temperature is different of the
real surface temperature.
- Venus = 227 K
- Earth = 255 K
- Mars  = 217 K
This is Fogg's eq.19. 
@param  orbit_radius The distance from the star to the body (unit = AU).
@param  albedo       The albedo of the body.
@return The Effective Temperature of this body (Unit = Kelvin)
/*--------------------------------------------------------------------------*/
long double SG_Star::getEffectiveTemperature(long double orbit_radius, long double albedo)
{
	return(sqrt(mR_ecosphere / orbit_radius)
		  * pow1_4((1.0 - albedo) / (1.0 - EARTH_ALBEDO))
		  * EARTH_EFFECTIVE_TEMP);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'orbite minimale où peut se former une planete.
/** @return unit = UA */
/* ------------------------------------------------------------------------- */
long double SG_Star::getNearestPlanetOrbit()
{
	return (0.3 * pow1_3(mMass));
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'orbite maximale où peut se former une planete.
/** @return unit = UA */
/* ------------------------------------------------------------------------- */
long double SG_Star::getFarthestPlanetOrbit()
{
	return (50.0 * pow1_3(mMass));
}


/* ------------------------------------------------------------------------- */
/// This function returns the radius of the stellar dust cloud surrounding the star.
/** @return The stellardust cloud radius (unit = UA) */
/* ------------------------------------------------------------------------- */
long double SG_Star::getStellarDustLimit()
{
	return (200.0 * pow1_3(mMass));
}


/* ------------------------------------------------------------------------- */
/// This function returns the orbit of the Nth planet of a planetary system.
/** 
This Orbit is based on the empirical Bode-Titius Law, modified to depend on the star.
This law originally is: Orbit = (3k+4)/10 AU (with k = 0,1,2,4,8,16,etc)
@param index The index of the planet (0..N). Note that a negative index will return a random orbit radius.
@return The orbit of the planet (unit = AU) 
*/
/* ------------------------------------------------------------------------- */
long double SG_Star::getBodePlanetOrbit(int index)
{
	long double orbit;

	if (index<-1) 
	{
		// If index is a negative value, the function returns a random orbit.
		orbit=SG_Utils::random_number(getNearestPlanetOrbit(), getFarthestPlanetOrbit());
	}
	else if (index==-1)
	{
		orbit = (28)*mMass; // pseudo-random: this value fits for Neptune.
	}
	else if (index==0)
	{
		orbit = (0.4)*mMass; // k=0
	}
	else 
	{
		int k= pow(2,index-1);
		orbit = (3*k+4)/10.0*mMass;
	}

	return orbit;
}


/* ------------------------------------------------------------------------- */
/// This function returns the number of planets orbiting around the star.
/* ------------------------------------------------------------------------- */
int SG_Star::getPlanetNumber()
{
	return mPlanetNumber;
}
