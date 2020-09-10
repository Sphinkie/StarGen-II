/* ------------------------------------------------------------------------- */
// File       : SG_Planet.h
// Project    : Star Gen
// Author C++ : David de Lorenzo
// Author C   : Jim Burrows (see Credits.txt)
/* ------------------------------------------------------------------------- */
#ifndef _STARGEN_PLANET_H_
#define _STARGEN_PLANET_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SG_Star.h"
#include "SG_Atmosphere.h"

/* ------------------------------------------------------------------------- */
/// A planet (generation and properties).
/** 
Planet Type is defined as follows:
- Rock       : Solid planet without atmosphere (ex: Mercure, Pluto)
- Ice        : Planet mostly covered with water-ice (more than 95%) 
- Water      : Planet with atmosphere, and mostly covered with water-ocean (more than 95%)
- Terrestrial: Planet with atmosphere, and more than 5% of water coverage (ex: Earth)
- Venusian   : Planet with atmosphere, few or no water, and a very high average surface temperature
- Martian    : Planet with atmosphere, few or no water, and a temperate surface temperature (ex: Mars)
- GasGiant   : Regular Gas planet (ex: Jupiter).
- SubGasGiant: Gas planet with a mass < 20 EM.
- SubSubGasGiant: Gas planet with a gas part lower than 20% of its total mass (gas dwarf).
- Asteroids  : Solid body with a very small mass.
*/
/* ------------------------------------------------------------------------- */
class SG_Planet
{
	friend class SG_Atmosphere; // SG_Atmosphere has access to protected members of SG_Planet
	friend class SG_File;       // SG_File has access to protected members of SG_Planet

	public:
		
		SG_Planet(SG_Star* sun, int planet_no, long double a, long double e, long double mass);
		~SG_Planet();
		void calculatePlanet();
		long double getOrbit();
		long double getEccentricity();
		long double getMass();
		void setOrbit(long double a);
		void setEccentricity(long double e);
		void setAxialTilt(long double tilt);
		void setMass(long double mass);
		void addDustMass(long double mass);
		void addGasMass(long double mass);
		void setGasGiant(bool giant);
		void setPlanetNumber(int number);

    private:
        enum planet_type
		{	tUnknown,     tRock,     tVenusian,
			tTerrestrial, tGasGiant, tMartian,
			tWater,       tIce,      tSubGasGiant,
			tSubSubGasGiant,         tAsteroids
		}; 

		void calculateGasPlanet();
		void calculateSolidPlanet();

		// Physical calculations
        long double  estimateDensity(long double orbit_radius, long double r_ecosphere, bool gas_giant);
		long double  calculateOrbitPeriod(long double star_mass);
		long double  calculateExoTemperature();
		int          calculateInclination(long double orb_radius);
		planet_type  calculateType(bool GasPlanet);
		long double  calculateDayLength();
		bool         calculateFaceLocking();
		long double  calculateGravity(long double distance);
		long double  getKothariRadius(long double mass, bool giant, int zone);
		int          getOrbitalZone(long double luminosity, long double orbit_radius);
		bool         calculateMetal();

		// Climatic calculations
		void         calculateGlobalClimate();
		void         estimateSurfaceClimate();
		bool         calculateGreenhouseEffect();
		void         calculateTemperatureRange();
		long double  calculateBoilingPoint(long double pressure);
		long double  calculateHydroFraction(long double volatile_gas_inventory);
		long double  calculateIceFraction(long double hydro_fraction);
		long double  calculateOpacity(long double molecular_weight, long double pressure);
		long double  calculateAlbedo(long double water_fraction, 
			                         long double cloud_fraction, 
			                         long double ice_fraction);
		long double  calculateCloudFraction(long double smallest_MW_retained, 
			                                long double hydro_fraction);
		long double  calculateGreehouseRiseTemp(long double optical_depth, 
			                                    long double effective_temp);
		void         calculateSurfaceClimate(long double last_water,long double last_clouds,
			                                 long double last_ice, 
			                                 long double last_temp, 
			                                 long double last_albedo);
		// Atmospheric calculations
		long double  calculateEscapeVelocity();
		long double  estimateMinimalMoleculeWeight();
		long double  calculateMinimalMoleculeWeight();
		long double  calculatePressure(long double radius);
		long double  getRMSvelocity(long double molecular_weight);
		long double  getGasLife(long double molecular_weight);
		long double  calculateVolatilGasRatio(long double escape_vel,
			                                  long double stellar_mass,
			                                  bool greenhouse_effect,
			                                  bool accreted_gas);

		// Mathematical calculations
		long double soft(long double v, long double max, long double min);
		long double getSphereMass(long double density, long double radius);
		long double getSphereRadius(long double mass,  long double density);
		long double getSphereDensity(long double mass, long double radius);

    private:
		// Global data
		int         mPlanet_no;         ///< Planet index
		planet_type mType;              ///< Code for the planet's type
		bool        mEarthlike;         ///< TRUE if the planet is very similar to Earth
		bool        mGas_giant;         ///< TRUE if the planet is a gas giant
		bool        mGas_planet;        ///< TRUE if the planet is a gas planet
		// Physical Parameters
		long double m_a;                ///< Semi-major axis of the orbit (unit = AU)
		long double m_e;                ///< Eccentricity of the orbit (unitless)
		long double mAxial_tilt;        ///< Planet inclination (unit = degrees)
		long double mMass;              ///< Planet mass (unit = solar masses)
		long double mDust_mass;         ///< Planet mass, ignoring gas (unit = solar masses)
		long double mGas_mass;          ///< Planet mass, ignoring dust (unit = solar masses)
		long double mCore_radius;       ///< Radius of the rocky core (unit = km)
		long double mRadius;            ///< Planet equatorial radius (unit = km)
		long double mDensity;           ///< Planet density (unit = g/cc)
		int         mOrbit_zone;        ///< The 'zone' of the planet
		long double mOrbit_period;      ///< Duration of the local year (unit = earth days)
		long double mDay;               ///< Duration of the local day (hours)
		bool        mResonant_period;   ///< TRUE if in resonant rotation
		bool        mFaceLocked;        ///< The same side of the planet is always facing the star
		long double mEsc_velocity;      ///< Escape velocity (unit = cm/sec)
		int         mBreathe;           ///< Atmosphere breathability        
		long double mSurf_grav;         ///< Surface Gravity (unit= Earth gravity)
		long double mSurf_temp;         ///< Surface temperature (unit = Kelvin)
		long double mSurf_pressure;     ///< Surface pressure (unit = millibars mb)
		long double mMolec_weight;      ///< The smallest molecular weight retained (unit=gram)
		long double mVolatile_gas_inventory;
		long double mBoiling_point;     ///< The boiling point of water (Kelvin)
		long double mAlbedo;            ///< Albedo of the planet
		long double mExospheric_temp;   ///< Temperature (unit = Kelvin)
		bool        mGreenhouse_effect; ///< Runaway greenhouse effect ?
		long double mGreenhouse_rise;   ///< Temperature rise due to greenhouse effect
		long double mHigh_temp;         ///< Day-time temperature
		long double mLow_temp;          ///< Night-time temperature
		long double mMax_temp;          ///< Temperature in a Summer Day (Kelvin)
		long double mMin_temp;          ///< Temperature in a Winter Night (Kelvin)
		long double mHydrosphere;       ///< Fraction of surface covered with water
		long double mCloud_cover;       ///< Fraction of surface covered with clouds
		long double mIce_cover;         ///< Fraction of surface covered with ice
		bool        mMetal;             ///< Presence of metal in the planet
		SG_Star*    mPrimaryStar;       ///< The main Star of the solar system
		SG_Atmosphere* mAtmosphere;     ///< The planet's atmosphere
};

#endif

